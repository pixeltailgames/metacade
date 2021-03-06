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
lua_instance.cpp:
===============================================================================
*/

#include "lua_private.h"

static void copyTable(lua_State* L, int32 order = 0)
{
	lua_pushnil(L); //-1 nil, -2 src, -3 dst

	while (lua_next(L, -2))
	{
		//-1 v, -2 k, -3 src, -4 dst
		if ( lua_type(L, -1) != LUA_TTABLE )
		{
			lua_pushvalue(L, -2); //-1 ck, -2 v, -3 k, -4 src, -5 dst
			lua_pushvalue(L, -2); //-1 cv, -2 ck, -3 v, -4 k, -5 src, -6 dst

			//for ( int32 i=0; i<order; ++i ) std::cout << "\t";
			//std::cout << lua_tostring(L, -2) << std::endl;

			lua_rawset(L, -6); //-1 v, -2 k, -3 src, -4 dst
		}
		else if ( order < 4 && ( lua_type(L, -2) != LUA_TSTRING || strcmp(lua_tostring(L, -2), "_G") ) )
		{
			//for ( int32 i=0; i<order; ++i ) std::cout << "\t";
			//std::cout << "---" << lua_tostring(L, -2) << std::endl;

			lua_newtable(L); //-1 t, -2 v, -3 k, -4 src, -5 dst
			lua_pushvalue(L, -2); //-1 cv, -2 t, -3 v, -4 k, -5 src, -6 dst
			copyTable(L, order+1); //-1 t, -2 v, -3 k, -4 src, -5 dst

			lua_pushvalue(L, -3); //-1 ck, -2 t, -3 v, -4 k, -5 src, -6 dst
			lua_pushvalue(L, -2); //-1 t, -2 ck, -3 t, -4 v, -5 k, -6 src, -7 dst
			lua_rawset(L, -7); //-1 t, -2 v, -3 k, -4 src, -5 dst
			lua_pop(L, 1); //-1 v, -2 k, -3 src, -4 dst
		}
		lua_pop(L, 1); //-1 k, -2 src, -3 dst
	}

	lua_pop(L, 1); //-1 dst
}

static const char* G_blacklist[] =
{
	"setfenv",
	"getfenv",
	"dofile",
	"load",
	"loadstring",
	"loadfile",
	0,
};

void CLuaVMInstance::createAssetRefTable(EAssetType type, const CString& prefix)
{
	CPackage* pkg = getLuaClass()->getPackage();
	lua_State* L = getLuaHost()->_L;

	lua_newtable(L);

	for ( uint32 i=0; i<pkg->getNumAssets(); ++i )
	{
		CAssetRef ref = pkg->getAsset(i);
		if ( ref.getType() != type ) continue;

		IAsset* asset = ref.get();
		if ( asset == nullptr ) continue;

		pushAssetRef(L, ref);

		CString name = asset->getName();
		int32 dot = name.find(".");
		if ( dot != -1 )
		{
			name = name.sub(0, dot);
		}

		lua_setfield(L, -2, *name);
	}

	lua_setfield(L, -2, *prefix);
}

static int metaFunctionIndex(lua_State* L)
{
	lua_getfield(L, 1, "__vminstance");
	CLuaVMInstance* VM = (CLuaVMInstance*) lua_touserdata(L, -1);

	if ( VM == nullptr ) return 0;
	if ( VM->getLuaClass()->pushLuaFunction( lua_tostring(L, 2) ) )
	{
		lua_pushvalue(L, 1);
		lua_setfenv(L, -2);
		return 1;
	}

	/*VM->getLuaClass()->pushLocalTable();
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);
	lua_remove(L, -3);*/

	return 0;
}

static int hostFunction(lua_State* L)
{
	CLuaVMInstance* VM = (CLuaVMInstance*) lua_touserdata(L, lua_upvalueindex(1));
	const char* funcname = lua_tostring(L, lua_upvalueindex(2));

	CFunctionCall call(funcname);

	for ( int32 i=1; i<=lua_gettop(L); ++i )
	{
		switch ( lua_type(L, i) )
		{
			case LUA_TNIL: call.addArg( CVariant() ); break;
			case LUA_TBOOLEAN: call.addArg( CVariant( lua_toboolean(L, i) ) ); break;
			case LUA_TNUMBER: call.addArg( CVariant( lua_tonumber(L, i) ) ); break;
			case LUA_TSTRING: call.addArg( CVariant( lua_tostring(L, i) ) ); break;
		}
	}

	CVariant returnValue;
	bool success = VM->callHostFunction(call, returnValue);

	if ( !success )
	{
		lua_pushfstring(L, "Host does not define function '%s'", funcname);
		lua_error(L);
	}

	switch ( returnValue.type() )
	{
	case VT_NONE:
		lua_pushnil(L);
		return 1;
	break;
	case VT_BOOLEAN:
		{
			bool boolvalue;
			if ( !returnValue.get(boolvalue) ) return 0;
			lua_pushboolean(L, boolvalue);
			return 1;
		}
	break;
	case VT_UINT:
		{
			uint64 uintvalue;
			if ( !returnValue.get(uintvalue) ) return 0;
			lua_pushinteger(L, (lua_Integer) uintvalue);
			return 1;
		}
	break;
	case VT_INT:
		{
			int64 intvalue;
			if ( !returnValue.get(intvalue) ) return 0;
			lua_pushinteger(L, (lua_Integer) intvalue);
			return 1;
		}
	break;
	case VT_DOUBLE:
		{
			double doublevalue;
			if ( !returnValue.get(doublevalue) ) return 0;
			lua_pushnumber(L, (lua_Number) doublevalue);
			return 1;
		}
	break;
	case VT_STRING:
		{
			CString strvalue;
			if ( !returnValue.get(strvalue) ) return 0;
			lua_pushstring(L, *strvalue);
			return 1;
		}
	break;
	default:
	break;
	}

	return 0;
}

