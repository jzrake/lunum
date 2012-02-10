

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LUNUM_PRIVATE_API
#include "lunum.h"
#include "lauxlib.h"


void lunum_pusharray1(lua_State *L, struct Array *B)
{
  _lunum_register_array(L, B);
}

void lunum_pusharray2(lua_State *L, void *data, enum ArrayType T, int N)
{
  struct Array A = array_new_zeros(N, T);
  memcpy(A.data, data, N*array_sizeof(T));
  lunum_pusharray1(L, &A);
}

struct Array *lunum_checkarray1(lua_State *L, int pos)
{
  lua_pushvalue(L, pos);

  if (!lunum_hasmetatable(L, -1, "array")) {
    luaL_error(L, "bad argument #%d (array expected, got %s)",
               pos, lua_typename(L, lua_type(L, -1)));
  }
  lua_pushstring(L, "__cstruct");
  lua_rawget(L, -2);

  struct Array *A = (struct Array*) lua_touserdata(L, -1);
  lua_pop(L, 2);

  return A;
}

void *lunum_checkarray2(lua_State *L, int pos, enum ArrayType T, int *N)
{
  if (lunum_upcast(L, pos, T, 1)) {
    lua_replace(L, pos);
  }
  struct Array *A = lunum_checkarray1(L, pos);
  if (N != NULL) *N = A->size;
  return A->data;
}


void lunum_astable(lua_State *L, int pos)
{
  struct Array *A = lunum_checkarray1(L, pos);
  const void *a = A->data;

  lua_newtable(L);
  for (int i=0; i<A->size; ++i) {

    lua_pushnumber(L, i+1);

    switch (A->dtype) {
    case ARRAY_TYPE_BOOL    : lua_pushboolean  (L, ((Bool    *)a)[i]); break;
    case ARRAY_TYPE_CHAR    : lua_pushnumber   (L, ((char    *)a)[i]); break;
    case ARRAY_TYPE_SHORT   : lua_pushnumber   (L, ((short   *)a)[i]); break;
    case ARRAY_TYPE_INT     : lua_pushnumber   (L, ((int     *)a)[i]); break;
    case ARRAY_TYPE_LONG    : lua_pushnumber   (L, ((long    *)a)[i]); break;
    case ARRAY_TYPE_FLOAT   : lua_pushnumber   (L, ((float   *)a)[i]); break;
    case ARRAY_TYPE_DOUBLE  : lua_pushnumber   (L, ((double  *)a)[i]); break;
    case ARRAY_TYPE_COMPLEX : lunum_pushcomplex(L, ((Complex *)a)[i]); break;
    }

    lua_settable(L, -3);
  }
}

void lunum_pushcomplex(lua_State *L, Complex z)
{
  Complex *w = (Complex*) lua_newuserdata(L, sizeof(Complex));
  luaL_getmetatable(L, "complex");
  lua_setmetatable(L, -2);
  *w = z;
}

Complex lunum_checkcomplex(lua_State *L, int n)
{
  Complex *w = (Complex*) luaL_checkudata(L, n, "complex");
  return *w;
}



int lunum_upcast(lua_State *L, int pos, enum ArrayType T, int N)
// -----------------------------------------------------------------------------
// If the object at position 'pos' is already an array of dtype 'T', then push
// nothing and return 0. If the dtype is not 'T', then return 1 and push a copy
// of that array with dtype 'T' onto the stack. If it is a table, then push an
// array of dtype 'T' having the length of the table. If it is a number or
// complex, then push an array of dtype float or complex respectively having
// length 'N'.
// -----------------------------------------------------------------------------
{
  // Deal with lunum.array
  // ---------------------------------------------------------------------------
  if (lunum_hasmetatable(L, pos, "array")) {

    struct Array *A = lunum_checkarray1(L, pos);

    if (A->dtype == T) {
      return 0;
    }

    else {

      struct Array A_ = array_new_copy(A, T);
      lunum_pusharray1(L, &A_);
      return 1;
    }
  }

  // Deal with Lua table
  // ---------------------------------------------------------------------------
  else if (lua_istable(L, pos)) {

    struct Array A = array_new_zeros(lua_rawlen(L, pos), T);

    for (int i=0; i<A.size; ++i) {

      lua_pushnumber(L, i+1);
      lua_gettable(L, pos);

      void *val = lunum_tovalue(L, T);
      memcpy((char*)A.data + array_sizeof(T)*i, val, array_sizeof(T));
      free(val);

      lua_pop(L, 1);
    }
    lunum_pusharray1(L, &A);

    return 1;
  }

  // Deal with Lua bool
  // ---------------------------------------------------------------------------
  else if (lua_isboolean(L, pos)) {
    const Bool x = lua_toboolean(L, pos);
    struct Array A = array_new_zeros(N, ARRAY_TYPE_BOOL);
    array_assign_from_scalar(&A, &x);
    lunum_pusharray1(L, &A);
    return 1;
  }

  // Deal with Lua numbers
  // ---------------------------------------------------------------------------
  else if (lua_isnumber(L, pos)) {
    const double x = lua_tonumber(L, pos);
    struct Array A = array_new_zeros(N, ARRAY_TYPE_DOUBLE);
    array_assign_from_scalar(&A, &x);
    struct Array B = array_new_copy(&A, T);
    array_del(&A);
    lunum_pusharray1(L, &B);
    return 1;
  }

  // Deal with lunum.complex
  // ---------------------------------------------------------------------------
  else if (lunum_hasmetatable(L, pos, "complex")) {

    const Complex z = *((Complex*) lua_touserdata(L, pos));
    struct Array A = array_new_zeros(N, ARRAY_TYPE_COMPLEX);
    array_assign_from_scalar(&A, &z);
    lunum_pusharray1(L, &A);
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


int lunum_hasmetatable(lua_State *L, int pos, const char *name)
{
  const int top = lua_gettop(L);
  if (lua_getmetatable(L, pos) == 0) return 0;
  luaL_getmetatable(L, name);
  const int eq = lua_rawequal(L, -2, -1);
  lua_settop(L, top);
  return eq;
}


void *lunum_tovalue(lua_State *L, enum ArrayType T)
{
  Complex x=0.0;

  if (lua_isnumber(L, -1)) {
    x = lua_tonumber(L, -1);
  }
  else if (lua_isboolean(L, -1)) {
    x = lua_toboolean(L, -1);
  }
  else if (lunum_hasmetatable(L, -1, "complex")) {
    x = *((Complex*) lua_touserdata(L, -1));
  }
  else {
    luaL_error(L, "unkown data type");
  }

  void *y = malloc(array_sizeof(T));

  switch (T) {
  case ARRAY_TYPE_BOOL    : *((Bool   *)y) = x; break;
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

