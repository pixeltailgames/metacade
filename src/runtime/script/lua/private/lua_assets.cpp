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
lua_assets.cpp:
===============================================================================
*/

#include "lua_private.h"

CAssetRef* Arcade::toAssetRef(lua_State* L, int idx)
{
	CAssetRef* ref = (CAssetRef*) luaL_checkudata(L, idx, "__assetmeta");
	return ref;
}

IAsset* Arcade::toAsset(lua_State* L, int idx)
{
	CAssetRef* ref = toAssetRef(L, idx);
	if ( ref == nullptr ) return nullptr;

	IAsset* asset = ref->get();
	return asset;
}

void Arcade::pushAssetRef(lua_State* L, const CAssetRef& ref)
{
	CAssetRef* lref = (CAssetRef*) lua_newuserdata(L, sizeof(CAssetRef));
	memcpy(lref, &ref, sizeof(CAssetRef));

	luaL_getmetatable(L, "__assetmeta");
	lua_setmetatable(L, -2);
}

MODULE_FUNCTION_DEF(asset_gettype)
{
	CAssetRef* ref = toAssetRef(L, -1);
	if ( ref == nullptr ) return 0;

	lua_pushnumber(L, ref->getType());

	return 1;
}

MODULE_FUNCTION_DEF(asset_getname)
{
	IAsset* asset = toAsset(L, -1);
	if ( asset == nullptr ) return 0;

	lua_pushstring(L, *asset->getName());
	return 1;
}

static const luaL_Reg assetlib[] = {
	{"getType", asset_gettype},
	{"getName", asset_getname},
	{nullptr, nullptr}
};

void Arcade::openAssetModule(lua_State* L)
{
	luaL_newmetatable(L, "__assetmeta");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_rawset(L, -3);

	luaL_register(L, NULL, assetlib);
}