static int hostFunctionIndex(lua_State* L)
{
	CLuaVMInstance* VM = (CLuaVMInstance*) lua_touserdata(L, lua_upvalueindex(1));
	const char* funcname = luaL_checkstring(L, 2);

	lua_pushlightuserdata(L, VM);
	lua_pushstring(L, funcname);
	lua_pushcclosure(L, hostFunction, 2);

	return 1;
}

static int instanceRandomSeed(lua_State* L)
{
	CLuaVMInstance* VM = (CLuaVMInstance*) lua_touserdata(L, lua_upvalueindex(1));

	int32 seed = (int32) luaL_checkinteger(L, 1);
	VM->getRandom().randomSeed( seed );

	return 0;
}

static int instanceRandom(lua_State* L)
{
	CLuaVMInstance* VM = (CLuaVMInstance*) lua_touserdata(L, lua_upvalueindex(1));

	switch( lua_gettop(L) )
	{
		case 0: //no arguments [0.0, 1.0]
		lua_pushnumber(L, (lua_Number) VM->getRandom().randomFloat());
		break;

		case 1: //one argument [1, n]
		{
			int32 high = (int32) luaL_checkinteger(L, 1);
			luaL_argcheck(L, 1 <= high, 1, "interval is empty");
			lua_pushinteger(L, (lua_Integer) VM->getRandom().randomInt(1, high));
			break;
		}

		case 2: //two arguments [low, high]
		{
			int32 low = (int32) luaL_checkinteger(L, 1);
			int32 high = (int32) luaL_checkinteger(L, 2);
			luaL_argcheck(L, low <= high, 2, "interval is empty");
			lua_pushinteger(L, (lua_Integer) VM->getRandom().randomInt(low, high));
			break;
		}

		default:
		return luaL_error(L, "wrong number of arguments");
	}

	return 1;
}

//VM INSTANCE
Arcade::CLuaVMInstance::CLuaVMInstance(weak_ptr<CLuaVMClass> klass)
	: _klass(klass)
{
	if ( _klass.expired() ) return;

	CPackage* pkg = getLuaClass()->getPackage();
	lua_State* L = getLuaHost()->_L;

	if ( pkg == nullptr ) return;

	lua_newtable(L);
	lua_getglobal(L, "_G");
	copyTable(L); //copy everything from _G to the local table

	getLuaClass()->pushLocalTable();
	copyTable(L); //copy everything from the class locals into the local table

	lua_pushstring(L, "game");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "__vminstance");

	createAssetRefTable(ASSET_TEXTURE, "_t");
	createAssetRefTable(ASSET_SOUND, "_s");

	lua_getfield(L, -1, "math");
	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, instanceRandomSeed, 1);
	lua_setfield(L, -2, "randomseed");

	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, instanceRandom, 1);
	lua_setfield(L, -2, "random");
	lua_pop(L, 1);

//meta table for indexing functions
	lua_newtable(L);
	lua_pushcfunction(L, metaFunctionIndex);
	lua_setfield(L, -2, "__index");

	lua_setmetatable(L, -2);
//

//meta table for calling host functions
	lua_newtable(L);
	lua_newtable(L);
	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, hostFunctionIndex, 1);
	lua_setfield(L, -2, "__index");

	lua_setmetatable(L, -2);
	lua_setfield(L, -2, "host");
//

	const char** ptr = G_blacklist;
	while (*ptr)
	{
		lua_pushstring(L, *ptr);
		lua_pushnil(L);
		lua_settable(L, -3);
		++ptr;
	}

	_object = makeShared<LuaVMReference>(getLuaClass()->_host, -1);

	lua_pop(L, 1);
}

Arcade::CLuaVMInstance::~CLuaVMInstance()
{
	log(LOG_MESSAGE, "Destruct luaVMInstance");
}

class IVMClass* Arcade::CLuaVMInstance::getClass()
{
	if ( _klass.expired() ) return nullptr;

	return getLuaClass();
}

