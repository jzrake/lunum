

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#include "lualib.h"
#include "lauxlib.h"

#include "numarray.h"



#define LUA_NEW_METAMETHOD(luastate, obj, funcname) {           \
    lua_pushfstring((luastate), "__%s", (#funcname));           \
    lua_pushcfunction((luastate), (luaC_##obj##__##funcname));  \
    lua_settable((luastate), -3);                               \
  }

#define LUA_NEW_MODULEMETHOD(luastate, obj, funcname) {         \
    lua_pushfstring((luastate), "%s", (#funcname));             \
    lua_pushcfunction((luastate), (luaC_##obj##_##funcname));   \
    lua_settable((luastate), -3);                               \
  }

#define LUA_NEW_MODULEDATA(luastate, obj, dataname) {		\
    lua_pushstring((luastate), (#dataname));			\
    lua_pushnumber((luastate), (obj));				\
    lua_settable((luastate), -3);                               \
  }



static int luaC_numlua_array(lua_State *L);
static int luaC_numlua_zeros(lua_State *L);

static int luaC_array__tostring(lua_State *L);
static int luaC_array__len(lua_State *L);
static int luaC_array__index(lua_State *L);
static int luaC_array__newindex(lua_State *L);
static int luaC_array__add(lua_State *L);
static int luaC_array__sub(lua_State *L);
static int luaC_array__mul(lua_State *L);
static int luaC_array__div(lua_State *L);
static int luaC_array__pow(lua_State *L);
static int luaC_array__gc(lua_State *L);


static int luaC_complex__tostring(lua_State *L);
static int luaC_complex__add(lua_State *L);
static int luaC_complex__sub(lua_State *L);
static int luaC_complex__mul(lua_State *L);
static int luaC_complex__div(lua_State *L);
static int luaC_complex__pow(lua_State *L);


static int   _array_binary_op1(lua_State *L, enum ArrayOperation op);
static int   _array_binary_op2(lua_State *L, enum ArrayOperation op);
static void *_array_tovalue(lua_State *L, enum ArrayType T);

static int _complex_binary_op1(lua_State *L, enum ArrayOperation op);
static int _complex_binary_op2(lua_State *L, enum ArrayOperation op);

static Complex ImaginaryUnit = I;




int luaopen_numlua(lua_State *L)
{
  lua_pop(L, 1); // don't need the argument 'numlua'

  // Create the 'array' metatable
  // ---------------------------------------------------------------------------
  luaL_newmetatable(L, "array");
  LUA_NEW_METAMETHOD(L, array, tostring);
  LUA_NEW_METAMETHOD(L, array, len);
  LUA_NEW_METAMETHOD(L, array, index);
  LUA_NEW_METAMETHOD(L, array, newindex);
  LUA_NEW_METAMETHOD(L, array, add);
  LUA_NEW_METAMETHOD(L, array, sub);
  LUA_NEW_METAMETHOD(L, array, mul);
  LUA_NEW_METAMETHOD(L, array, div);
  LUA_NEW_METAMETHOD(L, array, pow);
  LUA_NEW_METAMETHOD(L, array, gc);
  lua_pop(L, 1);


  // Create the 'complex' metatable
  // ---------------------------------------------------------------------------
  luaL_newmetatable(L, "complex");
  LUA_NEW_METAMETHOD(L, complex, tostring);
  LUA_NEW_METAMETHOD(L, complex, add);
  LUA_NEW_METAMETHOD(L, complex, sub);
  LUA_NEW_METAMETHOD(L, complex, mul);
  LUA_NEW_METAMETHOD(L, complex, div);
  LUA_NEW_METAMETHOD(L, complex, pow);
  lua_pop(L, 1);


  // Create the 'numlua' table
  // ---------------------------------------------------------------------------
  lua_newtable(L);
  LUA_NEW_MODULEMETHOD(L, numlua, array);
  LUA_NEW_MODULEMETHOD(L, numlua, zeros);

  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_CHAR   , char);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_SHORT  , short);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_INT    , int);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_LONG   , long);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_FLOAT  , float);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_DOUBLE , double);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_COMPLEX, complex);

  // Register the purely imaginary number 'I'
  lua_pushlightuserdata(L, &ImaginaryUnit);
  luaL_getmetatable(L, "complex");
  lua_setmetatable(L, -2);
  lua_setfield(L, 1, "I");

  lua_setglobal(L, "numlua");


  return 0;
}




