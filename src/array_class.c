

#define LUNUM_PRIVATE_API
#include <stdlib.h>
#include <string.h>
#include "lunum.h"
#include "lauxlib.h"


static int luaC_array_dtype(lua_State *L);
static int luaC_array_shape(lua_State *L);
static int luaC_array_lower(lua_State *L);
static int luaC_array_size(lua_State *L);
static int luaC_array_astable(lua_State *L);
static int luaC_array_astype(lua_State *L);


void _lunum_register_array(lua_State *L, struct Array *B)
{
  lua_newtable(L);

  lua_pushcfunction(L, luaC_array_dtype);
  lua_setfield(L, -2, "dtype");

  lua_pushcfunction(L, luaC_array_shape);
  lua_setfield(L, -2, "shape");

  lua_pushcfunction(L, luaC_array_lower);
  lua_setfield(L, -2, "lower");

  lua_pushcfunction(L, luaC_array_size);
  lua_setfield(L, -2, "size");

  lua_pushcfunction(L, luaC_array_astable);
  lua_setfield(L, -2, "astable");

  lua_pushcfunction(L, luaC_array_astype);
  lua_setfield(L, -2, "astype");


  // Calls code written in Lua: lunum.__register_array(new_array) to add
  // additional methods to the new array.
  // ---------------------------------------------------------------------------
  lua_getglobal(L, "lunum");
  lua_getfield(L, -1, "__register_array");
  lua_pushvalue(L, -3);
  lua_call(L, 1, 0);
  lua_pop(L, 1);


  void *data = lua_newuserdata(L, B->size * array_sizeof(B->dtype));
  lua_setfield(L, -2, "__buffer");

  memcpy(data, B->data, B->size * array_sizeof(B->dtype));
  free(B->data);
  B->owns = 0;
  B->data = data;

  struct Array *A = (struct Array*) lua_newuserdata(L, sizeof(struct Array));
  lua_setfield(L, -2, "__cstruct");
  *A = *B;

  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);
}




int luaC_array_dtype(lua_State *L)
// -----------------------------------------------------------------------------
// If there is no argument, return a string description of the data type. If
// the string 'enum' is given as the first argument, then return the enumated
// value of the Array's type.
// -----------------------------------------------------------------------------
{
  struct Array *A = lunum_checkarray1(L, 1);

  if (lua_isstring(L, 2)) {
    if (strcmp(lua_tostring(L, 2), "enum") == 0) {
      lua_pushnumber(L, A->dtype);
      return 1;
    }
  }

  lua_pushstring(L, array_typename(A->dtype));
  return 1;
}

int luaC_array_shape(lua_State *L)
// -----------------------------------------------------------------------------
// If there is no argument, return the shape as a table. If the string 'array'
// is given, return it as an array.
// -----------------------------------------------------------------------------
{
  struct Array *A = lunum_checkarray1(L, 1);
  lunum_pusharray2(L, A->shape, ARRAY_TYPE_INT, A->ndims);

  if (lua_isstring(L, 2)) {
    if (strcmp(lua_tostring(L, 2), "array") == 0) {
      return 1;
    }
  }

  lunum_astable(L, 2);
  lua_replace(L, -2);
  return 1;
}

int luaC_array_lower(lua_State *L)
// -----------------------------------------------------------------------------
// If there is no argument, return the index lower-bound as a table. If the
// string 'array' is given, return it as an array.
// -----------------------------------------------------------------------------
{
  struct Array *A = lunum_checkarray1(L, 1);
  lunum_pusharray2(L, A->lower, ARRAY_TYPE_INT, A->ndims);

  if (lua_isstring(L, 2)) {
    if (strcmp(lua_tostring(L, 2), "array") == 0) {
      return 1;
    }
  }

  lunum_astable(L, 2);
  lua_replace(L, -2);
  return 1;
}

int luaC_array_size(lua_State *L)
{
  struct Array *A = lunum_checkarray1(L, 1);
  lua_pushnumber(L, A->size);
  return 1;
}

int luaC_array_astable(lua_State *L)
{
  lunum_astable(L, 1);
  return 1;
}

int luaC_array_astype(lua_State *L)
{
  struct Array *A = lunum_checkarray1(L, 1);
  const enum ArrayType T = (enum ArrayType) luaL_checkinteger(L, 2);
  struct Array B = array_new_copy(A, T);
  lunum_pusharray1(L, &B);
  return 1;
}
