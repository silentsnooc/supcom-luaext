#pragma once


#include "lauxlib.h"


#ifdef LUA_EXTENSION_EXPORTS
#define LUA_EXTENSION_API __declspec(dllexport) 
#else  
#define LUA_EXTENSION_API __declspec(dllimport)   
#endif  

#define EXTERN_DLL_EXPORT extern "C" LUA_EXTENSION_API

/*
Main entry point of the Supreme Commander Lua Extension Library.
*/
EXTERN_DLL_EXPORT void initialize(lua_State *L);


