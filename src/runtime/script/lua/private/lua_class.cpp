/*
Copyright 2017 Zachary Blystone <zakblystone@gmail.com>

This file is part of Metacade.

Metacade is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Metacade is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Metacade.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
===============================================================================
lua_class.cpp:
===============================================================================
*/

#include "lua_private.h"

#include <fstream>

Arcade::CLuaVMClass::CLuaVMClass(shared_ptr<CLuaVM> host)
	: _host(host)
	, _metaData(make_shared<CMetaData>())
{

}

Arcade::CLuaVMClass::~CLuaVMClass()
{

}

bool Arcade::CLuaVMClass::reload()
{
	if ( _lastLoadFile != "" )
	{
		return loadFromFile(_lastLoadFile);
	}
	return false;
}

static bool parseArgs(const CString& str, CMetaData& meta)
{
	const char* raw = *str;

	uint32 mark = 0;
	vector<CString> args;

	for ( uint32 i=0; i<=str.length(); ++i )
	{
		if ( raw[i] == ' ' || i == str.length() )
		{
			args.push_back(str.sub(mark, i-mark));
			mark = i+1;
		}
	}

	if ( args.size() == 0 ) return true;

	meta.setKeyValuePair("file", args[0]);
	for ( uint32 i=1; i<args.size(); ++i )
	{
		meta.setKeyValuePair(args[i], "1");
	}

	return true;
}

void CLuaVMClass::buildAssets(CPackageBuilder* builder)
{
	for ( auto texLoadPair : _textureLoadArgs )
	{
		CString assetName = (texLoadPair).first;
		CString args = (texLoadPair).second;

		CImageAsset* image = builder->addNamedAsset<CImageAsset>(assetName + ".tex");
		CMetaData meta;

		if ( !parseArgs(args, meta) )
		{
			std::cout << "Parse Fail" << std::endl;
			continue;
		}

		IAssetCompiler* compiler = builder->getAssetCompiler();
		if ( compiler != nullptr )
		{
			compiler->compile(image, &meta);
		}
	}
}

shared_ptr<CMetaData> Arcade::CLuaVMClass::getMetaData()
{
	return _metaData;
}

class IVMHost* Arcade::CLuaVMClass::getHost()
{
	return _host.get();
}

int CLuaVMClass::metaFunctionCreate(lua_State *L)
{
	lua_getfield(L, 1, "__klass");

	CLuaVMClass *klass = (CLuaVMClass *) lua_touserdata(L, -1);
	const char *key = lua_tostring(L, 2);

	if ( klass != nullptr )
	{
		int type = lua_type(L, 3);

		if ( type == LUA_TFUNCTION )
		{
			lua_getglobal(L, "_G");
			lua_setfenv(L, 3);

			auto entry = make_pair(std::string(key), make_shared<LuaVMReference>(klass->_host, 3));
			klass->_functions.insert(entry);
		}
		else
		{
			luaL_error(L, "functions only");
		}
	}

	return 0;
}

int CLuaVMClass::metaTextureCreate(lua_State *L)
{
	lua_getfield(L, 1, "__klass");

	CLuaVMClass* klass = (CLuaVMClass*) lua_touserdata(L, -1);
	const char* key = lua_tostring(L, 2);

	if ( klass != nullptr )
	{
		int type = lua_type(L, 3);

		if ( type == LUA_TSTRING )
		{
			const char *value = lua_tostring(L, 3);

			lua_getglobal(L, "_G");
			lua_setfenv(L, 3);

			std::cout << "TEXTURE: " << key << " = " << value << std::endl;

			auto entry = make_pair(CString(key), CString(value));
			klass->_textureLoadArgs.insert(entry);
		}
		else
		{
			luaL_error(L, "strings only");
		}
	}
	else
	{
		std::cout << "NO KLASS" << std::endl;
	}

	return 0;
}

shared_ptr<IVMInstance> Arcade::CLuaVMClass::createVMInstance()
{
	return shared_ptr<CLuaVMInstance>( new CLuaVMInstance(shared_from_this()) );
}

bool Arcade::CLuaVMClass::pushLuaFunction(string functionName) const
{
	auto found = _functions.find(functionName);
	if ( found != _functions.end() )
	{
		(*found).second->push();
		return true;
	}
	return false;
}

bool Arcade::CLuaVMClass::loadFromAsset(CCodeAsset* asset)
{
	lua_State *L = _host->getState();
	_functions.clear();

	if (luaL_loadbuffer(L, asset->getCodeBuffer(), asset->getCodeLength(), "main"))
	{
		std::cout << "Lua: main: " << lua_tostring(L, -1);
		lua_pop(L, 1);
		return false;	
	}

	lua_newtable(L);

	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "__klass");

	lua_newtable(L);
	lua_newtable(L);
	lua_setfield(L, -2, "sounds");

		
	//TEXTURE TABLE
	lua_newtable(L);

	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "__klass");

		//TEXTURE METATABLE
		lua_newtable(L);
		lua_pushcclosure(L, CLuaVMClass::metaTextureCreate, 0);
		lua_setfield(L, -2, "__newindex");
		//DONE TEXTURE METATABLE

	lua_setmetatable(L, -2);
	lua_setfield(L, -2, "textures");
	//DONE TEXTURE TABLE

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	lua_pushcclosure(L, CLuaVMClass::metaFunctionCreate, 0);
	lua_setfield(L, -2, "__newindex");

	lua_setmetatable(L, -2);
	lua_setfenv(L, -2);

	if (lua_pcall(L, 0, 0, 0)) {
		std::cout << "Lua: main: " << lua_tostring(L, -1);
		lua_pop(L, 1);
		return false;
	}

	return true;
}

bool Arcade::CLuaVMClass::loadFromFile(string filename)
{
	_lastLoadFile = filename;
	_functions.clear();

	std::fstream input(filename, std::ios::binary | std::ios::in | std::ios::ate);

	lua_State *L = _host->getState();

	if ( input.is_open() )
	{
		uint32 size = (uint32) input.tellg();
		input.seekg(0);

		if ( size == 0 ) return false;
		
		char *buffer = new char[size];
		input.read(buffer, size);

		if (luaL_loadbuffer(L, buffer, size, "main"))
		{
			std::cout << "Lua: main: " << lua_tostring(L, -1);
			lua_pop(L, 1);
			return false;	
		}

		lua_newtable(L);

		lua_pushlightuserdata(L, this);
		lua_setfield(L, -2, "__klass");

		lua_newtable(L);
		lua_newtable(L);
		lua_setfield(L, -2, "sounds");

		
		//TEXTURE TABLE
		lua_newtable(L);

		lua_pushlightuserdata(L, this);
		lua_setfield(L, -2, "__klass");

			//TEXTURE METATABLE
			lua_newtable(L);
			lua_pushcclosure(L, CLuaVMClass::metaTextureCreate, 0);
			lua_setfield(L, -2, "__newindex");
			//DONE TEXTURE METATABLE

		lua_setmetatable(L, -2);
		lua_setfield(L, -2, "textures");
		//DONE TEXTURE TABLE

		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");

		lua_pushcclosure(L, CLuaVMClass::metaFunctionCreate, 0);
		lua_setfield(L, -2, "__newindex");

		lua_setmetatable(L, -2);
		lua_setfenv(L, -2);

		if (lua_pcall(L, 0, 0, 0)) {
			std::cout << "Lua: main: " << lua_tostring(L, -1);
			lua_pop(L, 1);
			return false;
		}

		return true;
	}

	return false;
}