int luaC_array__gc(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  array_del(A);
  return 0;
}

int luaC_array__tostring(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");

  lua_pushstring(L, "  [ ");
  int nstr = 1;
  for (size_t n=0; n<A->size; ++n) {

    char s[64];

    switch (A->type) {
    case ARRAY_TYPE_CHAR    : sprintf(s, "%d" , ((char   *)A->data)[n]); break;
    case ARRAY_TYPE_SHORT   : sprintf(s, "%d" , ((short  *)A->data)[n]); break;
    case ARRAY_TYPE_INT     : sprintf(s, "%d" , ((int    *)A->data)[n]); break;
    case ARRAY_TYPE_LONG    : sprintf(s, "%ld", ((long   *)A->data)[n]); break;
    case ARRAY_TYPE_FLOAT   : sprintf(s, "%g" , ((float  *)A->data)[n]); break;
    case ARRAY_TYPE_DOUBLE  : sprintf(s, "%g" , ((double *)A->data)[n]); break;
    case ARRAY_TYPE_COMPLEX : sprintf(s, "%g+%gj",
				      creal(((Complex*)A->data)[n]),
				      cimag(((Complex*)A->data)[n])); break;

    }

    if (n == A->size-1) {
      lua_pushfstring(L, "%s", s);
    }
    else {
      lua_pushfstring(L, "%s, ", s);
    }
    if ((n+1) % 10 == 0 && n != 0 && n != A->size-1) {
      lua_pushstring(L, "\n    "); ++nstr;
    }
  }
  lua_pushstring(L, " ]"); ++nstr;
  lua_concat(L, A->size + nstr);

  return 1;
}

int luaC_array__len(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  lua_pushnumber(L, A->size);
  return 1;
}

int luaC_array__index(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  const int n = luaL_checkinteger(L, 2);

  if (n >= A->size) {
    luaL_error(L, "index %d out of bounds on array of length %d", n, A->size);
  }

  switch (A->type) {
  case ARRAY_TYPE_CHAR    : lua_pushnumber(L, ((char   *)A->data)[n]); break;
  case ARRAY_TYPE_SHORT   : lua_pushnumber(L, ((short  *)A->data)[n]); break;
  case ARRAY_TYPE_INT     : lua_pushnumber(L, ((int    *)A->data)[n]); break;
  case ARRAY_TYPE_LONG    : lua_pushnumber(L, ((long   *)A->data)[n]); break;
  case ARRAY_TYPE_FLOAT   : lua_pushnumber(L, ((float  *)A->data)[n]); break;
  case ARRAY_TYPE_DOUBLE  : lua_pushnumber(L, ((double *)A->data)[n]); break;
  case ARRAY_TYPE_COMPLEX :
    lua_pushlightuserdata(L, ((Complex*)A->data)+n);
    luaL_getmetatable(L, "complex");
    lua_setmetatable(L, -2);
    break;
  }
  return 1;
}

int luaC_array__newindex(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  const int n = luaL_checkinteger(L, 2);
  const enum ArrayType T = A->type;

  if (n >= A->size) {
    luaL_error(L, "index %d out of bounds on array of length %d", n, A->size);
  }

  void *val = _array_tovalue(L, T);
  memcpy(A->data + array_sizeof(T)*n, val, array_sizeof(T));
  free(val);

  return 0;
}

int luaC_array__add(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_ADD); }
int luaC_array__sub(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_SUB); }
int luaC_array__mul(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_MUL); }
int luaC_array__div(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_DIV); }
int luaC_array__pow(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_POW); }





int _array_binary_op1(lua_State *L, enum ArrayOperation op)
{
  if (!lua_isuserdata(L, 1)) {

    struct Array *B = (struct Array*) luaL_checkudata(L, 2, "array");
    void *val = _array_tovalue(L, B->type);

    lua_getglobal(L, "numlua");
    lua_getfield(L, -1, "zeros");
    lua_pushnumber(L, B->size);
    lua_pushnumber(L, B->type);
    lua_call(L, 2, 1);
    lua_replace(L, 1);

    struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
    array_assign_from_scalar(A, &val);
    free(val);
    lua_pop(L, 1);
  }

  if (!lua_isuserdata(L, 2)) {

    struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
    void *val = _array_tovalue(L, A->type);

    lua_getglobal(L, "numlua");
    lua_getfield(L, -1, "zeros");
    lua_pushnumber(L, A->size);
    lua_pushnumber(L, A->type);
    lua_call(L, 2, 1);
    lua_replace(L, 2);

    struct Array *B = (struct Array*) luaL_checkudata(L, 2, "array");
    array_assign_from_scalar(B, val);
    free(val);
    lua_pop(L, 1);
  }

  return _array_binary_op2(L, op);
}

