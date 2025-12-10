#include "widget.h"
#include "ui_widget.h"
#include <QDateTime>
#include <QString>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <string>
#include <sol/sol.hpp>

#if defined(_M_AMD64)
#pragma comment(lib, "lua51.lib")
#elif defined(_M_ARM64)
#pragma comment(lib, "lua51_ARM64.lib")
#endif

class LuaEnvManager
{
private:
    sol::state lua;
    sol::environment shared_env;
    std::unordered_map<std::string, sol::environment> modules;
    QTextEdit* LuaPrint_Text;

    sol::environment create_shared_env()
    {
        sol::environment env(lua, sol::create);
        env["log"] = [this](const std::string& msg)
        {
            HandleLuaOutput("SHARED", "[LOG]" + msg);
        };
        return env;
    }

public:
    void HandleLuaOutput(const std::string& moduleName, const std::string& text)
    {
        //std::string s = FormatOutput(moduleName, text);
        std::string s = "[" + moduleName + "] " + text + "\n";
        QString message = QString::fromStdString(s);

        if (LuaPrint_Text)
        {
            QMetaObject::invokeMethod(LuaPrint_Text, [this, message]()
                                      {
                                          QString htmlEscaped = message.toHtmlEscaped();
                                          htmlEscaped.replace("\r\n", "<br>");
                                          htmlEscaped.replace('\n', "<br>");
                                          htmlEscaped.replace('\r', "<br>");
                                          QString htmlMessage = QString("<span style='color: black;'>%1</span>").arg(htmlEscaped);
                                          QTextCursor cursor = LuaPrint_Text->textCursor();
                                          cursor.movePosition(QTextCursor::End);
                                          LuaPrint_Text->setTextCursor(cursor);
                                          LuaPrint_Text->insertHtml(htmlMessage);
                                          LuaPrint_Text->ensureCursorVisible();
                                      }, Qt::QueuedConnection);
        }
    }

    void HandleLuaError(const std::string& moduleName, const std::string& error)
    {
        //std::string s = FormatOutput(moduleName, "[ERROR] " + error);
        std::string s = "[" + moduleName + "] " + error + "\n";
        QString message = QString::fromStdString(s);

        if (LuaPrint_Text)
        {
            QMetaObject::invokeMethod(LuaPrint_Text, [this, message]()
                                      {
                                          QString htmlEscaped = message.toHtmlEscaped();
                                          htmlEscaped.replace("\r\n", "<br>");
                                          htmlEscaped.replace('\n', "<br>");
                                          htmlEscaped.replace('\r', "<br>");
                                          QString htmlMessage = QString("<span style='color: red;'>%1</span>").arg(htmlEscaped);
                                          QTextCursor cursor = LuaPrint_Text->textCursor();
                                          cursor.movePosition(QTextCursor::End);
                                          LuaPrint_Text->setTextCursor(cursor);
                                          LuaPrint_Text->insertHtml(htmlMessage);
                                          LuaPrint_Text->ensureCursorVisible();
                                      }, Qt::QueuedConnection);
        }
    }

    std::string FormatOutput(const  std::string& prefix, const  std::string& content) const
    {
        QString timeString = QDateTime::currentDateTime().toString("hh:mm:ss");
        QString result = QString("[%1][%2] %3").arg(timeString, prefix, content);
        if (!result.endsWith('\n'))
        {
            result += '\n';
        }
        return result.toStdString();
    }

public:
    LuaEnvManager(QTextEdit* textEditWidget = nullptr) : LuaPrint_Text(textEditWidget)
    {
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::os, sol::lib::math, sol::lib::table, sol::lib::bit32, sol::lib::ffi, sol::lib::jit);
        shared_env = sol::environment(lua, sol::create, lua.globals());
        shared_env["_G"] = shared_env;

        shared_env["log"] = [this](const std::string& msg)
        {
            HandleLuaOutput("SHARED", "[LOG] " + msg);
        };

