
#ifndef _MLLUA_H
#define _MLLUA_H

#include <list>

extern "C"
{
#include "lualib/lua.h"
#include "lualib/lualib.h"
#include "lualib/lauxlib.h"
}

#include "containers/vector.h"
#include "math/vec3.h"

namespace scripting
{
	class lua
	{
	public:

		struct VariableName 
		{
			VariableName() { }
			VariableName(const char * i_Dummy) { }
		};

		enum eLuaLib
		{
			LL_BASE =		0x01,
			LL_TABLE =		0x02,
			LL_IO =			0x04,
			LL_STRING =		0x08,
			LL_MATH =		0x10,
			LL_DEBUG =		0x20,
		};

		enum eType
		{
			T_NONE = LUA_TNONE,
			T_NIL = LUA_TNIL,
			T_BOOL = LUA_TBOOLEAN,
			T_LIGHTUSERDATA = LUA_TLIGHTUSERDATA,
			T_NUMBER = LUA_TNUMBER,
			T_STRING = LUA_TSTRING,
			T_TABLE = LUA_TTABLE,
			T_FUNCTION = LUA_TFUNCTION,
			T_USERDATA = LUA_TUSERDATA,
			T_THREAD = LUA_TTHREAD,
		};

		static std::string TypeAsString(eType i_Type);

		class Variable;

		lua();
		lua(lua_State *i_LuaState);
		 ~lua();

		void InitState(enum eLuaLib i_OpenLib);
		void InitState(lua_State *i_LuaState);
		void RegisterFunctions();
		void TermState();
		void CheckStack();

		Variable GetGlobalTable();
		Variable GetParamTable();
		int GetTop();
		void DoBuffer(const char* i_Buffer, size_t i_BufferLength);
		void DoString(const std::string &i_String);
		void DoFile(const std::string &i_FileName);

		void AddStackRefBase(int i_StackRefBase) { m_StackRefBase += i_StackRefBase; }
		void RemStackRefBase(int i_StackRefBase) { m_StackRefBase -= i_StackRefBase; }

	private:
		lua_State *GetState() const { return m_LuaState; }
		void AddStackRef(Variable *i_Variable, int i_StackIndex);
		void RemoveStackRef(Variable *i_Variable, int i_StackIndex, bool i_RemoveStackValue);

	private:
		bool			m_OwnState;
		lua_State		*m_LuaState;
		int				m_InitStackTop;
		int				m_StackRefBase;

		typedef std::list<Variable *> VariablePtrList;
		typedef ctr::vector<VariablePtrList> VariablePtrListVector;

		VariablePtrListVector	m_StackRef;


	public:
		//--------------------------------------------------------------------------
		class Variable
		{

		friend class lua;

		private:
			enum eStackType
			{
				ST_INVALID,
				ST_GLOBAL,
				ST_NORMAL,
				ST_PARAM,
			};

		public:
			Variable();
			Variable(lua *i_Lua, eStackType i_StackType, int i_StackIndex, bool i_Created, const VariableName& i_Name = VariableName());
			Variable(const Variable &i_Variable);
			Variable &operator =(const Variable &i_Variable);
		private:
			void Init(lua *i_Lua, eStackType i_StackType, int i_StackIndex, bool i_Created, const VariableName& i_Name = VariableName());
		public:
			~Variable();

		private:
			lua_State *GetState() const { return m_Lua->GetState(); }
			void Validate();
			void Invalidate(bool i_RemoveStackValue = true);

		public:
			// Common
			bool Valid() const;
			eType GetType() const;
			 bool IsNil() const;
			 bool IsBool() const;
			 bool IsFloat() const;
			 bool IsInt() const;
			 bool IsString() const;
			 bool IsString(const char *i_Str) const;
			 bool IsPtr() const;
			 bool IsTable() const;
			 bool IsFunction() const;

			std::string GetTypeString() const;

			// Simple
			 bool GetBool() const;
			 float GetFloat() const;
			 int GetInt() const;
			 const char *GetString() const;
			 void *GetPtr() const;
			 const void *GetFunction() const;
			 math::vec3 GetVector3() const;
			 void GetIntArray(int* o_Array, int i_Num) const;
			 void GetFloatArray(float* o_Array, int i_Num) const;
//			 ColorFloat GetColRGBAf() const;
//			 ColorA8R8G8B8 GetColARGB() const;

			 bool GetBool(bool i_DefBool) const;
			 float GetFloat(float i_DefFloat) const;
			 int GetInt(int i_DefInt) const;
			 std::string GetString(const char *i_DefString) const;
			 math::vec3 GetVector3(const math::vec3 &i_DefVector3) const;

			void Push() const;

			// Table
			int Size() const;
			int OutSize() const;

			 Variable GetVariable(const char *i_KeyString) const;
			 Variable GetVariable(const std::string &i_KeyString) const;

			 Variable operator [](const char *i_KeyString) const;
			 Variable operator [](const std::string &i_KeyString) const;

			 Variable GetVariable(int i_KeyInt) const;
			 Variable operator [](int i_KeyInt) const;

			 void Begin(Variable &i_Key, Variable &i_Value);
			 bool End(Variable &i_Key);
			 void Next(Variable &i_Key, Variable &i_Value);

			void PushNil();
			void PushBool(bool i_Bool);
			void PushFloat(float i_Float);
			void PushInt(int i_Int);

			void PushString(const std::string &i_String);

			Variable PushTable();

			void PushNil(int i_KeyInt);
			void PushBool(int i_KeyInt, bool i_Bool);
			void PushFloat(int i_KeyInt, float i_Float);
			void PushInt(int i_KeyInt, int i_Int);
			void PushString(int i_KeyInt, const std::string &i_String);
			void PushPtr(int i_KeyInt, void *i_Ptr);
			void PushTable(int i_KeyInt);
			void PushVariable(int i_KeyInt, const Variable &i_Variable);

			void PushNil(const std::string &i_KeyString);
			void PushBool(const std::string &i_KeyString, bool i_Bool);
			void PushFloat(const std::string &i_KeyString, float i_Float);
			void PushInt(const std::string &i_KeyString, int i_Int);
			void PushString(const std::string &i_KeyString, const std::string &i_String);
			void PushPtr(const std::string &i_KeyString, void *i_Ptr);
			void PushTable(const std::string &i_KeyString);
			void PushVariable(const std::string &i_KeyString, const Variable &i_Variable);

		private:
			lua			*m_Lua;
			eStackType		m_StackType;
			int				m_StackIndex;
			int				m_Size;
			bool			m_Created;
			VariableName	m_Name;

		};
	};
} //namespace ML

#endif // _MLLUA_H

