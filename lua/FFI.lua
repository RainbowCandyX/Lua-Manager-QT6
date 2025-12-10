local ffi = require("ffi")
ffi.cdef([[
	int GetSystemMetrics(int nIndex);
	int MessageBoxA(void* hWnd, const char* lpText, const char* lpCaption, unsigned int uType);
]])

local user32 = ffi.load("user32")
local w = user32.GetSystemMetrics(0)
local h = user32.GetSystemMetrics(1)
local s = string.format("%d*%d", w, h)
print(s)
user32.MessageBoxA(nil, "LuaJIT FFI!", "LuaJIT", 0)