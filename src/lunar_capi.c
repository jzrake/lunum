

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LUNAR_PRIVATE_API
#include "lunar.h"
#include "lauxlib.h"


void lunar_pusharray1(lua_State *L, struct Array *B)
{
  _lunar_register_array(L, B);
}

void lunar_pusharray2(lua_State *L, void *data, enum ArrayType T, int N)
{
  struct Array A = array_new_zeros(N, T);
  memcpy(A.data, data, N*array_sizeof(T));
  lunar_pusharray1(L, &A);
}


struct Array *lunar_checkarray1(lua_State *L, int pos)
{
  if (!lunar_hasmetatable(L, pos, "array")) {
    luaL_error(L, "bad argument #%d (array expected, got %s)",
               pos, lua_typename(L, lua_type(L, pos)));
  }
  lua_pushstring(L, "__cstruct");
  lua_rawget(L, pos);

  struct Array* A = (struct Array*) lua_touserdata(L, -1);
  lua_pop(L, 1);

  return A;
}

void *lunar_checkarray2(lua_State *L, int pos, enum ArrayType T, int *N)
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

  struct Array *A = lunar_checkarray1(L, top);
  if (A->dtype != T) {
    luaL_error(L, "expected array of type %s, got %s\n",
               array_typename(T), array_typename(A->dtype));
  }
  lua_replace(L, pos);

  if (N != NULL) *N = A->size;
  return A->data;
}





void lunar_astable(lua_State *L, int pos)
{
  struct Array *A = lunar_checkarray1(L, pos);
  const void *a = A->data;

  lua_newtable(L);
  for (int i=0; i<A->size; ++i) {

    lua_pushnumber(L, i+1);

    switch (A->dtype) {
    case ARRAY_TYPE_CHAR    : lua_pushnumber(L, ((char    *)a)[i]); break;
    case ARRAY_TYPE_SHORT   : lua_pushnumber(L, ((short   *)a)[i]); break;
    case ARRAY_TYPE_INT     : lua_pushnumber(L, ((int     *)a)[i]); break;
    case ARRAY_TYPE_LONG    : lua_pushnumber(L, ((long    *)a)[i]); break;
    case ARRAY_TYPE_FLOAT   : lua_pushnumber(L, ((float   *)a)[i]); break;
    case ARRAY_TYPE_DOUBLE  : lua_pushnumber(L, ((double  *)a)[i]); break;
    case ARRAY_TYPE_COMPLEX : lunar_pushcomplex(L, ((Complex *)a)[i]); break;
    }

    lua_settable(L, -3);
  }
}

void lunar_pushcomplex(lua_State *L, Complex z)
{
  Complex *w = (Complex*) lua_newuserdata(L, sizeof(Complex));
  luaL_getmetatable(L, "complex");
  lua_setmetatable(L, -2);
  *w = z;
}

Complex lunar_checkcomplex(lua_State *L, int n)
{
  Complex *w = (Complex*) luaL_checkudata(L, n, "complex");
  return *w;
}



int lunar_upcast(lua_State *L, int pos, enum ArrayType T, int N)
// -----------------------------------------------------------------------------
// If the object at position 'pos' is already an array of dtype 'T', then push
// nothing and return 0. If the dtype is not 'T', then return 1 and push a copy
// of that array with dtype 'T' onto the stack. If it is a table, then push an
// array of dtype 'T' having the length of the table. If it is a number or
// complex, then push an array of dtype float or complex respectively having
// length 'N'.
// -----------------------------------------------------------------------------
{
  // Deal with lunar.array
  // ---------------------------------------------------------------------------
  if (lunar_hasmetatable(L, pos, "array")) {

    struct Array *A = lunar_checkarray1(L, pos);

    if (A->dtype == T) {
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

    for (int i=0; i<A.size; ++i) {

      lua_pushnumber(L, i+1);
      lua_gettable(L, pos);

      void *val = lunar_tovalue(L, T);
      memcpy((char*)A.data + array_sizeof(T)*i, val, array_sizeof(T));
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
    luaL_error(L, "cannot cast to array from object of dtype %s\n",
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

