#include "lua.h"
#include "file/filesystem.h"

namespace scripting
{

	//--------------------------------------------------------------------------
	lua::lua() :
		m_OwnState(false),
		m_LuaState(NULL),
		m_StackRefBase(0)
	{
	}

	//--------------------------------------------------------------------------
	lua::lua(lua_State *i_LuaState) :
		m_OwnState(false),
		m_LuaState(NULL),
		m_StackRefBase(0)
	{
		InitState(i_LuaState);
	}

	//--------------------------------------------------------------------------
	lua::~lua()
	{
		TermState();
	}

	//--------------------------------------------------------------------------
	ctr::string lua::TypeAsString(eType i_Type)
	{
		switch (i_Type)
		{
			case T_NONE: return "none";
			case T_NIL: return "nil";
			case T_BOOL: return "bool";
			case T_LIGHTUSERDATA: return "lightuserdata";
			case T_NUMBER: return "number";
			case T_STRING: return "string";
			case T_TABLE: return "table";
			case T_FUNCTION: return "function";
			case T_USERDATA: return "userdata";
			case T_THREAD: return "thread";
		}
		return "unknown";
	}

	//--------------------------------------------------------------------------
	static int lua_panic(lua_State *L)
	{
		*((char *)0) = 0;
		return 0;
	}

	//--------------------------------------------------------------------------
	static int lua_dofile(lua_State *L)
	{
		lua lua(L);
		lua::Variable Params = lua.GetParamTable();
		lua.DoFile(Params[0].GetString());
		return 0;
	}

	//--------------------------------------------------------------------------
	void lua::InitState(enum eLuaLib i_OpenLib)
	{
		assert(m_LuaState == NULL);
		m_OwnState = true;
		m_LuaState = lua_open();
		lua_atpanic(m_LuaState, lua_panic);

		if (i_OpenLib & LL_BASE)
			luaopen_base(m_LuaState);
		if (i_OpenLib & LL_TABLE)
			luaopen_table(m_LuaState);
		if (i_OpenLib & LL_IO)
			luaopen_io(m_LuaState);
		if (i_OpenLib & LL_STRING)
			luaopen_string(m_LuaState);
		if (i_OpenLib & LL_MATH)
			luaopen_math(m_LuaState);
		if (i_OpenLib & LL_DEBUG)
			luaopen_debug(m_LuaState);

		m_InitStackTop = lua_gettop(m_LuaState);
		
		RegisterFunctions();
	}

	//--------------------------------------------------------------------------
	void lua::InitState(lua_State *i_LuaState)
	{
		assert(m_LuaState == NULL);
		m_OwnState = false;
		m_LuaState = i_LuaState;
		RegisterFunctions();
	}

	//--------------------------------------------------------------------------
	void lua::RegisterFunctions()
	{
		lua_pushcclosure(m_LuaState, lua_dofile, 0);
		lua_setglobal(m_LuaState, "DoFile");
	}

	//--------------------------------------------------------------------------
	void lua::TermState()
	{
		if (m_LuaState && m_OwnState)
		{
			lua_close(m_LuaState);
		}
		m_LuaState = NULL;
	}

	//--------------------------------------------------------------------------
	void lua::CheckStack()
	{
		assert(m_OwnState == false || lua_gettop(m_LuaState) == m_InitStackTop);
	}

	//--------------------------------------------------------------------------
	lua::Variable lua::GetGlobalTable()
	{
		return Variable(this, Variable::ST_GLOBAL, LUA_GLOBALSINDEX, false);
	}

	//--------------------------------------------------------------------------
	lua::Variable lua::GetParamTable()
	{
		return Variable(this, Variable::ST_PARAM, 1, false);
	}

	//--------------------------------------------------------------------------
	int lua::GetTop()
	{
		return lua_gettop(m_LuaState);
	}

	//--------------------------------------------------------------------------
	void lua::DoBuffer(const char* i_Buffer, size_t i_BufferLength)
	{
		int Ret = lua_dobuffer(m_LuaState, i_Buffer, i_BufferLength, "DoBuffer");
		assert(Ret == 0);//(Ret).msg("Error: lua::DoBuffer()");
	}

	//--------------------------------------------------------------------------
	void lua::DoString(const ctr::string &i_String)
	{
		int Ret = lua_dostring(m_LuaState, i_String.c_str());
		assert(Ret == 0);//(Ret)(i_String).msg("Error: lua::DoString()");
	}

