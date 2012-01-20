

#include <stdio.h>
#include <stdlib.h>

#include "numarray.h"
#include "lualib.h"
#include "lauxlib.h"


void luaU_stack_dump(lua_State *L);
static int luaC_test_api1(lua_State *L);
static int luaC_test_api2(lua_State *L);
static void *numlua_checkarray(lua_State *L, int k, enum ArrayType T, size_t *N);



int main()
{
  lua_State *L = lua_open();
  luaL_openlibs(L);

  lua_register(L, "test_api1", luaC_test_api1);
  lua_register(L, "test_api2", luaC_test_api2);

  if (luaL_dofile(L, "src/test_api.lua")) {
    printf("%s\n", lua_tostring(L, -1));
  }

  lua_close(L);
  return 0;
}






int luaC_test_api1(lua_State *L)
{
  size_t N;
  double *A = (double*) numlua_checkarray(L, 1, ARRAY_TYPE_DOUBLE, &N);
  printf("#A = %ld, A[0], A[1] = %f %f\n", N, A[0], A[1]);
  return 0;
}

int luaC_test_api2(lua_State *L)
{
  size_t Na, Nb;

  double *A = (double*) numlua_checkarray(L, 1, ARRAY_TYPE_DOUBLE, &Na);
  double *B = (double*) numlua_checkarray(L, 2, ARRAY_TYPE_DOUBLE, &Nb);

  printf("#A = %ld, A[0], A[1] = %f %f\n", Na, A[0], A[1]);
  printf("#B = %ld, B[0], B[1] = %f %f\n", Nb, B[0], B[1]);

  return 0;
}




void *numlua_checkarray(lua_State *L, int k, enum ArrayType T, size_t *N)
{
  lua_pushvalue(L, k);
  const int top = lua_gettop(L);

  if (lua_type(L, -1) == LUA_TTABLE) {
    lua_getglobal(L, "numlua");
    lua_getfield(L, -1, "array");
    lua_insert(L, -3);
    lua_pop(L, 1);
    lua_call(L, 1, 1);
  }

  struct Array *A = (struct Array*) luaL_checkudata(L, top, "array");
  if (A->type != T) {
    luaL_error(L, "expected array of type double, got %d\n", A->type);
  }
  lua_replace(L, k);

  if (N != NULL) *N = A->size;
  return A->data;
}





void luaU_stack_dump(lua_State *L)
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

