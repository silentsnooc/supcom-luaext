// lua-extension.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h" 

#include "lua-extension.h"

#include "libs/ai/ai.h"


EXTERN_DLL_EXPORT void initialize(lua_State *L)
{
	open_ailib(L); // Open AI library
}