	//----------------------------------------------------------------------------
	void lua::DoFile(const ctr::string &i_FileName)
	{
		file::file File;
		
		File.open(i_FileName.c_str(), "rb");

		if (!File.opened())
		{
			assert(0);
			return;
		}

		unsigned filesize=File.size();

		char *Buf = new char[filesize];
		File.read_bytes(Buf, filesize);
		File.close();

		int Ret = lua_dobuffer(m_LuaState, (char*)Buf, size_t(filesize), i_FileName.c_str());

		assert(Ret == 0);
	}

	//--------------------------------------------------------------------------
	void lua::AddStackRef(Variable *i_Variable, int i_StackIndex)
	{
		if (!i_Variable->m_Created)
			return;
		int StackRefIndex = m_StackRefBase + i_StackIndex;
		assert(i_StackIndex > 0 && i_StackIndex <= lua_gettop(m_LuaState));

		if (StackRefIndex >= int(m_StackRef.size()))
		{
			m_StackRef.resize(StackRefIndex+1);
		}

		assert(unsigned int(StackRefIndex) < m_StackRef.size());

		m_StackRef[StackRefIndex].push_back(i_Variable);
	}

	//--------------------------------------------------------------------------
	void lua::RemoveStackRef(Variable *i_Variable, int i_StackIndex, bool i_RemoveStackValue)
	{
		if (!i_Variable->m_Created)
			return;
		int StackRefIndex = m_StackRefBase + i_StackIndex;
		assert(unsigned int(StackRefIndex) > 0 && unsigned int(StackRefIndex) < m_StackRef.size());
		VariablePtrList::iterator place;
		for(place = m_StackRef[StackRefIndex].begin();place != m_StackRef[StackRefIndex].end();place++)
			if(*place==i_Variable) 
				break;
		assert(place!=m_StackRef[StackRefIndex].end());
		m_StackRef[StackRefIndex].remove(i_Variable);
		if (m_StackRef[StackRefIndex].empty())
		{
			if (i_RemoveStackValue)
			{
				assert(i_StackIndex > 0 && i_StackIndex <= lua_gettop(m_LuaState));
				if (i_StackIndex == lua_gettop(m_LuaState))
					lua_pop(m_LuaState, 1);
				else
					lua_remove(m_LuaState, i_StackIndex);
			}
			if (StackRefIndex == m_StackRef.size()-1)
			{
				m_StackRef.pop_back();
			}
			else
			{
				VariablePtrListVector::iterator VPtrListIt = m_StackRef.begin()+StackRefIndex;
//				VariablePtrListVector::iterator VPtrListIt = (VariablePtrListVector::iterator)&m_StackRef[StackRefIndex];
				VariablePtrListVector::iterator next=VPtrListIt; ++next;
//				VPtrListIt = m_StackRef.erase(VPtrListIt);
				m_StackRef.erase(VPtrListIt);

				for (VPtrListIt=next; VPtrListIt != m_StackRef.end(); ++VPtrListIt)
				{
					for (VariablePtrList::iterator VPtrIt = VPtrListIt->begin(); VPtrIt != VPtrListIt->end(); ++ VPtrIt)
						--((*VPtrIt)->m_StackIndex);
				}
			}
		}
	}

	//--------------------------------------------------------------------------
	lua::Variable::Variable() :
		m_Lua(NULL),
		m_StackType(ST_INVALID),
		m_StackIndex(-1),
		m_Created(false)
#ifdef MITLUA_DEBUG
		, m_Name("<undef>")
#endif
	{
	}

	//--------------------------------------------------------------------------
	lua::Variable::Variable(lua *i_Lua, eStackType i_StackType, int i_StackIndex, bool i_Created, const VariableName& i_Name) :
		m_Lua(i_Lua),
		m_StackType(i_StackType),
		m_StackIndex(i_StackIndex),
		m_Size(0),
		m_Created(i_Created),
		m_Name(i_Name)
	{
		Validate();
		if (i_StackType == ST_PARAM)
			m_Size = lua_gettop(GetState());
	}

