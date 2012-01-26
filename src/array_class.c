

#define LUNAR_PRIVATE_API
#include "lunar.h"
#include "lauxlib.h"


#define LUA_CHUNK_TO_MEMBER(luastate, string, member) {	\
    luaL_dostring(L, string);				\
    lua_getglobal(L, "TempFunc");			\
    lua_setfield(L, -2, (#member));			\
    lua_pushnil(L);					\
    lua_setglobal(L, "TempFunc");			\
  }							\
    

static int luaC_array_dtype(lua_State *L);
static int luaC_array_shape(lua_State *L);
static int luaC_array_astable(lua_State *L);


static const char IndexIterator1[] = " \
function TempFunc(A)				\
   local N  =  A:shape()			\
   local n0 = { }				\
				                \
   for i=1,#N-1 do n0[i] = 0 end		\
   n0[#N] = -1					\
						\
   local function f(_,I)			\
      I[#N] = I[#N] + 1				\
      for d=#N,2,-1 do				\
         if I[d] == N[d] then			\
            I[d] = 0				\
            I[d-1] = I[d-1] + 1			\
         end					\
      end					\
      if I[1] == N[1] then return nil end	\
      return I,unpack(I)			\
   end						\
   return f,nil,n0				\
end						\
";

static const char IndexIterator2[] = " \
function TempFunc(A)		       \
   local N  =  A:shape()	       \
   local I = { }		       \
						\
   for d=1,#N-1 do I[d] = 0 end			\
   I[#N] = -1					\
						\
   local function f()				\
      I[#N] = I[#N] + 1				\
      for d=#N,2,-1 do				\
         if I[d] == N[d] then			\
            I[d] = 0				\
            I[d-1] = I[d-1] + 1			\
         end					\
      end					\
      if I[1] == N[1] then return nil end	\
      return unpack(I)				\
   end						\
   return f					\
end						\
";



void _lunar_register_array(lua_State *L, struct Array *B)
{
  lua_newtable(L);

  lua_pushcfunction(L, luaC_array_dtype);
  lua_setfield(L, -2, "dtype");

  lua_pushcfunction(L, luaC_array_shape);
  lua_setfield(L, -2, "shape");

  lua_pushcfunction(L, luaC_array_astable);
  lua_setfield(L, -2, "astable");

  LUA_CHUNK_TO_MEMBER(L, IndexIterator2, indices);

  struct Array *A = (struct Array*) lua_newuserdata(L, sizeof(struct Array));
  *A = *B;

  lua_setfield(L, -2, "__cstruct");
  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);
}




int luaC_array_dtype(lua_State *L)
{
  struct Array *A = lunar_checkarray1(L, 1);
  lua_pushstring(L, array_typename(A->dtype));
  return 1;
}

int luaC_array_shape(lua_State *L)
{
  struct Array *A = lunar_checkarray1(L, 1);
  lunar_pusharray2(L, A->shape, ARRAY_TYPE_INT, A->ndims);
  lua_insert(L, 1);
  return luaC_array_astable(L);
}

int luaC_array_astable(lua_State *L)
{
  lunar_astable(L, 1);
  return 1;
}
