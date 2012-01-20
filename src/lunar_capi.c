

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lunar.h"
#include "lauxlib.h"


static void *_array_tovalue(lua_State *L, enum ArrayType T);
static int   _hasmetatable(lua_State *L, int pos, const char *name);
void _stack_dump(lua_State *L);


struct Array *lunar_checkarray1(lua_State *L, int pos)
{
  return (struct Array*) luaL_checkudata(L, pos, "array");
}

void *lunar_checkarray2(lua_State *L, int pos, enum ArrayType T, size_t *N)
{
  lua_pushvalue(L, pos);
  const int top = lua_gettop(L);

  if (lua_type(L, -1) == LUA_TTABLE) {
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



void lunar_upcast(lua_State *L, int pos, enum ArrayType T, size_t N)
// -----------------------------------------------------------------------------
// If the object at position 'pos' is already an array of type greater than or
// equal to 'T', then do nothing. If the type is less than 'T', then upcast it
// to 'T'. If it is a table, then replace it with an array of type 'T' having
// the length of the table. If it is a number or complex, then replace it with
// an array of type number or complex respectively having length 'N'.
// -----------------------------------------------------------------------------
{
  // Deal with lunar.array
  // ---------------------------------------------------------------------------
  if (_hasmetatable(L, pos, "array")) {

    struct Array *A = (struct Array*) lua_touserdata(L, pos);

    if (A->type >= T) {
      return;
    }

    else {

      struct Array A_ = array_new_copy(A, T);
      lunar_pusharray1(L, &A_);
      lua_replace(L, pos);
      return;
    }
  }

  // Deal with Lua table
  // ---------------------------------------------------------------------------
  else if (lua_istable(L, pos)) {

    struct Array A = array_new_zeros(lua_objlen(L, pos), T);

    for (size_t i=0; i<A.size; ++i) {

      lua_pushnumber(L, i+1);
      lua_gettable(L, 1);

      void *val = _array_tovalue(L, T);
      memcpy(A.data + array_sizeof(T)*i, val, array_sizeof(T));
      free(val);

      lua_pop(L, 1);
    }
    lunar_pusharray1(L, &A);
    lua_replace(L, pos);
    return;
  }

  // Deal with Lua numbers
  // ---------------------------------------------------------------------------
  else if (lua_isnumber(L, pos)) {
    printf("it's a number!\n");
  }

  // Deal with lunar.complex
  // ---------------------------------------------------------------------------
  else if (_hasmetatable(L, pos, "complex")) {
    printf("it's a complex!\n");
  }

  // Throw an error
  // ---------------------------------------------------------------------------
  else {
    printf("it's a %s\n", lua_typename(L, lua_type(L, pos)));
  }
}


int _hasmetatable(lua_State *L, int pos, const char *name)
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







void *_array_tovalue(lua_State *L, enum ArrayType T)
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