        shared_env["current_time"] = []() -> std::string
        {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        };
    }

    bool load_module(const std::string& module_name, const std::string& script_path)
    {
        if (modules.find(module_name) != modules.end())
        {
            return false;
        }

        try
        {
            sol::environment module_env(lua, sol::create, shared_env);
            module_env["_MODULE"] = module_name;

            module_env["print"] = [this, module_name](sol::variadic_args va, sol::this_state ts)
            {
                sol::state_view lua(ts);
                sol::function tostring = lua["tostring"];
                std::ostringstream oss;

                bool first = true;
                for (auto arg : va)
                {
                    if (!first) oss << "\t";
                    first = false;
                    sol::optional<std::string> str = tostring(arg);
                    oss << (str.value_or("nil"));
                }

                HandleLuaOutput(module_name, oss.str());
            };

            auto load_result = lua.script_file(script_path, module_env);
            if (!load_result.valid())
            {
                sol::error err = load_result;
                HandleLuaError(module_name, "脚本加载失败: " + std::string(err.what()));
                return false;
            }

            modules[module_name] = module_env;
            return true;
        }
        catch (const sol::error& e)
        {
            HandleLuaError(module_name, "模块加载异常: " + std::string(e.what()));
            return false;
        }
    }

    void unload_module(const std::string& module_name)
    {
        auto it = modules.find(module_name);
        if (it != modules.end())
        {
            modules.erase(it);
        }
    }

    void unload_all()
    {
        if (modules.size())
        {
            modules.clear();
            HandleLuaOutput("Lua", "已卸载所有Lua");
        }
        HandleLuaOutput("Lua", "Lua列表已刷新");
    }

    template <typename... Args>
    sol::protected_function_result call(const std::string& module_name, const std::string& func_name, Args&&... args)
    {
        auto it = modules.find(module_name);
        if (it != modules.end())
        {
            try
            {
                sol::protected_function func = it->second[func_name];
                if (func.valid())
                {
                    auto result = func(std::forward<Args>(args)...);
                    if (!result.valid())
                    {
                        sol::error err = result;
                        HandleLuaError(module_name, "函数调用错误: " + std::string(err.what()));
                    }
                    return result;
                }
                else
                {
                    HandleLuaError(module_name, "函数不存在: " + func_name);
                }
            }
            catch (const sol::error& e)
            {
                HandleLuaError(module_name, "函数调用异常: " + std::string(e.what()));
            }
        }
        else
        {
            HandleLuaError(module_name, "模块不存在");
        }
        return lua.safe_script("return nil");
    }

    void expose_to(const std::string& target_module, const std::string& source_module, const std::string& alias)
    {
        auto src_it = modules.find(source_module);
        auto tgt_it = modules.find(target_module);

        if (src_it != modules.end() && tgt_it != modules.end())
        {
            tgt_it->second[alias] = src_it->second;
            HandleLuaOutput(target_module, "已导入模块 " + source_module + " 作为 " + alias);
        }
        else
        {
            HandleLuaError(target_module, "源模块或目标模块不存在");
        }
    }

    std::vector<std::string> get_loaded_modules() const
    {
        std::vector<std::string> result;
        result.reserve(modules.size());

        for (const auto& pair : modules)
        {
            result.push_back(pair.first);
        }

        return result;
    }

    bool Execute_in_module(const std::string& module_name, const std::string& script)
    {
        auto it = modules.find(module_name);
        if (it != modules.end())
        {
            try
            {
                auto result = lua.script(script, it->second);
                if (!result.valid())
                {
                    sol::error err = result;
                    HandleLuaError(module_name, "脚本执行失败: " + std::string(err.what()));
                    return false;
                }
                return true;
            }
            catch (const sol::error& e)
            {
                HandleLuaError(module_name, "脚本执行异常: " + std::string(e.what()));
                return false;
            }
        }
        else
        {
            HandleLuaError(module_name, "模块不存在");
            return false;
        }
    }

    bool Execute_in_shared(const std::string& script)
    {
        try
        {
            auto result = lua.script(script, shared_env);
            if (!result.valid())
            {
                sol::error err = result;
                HandleLuaError("SHARED", "共享脚本执行失败: " + std::string(err.what()));
                return false;
            }
            return true;
        }
        catch (const sol::error& e)
        {
            HandleLuaError("SHARED", "共享脚本执行异常: " + std::string(e.what()));
            return false;
        }
    }
};

LuaEnvManager* LuaManager;