void Arcade::CLuaVMInstance::setGameInstance(IGameInstance* gameInstance)
{
	lua_State* L = getLuaHost()->getState();

	_gameInstance = gameInstance;
	_object->push();
	lua_pushlightuserdata(L, _gameInstance);
	lua_setfield(L, -2, "__gameinstance");
	lua_pop(L, 1);
}

void Arcade::CLuaVMInstance::postInputEvent(const class CInputEvent& input)
{
	bool success = false;
	float mouseX;
	float mouseY;

	CInputState previousState = _state;

	_state.applyEvent(input);
	_state.getMousePosition(mouseX, mouseY);

	switch ( input.getEventType() )
	{
	case INPUTEVENT_NONE:
	break;
	case INPUTEVENT_MOUSEPRESSED:
		success = !_state.getMouseIsFocused() || callFunction(CFunctionCall("onMousePressed"
			, mouseX
			, mouseY
			, (int32) input.getMouseButton()
			, _state.getMouseIsFocused()));
	break;
	case INPUTEVENT_MOUSERELEASED:
		success = !_state.getMouseIsFocused() || callFunction(CFunctionCall("onMouseReleased"
			, mouseX
			, mouseY
			, (int32) input.getMouseButton()
			, _state.getMouseIsFocused()));
	break;
	case INPUTEVENT_MOUSEMOVED:
		success = !_state.getMouseIsFocused() || callFunction(CFunctionCall("onMouseMoved"
			, input.getMouseX()
			, input.getMouseY()
			, input.getMouseDeltaX()
			, input.getMouseDeltaY()
			, _state.getMouseIsFocused()));
	break;
	case INPUTEVENT_KEYPRESSED:
		if ( previousState.getKeyIsDown( input.getKeycode() ))
		{
			success = callFunction(CFunctionCall("onKeyRepeat", (int32) input.getKeycode()));
		}
		else
		{
			success = callFunction(CFunctionCall("onKeyPressed", (int32) input.getKeycode()));
		}
	break;
	case INPUTEVENT_KEYRELEASED:
		success = callFunction(CFunctionCall("onKeyReleased", (int32) input.getKeycode()));
	break;
	case INPUTEVENT_FOCUSGAINED:
		success = callFunction(CFunctionCall("onFocusGained", (int32) input.getFocusElement()));
	break;
	case INPUTEVENT_FOCUSLOST:
		success = callFunction(CFunctionCall("onFocusLost", (int32) input.getFocusElement()));
	break;
	default:
	break;
	}
}

void Arcade::CLuaVMInstance::init()
{
	callFunction(CFunctionCall("init"));
}

void Arcade::CLuaVMInstance::think(float seconds, float deltaSeconds)
{
	if ( _klass.expired() ) return;

	callFunction(CFunctionCall("think", seconds, deltaSeconds));
}

void Arcade::CLuaVMInstance::render(shared_ptr<CElementRenderer> renderer)
{
	if ( _klass.expired() ) return;

	lua_State *L = getLuaHost()->_L;

	beginLuaDraw(L, renderer);

	callFunction(CFunctionCall("draw"));

	endLuaDraw(L, renderer);
}

void Arcade::CLuaVMInstance::reset()
{
	callFunction(CFunctionCall("reset"));
}

bool CLuaVMInstance::callFunction(const CFunctionCall& call)
{
	if ( _klass.expired() ) return false;

	lua_State *L = getLuaHost()->_L;

	if ( !getLuaClass()->pushLuaFunction(*call.getFunction()) ) 
	{
		return false;
	}

	_object->push();
	lua_getfield(L, -1, "__gameinstance");
	lua_setglobal(L, "__gameinstance");

	lua_setfenv(L, -2);

	int top = lua_gettop(L);

	for ( uint32 i=0; i<call.numArgs(); ++i )
	{
		if ( !getLuaHost()->pushVariant(call.getArg(i)) )
		{
			lua_settop(L, top);
			return false;
		}
	}

	return pcall(call.numArgs());
}

CLuaVM* Arcade::CLuaVMInstance::getLuaHost() const
{
	if ( _klass.expired() ) return nullptr;
	if ( getLuaClass()->getLuaHost().expired() ) return nullptr;

	return getLuaClass()->getLuaHost().lock().get();
}

CLuaVMClass* Arcade::CLuaVMInstance::getLuaClass() const
{
	auto klass = _klass.lock();
	return klass.get();
}

LuaVMReference* Arcade::CLuaVMInstance::getLuaObject() const
{
	return _object.get();
}

bool Arcade::CLuaVMInstance::pcall(int nargs)
{
	if ( _klass.expired() ) return false;

	return getLuaHost()->pcall(nargs);
}

bool CLuaVMInstance::callHostFunction(const CFunctionCall& call, CVariant& returnValue)
{
	CGameInstance* instance = (CGameInstance*)(_gameInstance);
	return instance->callHostFunction(call, returnValue);
}

CRandom& CLuaVMInstance::getRandom()
{
	return _random;
}