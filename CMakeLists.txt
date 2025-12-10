cmake_minimum_required(VERSION 3.19)
project(Lua-Manager-QT6 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Lua-Manager-QT6
    WIN32 MACOSX_BUNDLE
    main.cpp
    widget.cpp
    widget.h
    widget.ui
    LuaJIT-2.1/src/lauxlib.h
    LuaJIT-2.1/src/lua.h
    LuaJIT-2.1/src/lua.hpp
    LuaJIT-2.1/src/luaconf.h
    LuaJIT-2.1/src/luajit.h
    LuaJIT-2.1/src/lualib.h
    sol/config.hpp
    sol/forward.hpp
    sol/sol.hpp
)

target_include_directories(Lua-Manager-QT6
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/LuaJIT-2.1/src
        ${CMAKE_CURRENT_SOURCE_DIR}/sol
)

target_link_libraries(Lua-Manager-QT6
    PRIVATE
        Qt::Core
        Qt::Widgets
)

# Link Lua library based on platform
if(WIN32)
    # Windows: use lua51.lib
    target_link_libraries(Lua-Manager-QT6 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/LuaJIT-2.1/src/lua51.lib)
elseif(APPLE)
    # macOS: use libluajit.a
    target_link_libraries(Lua-Manager-QT6 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/LuaJIT-2.1/src/libluajit.a)
    # Link required system libraries
    target_link_libraries(Lua-Manager-QT6 PRIVATE m)
elseif(UNIX)
    # Linux: use libluajit.a
    target_link_libraries(Lua-Manager-QT6 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/LuaJIT-2.1/src/libluajit.a)
    # Link required system libraries
    target_link_libraries(Lua-Manager-QT6 PRIVATE m dl)
endif()

include(GNUInstallDirs)

install(TARGETS Lua-Manager-QT6
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Lua-Manager-QT6
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