	//--------------------------------------------------------------------------
	lua::Variable::Variable(const Variable &i_Variable) :
		m_Lua(i_Variable.m_Lua),
		m_StackType(i_Variable.m_StackType),
		m_StackIndex(i_Variable.m_StackIndex),
		m_Size(i_Variable.m_Size),
		m_Created(i_Variable.m_Created),
		m_Name(i_Variable.m_Name)
	{
		Validate();
	}

	//--------------------------------------------------------------------------
	lua::Variable &lua::Variable::operator =(const Variable &i_Variable)
	{
		Invalidate();
		Init(i_Variable.m_Lua, i_Variable.m_StackType, i_Variable.m_StackIndex, i_Variable.m_Created, i_Variable.m_Name);
		return *this;
	}

	//--------------------------------------------------------------------------
	void lua::Variable::Init(lua *i_Lua, eStackType i_StackType, int i_StackIndex, bool i_Created, const VariableName& i_Name)
	{
		assert(!Valid());
		m_Lua = i_Lua;
		m_StackType = i_StackType;
		m_StackIndex = i_StackIndex;
		m_Created = i_Created;
		m_Name = i_Name;
		Validate();
	}

	//--------------------------------------------------------------------------
	lua::Variable::~Variable()
	{
		Invalidate();
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::Valid() const
	{
		return m_StackType != ST_INVALID;
	}

	//--------------------------------------------------------------------------
	void lua::Variable::Validate()
	{
		m_Lua->AddStackRef(this, m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::Invalidate(bool i_RemoveStackValue)
	{
		if (Valid())
		{
			m_Lua->RemoveStackRef(this, m_StackIndex, i_RemoveStackValue);
			m_StackType = ST_INVALID;
		}
	}

	//--------------------------------------------------------------------------
	lua::eType lua::Variable::GetType() const
	{
		if (!Valid())
			return T_NONE;
		if (m_StackType == ST_NORMAL)
			return (eType)lua_type(GetState(), m_StackIndex);
		return T_TABLE;
	}

	//--------------------------------------------------------------------------
	ctr::string lua::Variable::GetTypeString() const
	{
		return lua::TypeAsString(GetType());
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::IsNil() const
	{
		return GetType() == T_NIL;
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::IsBool() const
	{
		return GetType() == T_BOOL;
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::IsFloat() const
	{
		return GetType() == T_NUMBER;
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::IsInt() const
	{
		if (!IsFloat())
			return false;
		float F = GetFloat();
		return (F == (int)F);
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::IsString() const
	{
		return GetType() == T_STRING;
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::IsString(const char *i_Str) const
	{
		return GetType() == T_STRING && strcmp(lua_tostring(GetState(), m_StackIndex),i_Str)==0;
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::IsPtr() const
	{
		return GetType() == T_LIGHTUSERDATA;
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::IsTable() const
	{
		return GetType() == T_TABLE;
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::IsFunction() const
	{
		return GetType() == T_FUNCTION;
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::GetBool() const
	{
		return !!lua_toboolean(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	float lua::Variable::GetFloat() const
	{
		return lua_tonumber(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	int lua::Variable::GetInt() const
	{
		return (int)GetFloat();
	}

	//--------------------------------------------------------------------------
	const char * lua::Variable::GetString() const
	{
		return lua_tostring(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void *lua::Variable::GetPtr() const
	{
		return lua_touserdata(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	const void *lua::Variable::GetFunction() const
	{
		return lua_topointer(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	math::vec3 lua::Variable::GetVector3() const
	{
		math::vec3 V;
		V.x = GetVariable(1).GetFloat();
		V.y = GetVariable(2).GetFloat();
		V.z = GetVariable(3).GetFloat();
		return V;
	}

	//--------------------------------------------------------------------------
	void lua::Variable::GetIntArray(int* o_Array, int i_Num) const
	{
		for (int n=1; n<=i_Num; ++n)
			o_Array[n-1]= GetVariable(n).GetInt();
	}

	//--------------------------------------------------------------------------
	void lua::Variable::GetFloatArray(float* o_Array, int i_Num) const
	{
		for (int n=1; n<=i_Num; ++n)
			o_Array[n-1]= GetVariable(n).GetFloat();
	}

	//----------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	bool lua::Variable::GetBool(bool i_DefBool) const
	{
		if (IsBool())
			return GetBool();
		else
			return i_DefBool;
	}

	//--------------------------------------------------------------------------
	float lua::Variable::GetFloat(float i_DefFloat) const
	{
		if (IsFloat())
			return GetFloat();
		else
			return i_DefFloat;
	}

	//--------------------------------------------------------------------------
	int lua::Variable::GetInt(int i_DefInt) const
	{
		if (IsFloat())
			return GetInt();
		else
			return i_DefInt;
	}

	//--------------------------------------------------------------------------
	ctr::string lua::Variable::GetString(const char *i_DefString) const
	{
		if (IsString())
			return GetString();
		else
			return i_DefString;
	}

	//--------------------------------------------------------------------------
	math::vec3 lua::Variable::GetVector3(const math::vec3 &i_DefVector3) const
	{
		if (IsTable())
			return GetVector3();
		return i_DefVector3;
	}

	//--------------------------------------------------------------------------
	void lua::Variable::Push() const
	{
		lua_pushvalue(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	int lua::Variable::Size() const
	{
		return m_Size;
	}

	//--------------------------------------------------------------------------
	int lua::Variable::OutSize() const
	{
		return lua_gettop(GetState()) - m_Size;
	}

	//--------------------------------------------------------------------------
	lua::Variable lua::Variable::GetVariable(int i_KeyInt) const
	{
		if (m_StackType == ST_NORMAL)
		{
			lua_pushnumber(GetState(), lua_Number(i_KeyInt));
			lua_gettable(GetState(), m_StackIndex);
			return Variable(m_Lua, ST_NORMAL, lua_gettop(GetState()), true, m_Name);
		}
		else
		{
			return Variable(m_Lua, ST_NORMAL, m_StackIndex+i_KeyInt, false, m_Name);
		}
	}

	//--------------------------------------------------------------------------
	lua::Variable lua::Variable::operator [](int i_KeyInt) const
	{
		return GetVariable(i_KeyInt);
	}

	//--------------------------------------------------------------------------
	lua::Variable lua::Variable::GetVariable(const ctr::string &i_KeyString) const
	{
		return GetVariable(i_KeyString.c_str());
	}

	//--------------------------------------------------------------------------
	lua::Variable lua::Variable::GetVariable(const char *i_KeyString) const
	{
		if (m_StackType != ST_PARAM)
		{
			lua_pushlstring(GetState(), i_KeyString, strlen(i_KeyString));
			lua_gettable(GetState(), m_StackIndex);
			return Variable(m_Lua, ST_NORMAL, lua_gettop(GetState()), true, i_KeyString);
		}
		else
		{
			lua_pushlstring(GetState(), i_KeyString, strlen(i_KeyString));
			lua_gettable(GetState(), LUA_GLOBALSINDEX);
			return Variable(m_Lua, ST_NORMAL, lua_gettop(GetState()), true, i_KeyString);
		}
	}

	//--------------------------------------------------------------------------
	lua::Variable lua::Variable::operator [](const ctr::string &i_KeyString) const
	{
		return GetVariable(i_KeyString.c_str());
	}

	//--------------------------------------------------------------------------
	lua::Variable lua::Variable::operator [](const char *i_KeyString) const
	{
		return GetVariable(i_KeyString);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::Begin(Variable &i_Key, Variable &i_Value) 
	{
		i_Value.Invalidate();
		i_Key.Invalidate();
		lua_pushnil(GetState());
		if (lua_next(GetState(), m_StackIndex))
		{
			i_Key.Init(m_Lua, ST_NORMAL, lua_gettop(GetState())-1, true, m_Name);
			i_Value.Init(m_Lua, ST_NORMAL, lua_gettop(GetState()), true, m_Name);
		}
	}

	//--------------------------------------------------------------------------
	bool lua::Variable::End(Variable &i_Key)
	{
		return !i_Key.Valid();
	}

	//--------------------------------------------------------------------------
	void lua::Variable::Next(Variable &i_Key,  Variable &i_Value) 
	{
		i_Value.Invalidate();
		if (i_Key.m_StackIndex == lua_gettop(GetState()))
		{
			i_Key.Invalidate(false);
		}
		else
		{
			lua_pushvalue(GetState(), i_Key.m_StackIndex);
			i_Key.Invalidate();
		}
		if (lua_next(GetState(), m_StackIndex))
		{
			i_Key.Init(m_Lua, ST_NORMAL, lua_gettop(GetState())-1, true, m_Name);
			i_Value.Init(m_Lua, ST_NORMAL, lua_gettop(GetState()), true, m_Name);
		}
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushNil()
	{
		lua_pushnil(GetState());
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushBool(bool i_Bool)
	{
		lua_pushboolean(GetState(),i_Bool?1:0);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushFloat(float i_Float)
	{
		lua_pushnumber(GetState(),i_Float);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushInt(int i_Int)
	{
		lua_pushnumber(GetState(), lua_Number(i_Int));
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushString(const ctr::string &i_String)
	{
		lua_pushlstring(GetState(),i_String.c_str(),i_String.size());
	}

	//--------------------------------------------------------------------------
	lua::Variable lua::Variable::PushTable()
	{
		lua_newtable(GetState());
		return Variable(m_Lua, ST_NORMAL, lua_gettop(GetState()), false, m_Name);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushNil(int i_KeyInt)
	{
		lua_pushnumber(GetState(), lua_Number(i_KeyInt));
		lua_pushnil(GetState());
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushBool(int i_KeyInt, bool i_Bool)
	{
		lua_pushnumber(GetState(), lua_Number(i_KeyInt));
		lua_pushboolean(GetState(), i_Bool);
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushFloat(int i_KeyInt, float i_Float)
	{
		lua_pushnumber(GetState(), lua_Number(i_KeyInt));
		lua_pushnumber(GetState(), i_Float);
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushInt(int i_KeyInt, int i_Int)
	{
		lua_pushnumber(GetState(), lua_Number(i_KeyInt));
		lua_pushnumber(GetState(), lua_Number(i_Int));
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushString(int i_KeyInt, const ctr::string &i_String)
	{
		lua_pushnumber(GetState(), lua_Number(i_KeyInt));
		lua_pushlstring(GetState(), i_String.c_str(), i_String.size());
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushPtr(int i_KeyInt, void *i_Ptr)
	{
		lua_pushnumber(GetState(), lua_Number(i_KeyInt));
		lua_pushlightuserdata(GetState(), i_Ptr);
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushTable(int i_KeyInt)
	{
		lua_pushnumber(GetState(), lua_Number(i_KeyInt));
		lua_newtable(GetState());
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushVariable(int i_KeyInt, const Variable &i_Variable)
	{
		lua_pushnumber(GetState(), lua_Number(i_KeyInt));
		lua_pushvalue(GetState(), i_Variable.m_StackIndex);
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushNil(const ctr::string &i_KeyString)
	{
		lua_pushlstring(GetState(), i_KeyString.c_str(), i_KeyString.size());
		lua_pushnil(GetState());
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushBool(const ctr::string &i_KeyString, bool i_Bool)
	{
		lua_pushlstring(GetState(), i_KeyString.c_str(), i_KeyString.size());
		lua_pushboolean(GetState(), i_Bool);
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushFloat(const ctr::string &i_KeyString, float i_Float)
	{
		lua_pushlstring(GetState(), i_KeyString.c_str(), i_KeyString.size());
		lua_pushnumber(GetState(), i_Float);
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushInt(const ctr::string &i_KeyString, int i_Int)
	{
		lua_pushlstring(GetState(), i_KeyString.c_str(), i_KeyString.size());
		lua_pushnumber(GetState(), lua_Number(i_Int));
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushString(const ctr::string &i_KeyString, const ctr::string &i_String)
	{
		lua_pushlstring(GetState(), i_KeyString.c_str(), i_KeyString.size());
		lua_pushlstring(GetState(), i_String.c_str(), i_String.size());
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushPtr(const ctr::string &i_KeyString, void *i_Ptr)
	{
		lua_pushlstring(GetState(), i_KeyString.c_str(), i_KeyString.size());
		lua_pushlightuserdata(GetState(), i_Ptr);
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushTable(const ctr::string &i_KeyString)
	{
		lua_pushlstring(GetState(), i_KeyString.c_str(), i_KeyString.size());
		lua_newtable(GetState());
		lua_settable(GetState(), m_StackIndex);
	}

	//--------------------------------------------------------------------------
	void lua::Variable::PushVariable(const ctr::string &i_KeyString, const Variable &i_Variable)
	{
		lua_pushlstring(GetState(), i_KeyString.c_str(), i_KeyString.size());
		lua_pushvalue(GetState(), i_Variable.m_StackIndex);
		lua_settable(GetState(), m_StackIndex);
	}

}
