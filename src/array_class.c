

#define LUNAR_PRIVATE_API
#include <string.h>
#include "lunar.h"
#include "lauxlib.h"


static int luaC_array_dtype(lua_State *L);
static int luaC_array_shape(lua_State *L);
static int luaC_array_size(lua_State *L);
static int luaC_array_astable(lua_State *L);
static int luaC_array_astype(lua_State *L);


void _lunar_register_array(lua_State *L, struct Array *B)
{
  lua_newtable(L);

  lua_pushcfunction(L, luaC_array_dtype);
  lua_setfield(L, -2, "dtype");

  lua_pushcfunction(L, luaC_array_shape);
  lua_setfield(L, -2, "shape");

  lua_pushcfunction(L, luaC_array_size);
  lua_setfield(L, -2, "size");

  lua_pushcfunction(L, luaC_array_astable);
  lua_setfield(L, -2, "astable");

  lua_pushcfunction(L, luaC_array_astype);
  lua_setfield(L, -2, "astype");

  lua_getglobal(L, "lunar");
  lua_getfield(L, -1, "__array_methods");
  lua_getfield(L, -1, "indices");
  lua_setfield(L, -4, "indices");
  lua_pop(L, 2);

  struct Array *A = (struct Array*) lua_newuserdata(L, sizeof(struct Array));
  *A = *B;

  lua_setfield(L, -2, "__cstruct");
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
  struct Array *A = lunar_checkarray1(L, 1);

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
  struct Array *A = lunar_checkarray1(L, 1);
  lunar_pusharray2(L, A->shape, ARRAY_TYPE_INT, A->ndims);

  if (lua_isstring(L, 2)) {
    if (strcmp(lua_tostring(L, 2), "array") == 0) {
      return 1;
    }
  }

  lunar_astable(L, 2);
  lua_replace(L, -2);
  return 1;
}

int luaC_array_size(lua_State *L)
{
  struct Array *A = lunar_checkarray1(L, 1);
  lua_pushnumber(L, A->size);
  return 1;
}

int luaC_array_astable(lua_State *L)
{
  lunar_astable(L, 1);
  return 1;
}

int luaC_array_astype(lua_State *L)
{
  struct Array *A = lunar_checkarray1(L, 1);
  const enum ArrayType T = (enum ArrayType) luaL_checkinteger(L, 2);
  struct Array B = array_new_copy(A, T);
  lunar_pusharray1(L, &B);
  return 1;
}
