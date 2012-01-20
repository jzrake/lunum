

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "numlua_capi.h"
#include "lauxlib.h"


void *numlua_checkarray(lua_State *L, int pos, enum ArrayType T, size_t *N)
{
  lua_pushvalue(L, pos);
  const int top = lua_gettop(L);

  if (lua_type(L, -1) == LUA_TTABLE) {
    lua_getglobal(L, "numlua");
    lua_getfield(L, -1, "array");
    lua_insert(L, -3);
    lua_pop(L, 1);
    lua_pushnumber(L, T);
    lua_call(L, 2, 1);
  }

  printf("%s\n", array_typename(T));

  struct Array *A = (struct Array*) luaL_checkudata(L, top, "array");
  if (A->type != T) {
    luaL_error(L, "expected array of type %s, got %s\n",
	       array_typename(T), array_typename(A->type));
  }
  lua_replace(L, pos);

  if (N != NULL) *N = A->size;
  return A->data;
}


void numlua_pusharray(lua_State *L, void *data, enum ArrayType T, size_t N)
{
  struct Array *A = (struct Array*) lua_newuserdata(L, sizeof(struct Array));

  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);

  *A = array_new_zeros(N, T);
  memcpy(A->data, data, N*array_sizeof(T)); 
}
