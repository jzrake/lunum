

#define LUNAR_PRIVATE_API
#include "lunar.h"
#include "lauxlib.h"


static int luaC_array_dtype(lua_State *L);
static int luaC_array_shape(lua_State *L);
static int luaC_array_astable(lua_State *L);


void _lunar_register_array(lua_State *L, struct Array *B)
{
  lua_newtable(L);

  lua_pushcfunction(L, luaC_array_dtype);
  lua_setfield(L, -2, "dtype");

  lua_pushcfunction(L, luaC_array_shape);
  lua_setfield(L, -2, "shape");

  lua_pushcfunction(L, luaC_array_astable);
  lua_setfield(L, -2, "astable");

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
