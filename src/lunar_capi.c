

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lunar.h"
#include "lauxlib.h"



void _stack_dump(lua_State *L);


struct Array *lunar_checkarray1(lua_State *L, int pos)
{
  return (struct Array*) luaL_checkudata(L, pos, "array");
}

void *lunar_checkarray2(lua_State *L, int pos, enum ArrayType T, size_t *N)
{
  lua_pushvalue(L, pos);
  const int top = lua_gettop(L);

  if (lua_istable(L, -1)) {
    lua_getglobal(L, "lunar");
    lua_getfield(L, -1, "array");
    lua_insert(L, -3);
    lua_pop(L, 1);
    lua_pushnumber(L, T);
    lua_call(L, 2, 1);
  }

  struct Array *A = (struct Array*) luaL_checkudata(L, top, "array");
  if (A->type != T) {
    luaL_error(L, "expected array of type %s, got %s\n",
	       array_typename(T), array_typename(A->type));
  }
  lua_replace(L, pos);

  if (N != NULL) *N = A->size;
  return A->data;
}


void lunar_pusharray1(lua_State *L, struct Array *B)
{
  struct Array *A = (struct Array *) lua_newuserdata(L, sizeof(struct Array));
  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);
  *A = *B;
}

void lunar_pusharray2(lua_State *L, void *data, enum ArrayType T, size_t N)
{
  struct Array *A = (struct Array*) lua_newuserdata(L, sizeof(struct Array));
  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);

  *A = array_new_zeros(N, T);
  memcpy(A->data, data, N*array_sizeof(T));
}



int lunar_upcast(lua_State *L, int pos, enum ArrayType T, size_t N)
// -----------------------------------------------------------------------------
// If the object at position 'pos' is already an array of type 'T', then push
// nothing and return 0. If the type is not 'T', then return 1 and push a copy
// of that array with type 'T' onto the stack. If it is a table, then push an
// array of type 'T' having the length of the table. If it is a number or
// complex, then push an array of type float or complex respectively having
// length 'N'.
// -----------------------------------------------------------------------------
{
  // Deal with lunar.array
  // ---------------------------------------------------------------------------
  if (lunar_hasmetatable(L, pos, "array")) {

    struct Array *A = (struct Array*) lua_touserdata(L, pos);

    if (A->type == T) {
      return 0;
    }

    else {

      struct Array A_ = array_new_copy(A, T);
      lunar_pusharray1(L, &A_);
      return 1;
    }
  }

  // Deal with Lua table
  // ---------------------------------------------------------------------------
  else if (lua_istable(L, pos)) {

    struct Array A = array_new_zeros(lua_objlen(L, pos), T);

    for (size_t i=0; i<A.size; ++i) {

      lua_pushnumber(L, i+1);
      lua_gettable(L, pos);

      void *val = lunar_tovalue(L, T);
      memcpy(A.data + array_sizeof(T)*i, val, array_sizeof(T));
      free(val);

      lua_pop(L, 1);
    }
    lunar_pusharray1(L, &A);

    return 1;
  }

  // Deal with Lua numbers
  // ---------------------------------------------------------------------------
  else if (lua_isnumber(L, pos)) {

    const double x = lua_tonumber(L, pos);
    struct Array A = array_new_zeros(N, ARRAY_TYPE_DOUBLE);
    array_assign_from_scalar(&A, &x);
    lunar_pusharray1(L, &A);
    return 1;
  }

  // Deal with lunar.complex
  // ---------------------------------------------------------------------------
  else if (lunar_hasmetatable(L, pos, "complex")) {

    const Complex z = *((Complex*) lua_touserdata(L, pos));
    struct Array A = array_new_zeros(N, ARRAY_TYPE_COMPLEX);
    array_assign_from_scalar(&A, &z);
    lunar_pusharray1(L, &A);
    return 1;
  }

  // Throw an error
  // ---------------------------------------------------------------------------
  else {
    luaL_error(L, "cannot cast to array from object of type %s\n",
	       lua_typename(L, lua_type(L, pos)));
    return 0;
  }
}


int lunar_hasmetatable(lua_State *L, int pos, const char *name)
{
  const int top = lua_gettop(L);
  if (lua_getmetatable(L, pos) == 0) return 0;
  luaL_getmetatable(L, name);
  const int eq = lua_rawequal(L, -2, -1);
  lua_settop(L, top);
  return eq;
}






void _stack_dump(lua_State *L)
{
  const int top = lua_gettop(L);
  printf("lua stack size: %d ... (", top);
  for (int i=1; i<=top; ++i) {

    int t = lua_type(L, i);

    switch (t) {

    case LUA_TSTRING:
      printf("`%s'", lua_tostring(L, i));
      break;

    case LUA_TBOOLEAN:
      printf(lua_toboolean(L, i) ? "true" : "false");
      break;

    case LUA_TNUMBER:
      printf("%g", lua_tonumber(L, i));
      break;

    default:
      printf("%s", lua_typename(L, t));
      break;

    }
    printf(", ");
  }
  printf(")\n");
}







void *lunar_tovalue(lua_State *L, enum ArrayType T)
{
  Complex x;

  if (lua_isnumber(L, -1)) {
    x = lua_tonumber(L, -1);
  }
  else {
    x = *((Complex*) luaL_checkudata(L, -1, "complex"));
  }

  void *y = malloc(array_sizeof(T));

  switch (T) {
  case ARRAY_TYPE_CHAR    : *((char   *)y) = x; break;
  case ARRAY_TYPE_SHORT   : *((short  *)y) = x; break;
  case ARRAY_TYPE_INT     : *((int    *)y) = x; break;
  case ARRAY_TYPE_LONG    : *((long   *)y) = x; break;
  case ARRAY_TYPE_FLOAT   : *((float  *)y) = x; break;
  case ARRAY_TYPE_DOUBLE  : *((double *)y) = x; break;
  case ARRAY_TYPE_COMPLEX : *((Complex*)y) = x; break;
  }

  return y;
}