QVector<QString> GetLuaFiles(const QString& folder_path)
{
    QVector<QString> lua_files;
    QDir Dir(folder_path);
    if (!Dir.exists())
    {
        QMessageBox::critical(nullptr, "错误", "目录不存在或无法访问");
        return lua_files;
    }
    auto files = Dir.entryList(QDir::Files);
    for (const auto& file : files)
    {
        QFileInfo fileInfo(Dir, file);
        QString extension = fileInfo.suffix().toLower();
        if (extension == "lua" || extension == "ljbc")
        {
            lua_files.append(fileInfo.fileName());
        }
    }
    return lua_files;
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    QFont Font;
#ifdef Q_OS_MACOS
    Font.setFamily("PingFang SC");
#elif defined(Q_OS_WIN)
    Font.setFamily("Microsoft YaHei");
#else
    Font.setFamily("Noto Sans CJK SC");
#endif
    qApp->setFont(Font);
    ui->setupUi(this);
    LuaManager = new LuaEnvManager(ui->textEdit);

    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QString luaFolder = QDir::currentPath() + "/lua/";
    auto Refresh_Lua_List = [=]()
    {
        ui->treeWidget->clear();
        for (auto& fileName : GetLuaFiles(luaFolder))
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
            item->setText(0, fileName);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(0, Qt::Unchecked);
            item->setText(1, "❌ - 未加载");
        }
    };
    Refresh_Lua_List();

    connect(ui->treeWidget, &QTreeWidget::itemChanged, [=](QTreeWidgetItem* item, int column)
            {
                if (column == 0)
                {
                    Qt::CheckState state = item->checkState(0);
                    std::string name = item->text(0).toStdString();
                    std::string s = luaFolder.toStdString() + name;

                    if (state == Qt::Checked)
                    {
                        if (!QFile::exists(QString::fromStdString(s)))
                        {
                            item->setText(1, "❌ - 文件不存在");
                            ui->treeWidget->blockSignals(true);
                            item->setCheckState(0, Qt::Unchecked);
                            ui->treeWidget->blockSignals(false);
                            return;
                        }

                        auto load_result = LuaManager->load_module(name, s);
                        if (load_result)
                        {
                            item->setText(1, "✅ - 已加载");
                        }
                        else
                        {
                            item->setText(1, "❌ - 加载模块异常");
                            ui->treeWidget->blockSignals(true);
                            item->setCheckState(0, Qt::Unchecked);
                            ui->treeWidget->blockSignals(false);
                        }
                    }
                    else
                    {
                        if (item->text(1).contains("已加载"))
                        {
                            LuaManager->unload_module(name);
                        }
                        item->setText(1, "❌ - 未加载");
                    }
                }
            });

    connect(ui->pushButton, &QPushButton::clicked, [=]()
            {
                LuaManager->unload_all();
                Refresh_Lua_List();
            });

    connect(ui->pushButton_2, &QPushButton::clicked, [=]() { ui->textEdit->clear(); });
    connect(ui->pushButton_3, &QPushButton::clicked, [=]()
            {
                int itemCount = ui->treeWidget->topLevelItemCount();
                for (int i = 0; i < itemCount; ++i)
                {
                    QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
                    if (item->checkState(0) == Qt::Checked && item->text(1).contains("已加载"))
                    {
                        std::string name = item->text(0).toStdString();
                        std::string s = luaFolder.toStdString() + name;
                        LuaManager->unload_module(name);
                        if (!QFile::exists(QString::fromStdString(s)))
                        {
                            item->setText(1, "❌ - 文件不存在");
                            ui->treeWidget->blockSignals(true);
                            item->setCheckState(0, Qt::Unchecked);
                            ui->treeWidget->blockSignals(false);
                            continue;
                        }
                        auto load_result = LuaManager->load_module(name, s);
                        if (load_result)
                        {
                            item->setText(1, "✅ - 已加载");
                        }
                        else
                        {
                            item->setText(1, "❌ - 加载模块异常");
                            ui->treeWidget->blockSignals(true);
                            item->setCheckState(0, Qt::Unchecked);
                            ui->treeWidget->blockSignals(false);
                        }
                    }
                }
            });
}

Widget::~Widget()
{
    delete ui;
}
