#include "stdafx.h" 

#include "libs/ai/ai.h"

#include <iostream>
#include <string>
#include <sstream>

#include "LuaLinker.h"

static int luaAi_helloworld(lua_State *L)
{
    double d = lua_tonumber(L, 1);

    std::wostringstream strs;
    strs << d;

	MessageBox(NULL, (std::wstring(L"Hello World") + std::wstring(strs.str())).c_str(), L"", MB_OK);
	return 1;
}

static const luaL_reg ai_funcs[] = {
	{ "helloworld", luaAi_helloworld },
	{ NULL, NULL }
};

void open_ailib(lua_State *L)
{
    LuaLinker::Link();
	luaL_openlib(L, "ai", ai_funcs, 0);
}