int _array_binary_op2(lua_State *L, enum ArrayOperation op)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  struct Array *B = (struct Array*) luaL_checkudata(L, 2, "array");

  if (A->size != B->size) {
    luaL_error(L, "arrays could not be broadcast together with shapes (%d) (%d)",
	       A->size, B->size);
  }
  const size_t N = A->size;
  enum ArrayType T = (A->type >= B->type) ? A->type : B->type;

  struct Array A_ = (A->type == T) ? *A : array_new_copy(A, T);
  struct Array B_ = (B->type == T) ? *B : array_new_copy(B, T);

  struct Array *C = (struct Array*) lua_newuserdata(L, sizeof(struct Array));
  *C = array_new_zeros(N, T);

  array_binary_op(&A_, &B_, C, op);

  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);

  if (A->type != T) array_del(&A_);
  if (B->type != T) array_del(&B_);

  return 1;
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



int luaC_numlua_array(lua_State *L)
{
  const size_t N = lua_objlen(L, 1);
  const size_t T = luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);

  struct Array *A = (struct Array*) lua_newuserdata(L, sizeof(struct Array));
  *A = array_new_zeros(N, T);

  for (size_t i=0; i<A->size; ++i) {

    lua_pushnumber(L, i+1);
    lua_gettable(L, 1);

    void *val = _array_tovalue(L, T);
    memcpy(A->data + array_sizeof(T)*i, val, array_sizeof(T));
    free(val);

    lua_pop(L, 1);
  }

  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);

  return 1;
}

int luaC_numlua_zeros(lua_State *L)
{
  const size_t N = luaL_checkinteger(L, 1);
  const size_t T = luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);
  struct Array A = array_new_zeros(N, T);

  struct Array *B = (struct Array *) lua_newuserdata(L, sizeof(struct Array));
  *B = A;

  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);

  return 1;
}



int luaC_complex__tostring(lua_State *L)
{
  Complex z = *((Complex*) luaL_checkudata(L, 1, "complex"));
  lua_pushfstring(L, "%f + %fj", creal(z), cimag(z));
  return 1;
}

int _complex_binary_op1(lua_State *L, enum ArrayOperation op)
{
  if (!lua_isuserdata(L, 1)) {
    double a = lua_tonumber(L, 1);
    Complex *v = (Complex*) lua_newuserdata(L, sizeof(Complex));
    luaL_getmetatable(L, "complex");
    lua_setmetatable(L, -2);

    *v = a;
    lua_replace(L, 1);
  }

  if (!lua_isuserdata(L, 2)) {
    double a = lua_tonumber(L, 2);
    Complex *w = (Complex*) lua_newuserdata(L, sizeof(Complex));
    luaL_getmetatable(L, "complex");
    lua_setmetatable(L, -2);

    *w = a;
    lua_replace(L, 2);
  }

  return _complex_binary_op2(L, op);
}

int _complex_binary_op2(lua_State *L, enum ArrayOperation op)
{
  Complex v = *((Complex*) luaL_checkudata(L, 1, "complex"));
  Complex w = *((Complex*) luaL_checkudata(L, 2, "complex"));

  Complex *z = (Complex*) lua_newuserdata(L, sizeof(Complex));
  luaL_getmetatable(L, "complex");
  lua_setmetatable(L, -2);

  switch (op) {
  case ARRAY_OP_ADD: *z = v + w; break;
  case ARRAY_OP_SUB: *z = v - w; break;
  case ARRAY_OP_MUL: *z = v * w; break;
  case ARRAY_OP_DIV: *z = v / w; break;
  case ARRAY_OP_POW: *z = cpow(v,w); break;
  }

  return 1;
}


int luaC_complex__add(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_ADD); }
int luaC_complex__sub(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_SUB); }
int luaC_complex__mul(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_MUL); }
int luaC_complex__div(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_DIV); }
int luaC_complex__pow(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_POW); }
