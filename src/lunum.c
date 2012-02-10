
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lualib.h"
#include "lauxlib.h"
#include "lunum.h"



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

#define LUA_NEW_MODULEDATA(luastate, obj, dataname) {   \
    lua_pushstring((luastate), (#dataname));            \
    lua_pushnumber((luastate), (obj));                  \
    lua_settable((luastate), -3);                       \
  }



static int luaC_lunum_array(lua_State *L);
static int luaC_lunum_zeros(lua_State *L);
static int luaC_lunum_range(lua_State *L);
static int luaC_lunum_resize(lua_State *L);


static int luaC_lunum_sin(lua_State *L);
static int luaC_lunum_cos(lua_State *L);
static int luaC_lunum_tan(lua_State *L);

static int luaC_lunum_sin(lua_State *L);
static int luaC_lunum_cos(lua_State *L);
static int luaC_lunum_tan(lua_State *L);

static int luaC_lunum_asin(lua_State *L);
static int luaC_lunum_acos(lua_State *L);
static int luaC_lunum_atan(lua_State *L);

static int luaC_lunum_sinh(lua_State *L);
static int luaC_lunum_cosh(lua_State *L);
static int luaC_lunum_tanh(lua_State *L);

static int luaC_lunum_asinh(lua_State *L);
static int luaC_lunum_acosh(lua_State *L);
static int luaC_lunum_atanh(lua_State *L);

static int luaC_lunum_exp(lua_State *L);
static int luaC_lunum_log(lua_State *L);
static int luaC_lunum_log10(lua_State *L);
static int luaC_lunum_conjugate(lua_State *L);
static int luaC_lunum_slice(lua_State *L);



static int luaC_array__tostring(lua_State *L);
static int luaC_array__call(lua_State *L);
static int luaC_array__index(lua_State *L);
static int luaC_array__newindex(lua_State *L);
static int luaC_array__add(lua_State *L);
static int luaC_array__sub(lua_State *L);
static int luaC_array__mul(lua_State *L);
static int luaC_array__div(lua_State *L);
static int luaC_array__pow(lua_State *L);
static int luaC_array__unm(lua_State *L);
static int luaC_array__gc(lua_State *L);


static int luaC_complex__tostring(lua_State *L);
static int luaC_complex__add(lua_State *L);
static int luaC_complex__sub(lua_State *L);
static int luaC_complex__mul(lua_State *L);
static int luaC_complex__div(lua_State *L);
static int luaC_complex__pow(lua_State *L);
static int luaC_complex__unm(lua_State *L);
static int luaC_complex__eq(lua_State *L);
static int luaC_complex__lt(lua_State *L);
static int luaC_complex__le(lua_State *L);


static int   _array_binary_op1(lua_State *L, enum ArrayOperation op);
static int   _array_binary_op2(lua_State *L, enum ArrayOperation op);

static int _complex_binary_op1(lua_State *L, enum ArrayOperation op);
static int _complex_binary_op2(lua_State *L, enum ArrayOperation op);

static void _unary_func(lua_State *L, double(*f)(double), Complex(*g)(Complex),
			int cast);
static int _get_index(lua_State *L, struct Array *A);

static Complex ImaginaryUnit = I;


// Functions used by unary predicates
// -----------------------------------------------------------------------------
static double rconj(double x) { return x; }   // conj, but for real argument
static double runm(double x) { return -x; }   // unary minus, real
static Complex cunm(Complex z) { return -z; } // unary minus, complex
// -----------------------------------------------------------------------------





int luaopen_lunum(lua_State *L)
{
  lua_settop(L, 0); // start with an empty stack

  // Create the 'array' metatable
  // ---------------------------------------------------------------------------
  luaL_newmetatable(L, "array");
  LUA_NEW_METAMETHOD(L, array, tostring);
  LUA_NEW_METAMETHOD(L, array, call);
  LUA_NEW_METAMETHOD(L, array, index);
  LUA_NEW_METAMETHOD(L, array, newindex);
  LUA_NEW_METAMETHOD(L, array, add);
  LUA_NEW_METAMETHOD(L, array, sub);
  LUA_NEW_METAMETHOD(L, array, mul);
  LUA_NEW_METAMETHOD(L, array, div);
  LUA_NEW_METAMETHOD(L, array, pow);
  LUA_NEW_METAMETHOD(L, array, unm);
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
  LUA_NEW_METAMETHOD(L, complex, unm);
  LUA_NEW_METAMETHOD(L, complex, eq);
  LUA_NEW_METAMETHOD(L, complex, lt);
  LUA_NEW_METAMETHOD(L, complex, le);
  lua_pop(L, 1);


  // Create the 'lunum' table
  // ---------------------------------------------------------------------------
  lua_newtable(L);
  LUA_NEW_MODULEMETHOD(L, lunum, array);
  LUA_NEW_MODULEMETHOD(L, lunum, zeros);
  LUA_NEW_MODULEMETHOD(L, lunum, range);
  LUA_NEW_MODULEMETHOD(L, lunum, resize);

  LUA_NEW_MODULEMETHOD(L, lunum, sin);
  LUA_NEW_MODULEMETHOD(L, lunum, cos);
  LUA_NEW_MODULEMETHOD(L, lunum, tan);

  LUA_NEW_MODULEMETHOD(L, lunum, asin);
  LUA_NEW_MODULEMETHOD(L, lunum, acos);
  LUA_NEW_MODULEMETHOD(L, lunum, atan);

  LUA_NEW_MODULEMETHOD(L, lunum, sinh);
  LUA_NEW_MODULEMETHOD(L, lunum, cosh);
  LUA_NEW_MODULEMETHOD(L, lunum, tanh);

  LUA_NEW_MODULEMETHOD(L, lunum, asinh);
  LUA_NEW_MODULEMETHOD(L, lunum, acosh);
  LUA_NEW_MODULEMETHOD(L, lunum, atanh);

  LUA_NEW_MODULEMETHOD(L, lunum, exp);
  LUA_NEW_MODULEMETHOD(L, lunum, log);
  LUA_NEW_MODULEMETHOD(L, lunum, log10);

  LUA_NEW_MODULEMETHOD(L, lunum, conjugate);

  LUA_NEW_MODULEMETHOD(L, lunum, slice);


  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_BOOL   , bool);
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

  lua_setglobal(L, "lunum");
#include "array_class.lc" // sets the lunum.__array_methods table

  return 0;
}




// *****************************************************************************
// Implementation of lunum.array metatable
//
// *****************************************************************************
int luaC_array__gc(lua_State *L)
{
  struct Array *A = lunum_checkarray1(L, 1);
  array_del(A);
  return 0;
}

int luaC_array__tostring(lua_State *L)
{
  struct Array *A = lunum_checkarray1(L, 1);

  lua_pushstring(L, "  [ ");
  int nstr = 1;
  for (int n=0; n<A->size; ++n) {

    char s[64];

    switch (A->dtype) {
    case ARRAY_TYPE_BOOL    : sprintf(s, "%s" , ((Bool*)A->data)[n]?"true":"false"); break;
    case ARRAY_TYPE_CHAR    : sprintf(s, "%d" , ((char   *)A->data)[n]); break;
    case ARRAY_TYPE_SHORT   : sprintf(s, "%d" , ((short  *)A->data)[n]); break;
    case ARRAY_TYPE_INT     : sprintf(s, "%d" , ((int    *)A->data)[n]); break;
    case ARRAY_TYPE_LONG    : sprintf(s, "%ld", ((long   *)A->data)[n]); break;
    case ARRAY_TYPE_FLOAT   : sprintf(s, "%g" , ((float  *)A->data)[n]); break;
    case ARRAY_TYPE_DOUBLE  : sprintf(s, "%g" , ((double *)A->data)[n]); break;
    case ARRAY_TYPE_COMPLEX : sprintf(s, "%g%s%gj",
                                      creal(((Complex*)A->data)[n]),
                                      cimag(((Complex*)A->data)[n]) >= 0.0 ? "+" : "-",
                                      fabs(cimag(((Complex*)A->data)[n]))); break;
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

int luaC_array__call(lua_State *L)
{
  struct Array *A = lunum_checkarray1(L, 1);
  int nind = lua_gettop(L) - 1;

  if (nind != A->ndims) {
    luaL_error(L, "wrong number of indices (%d) for array of dimension %d",
               nind, A->ndims);
    return 0;
  }
  const int Nd = A->ndims;
  int *stride = (int*) malloc(A->ndims * sizeof(int));
  stride[Nd-1] = 1;

  for (int d=Nd-2; d>=0; --d) {
    stride[d] = stride[d+1] * A->shape[d+1];
  }
  int m = 0;

  for (int d=0; d<A->ndims; ++d) {
    int i = lua_tointeger(L, d+2);
    m += i*stride[d];
  }

  luaL_getmetafield(L, 1, "__index");
  lua_pushvalue(L, 1);
  lua_pushnumber(L, m);

  lua_call(L, 2, 1);
  free(stride);

  return 1;
}

int luaC_array__index(lua_State *L)
{
  struct Array *A = lunum_checkarray1(L, 1);
  const int m = _get_index(L, A);

  switch (A->dtype) {
  case ARRAY_TYPE_BOOL    : lua_pushboolean(L,    ((Bool   *)A->data)[m]); break;
  case ARRAY_TYPE_CHAR    : lua_pushnumber (L,    ((char   *)A->data)[m]); break;
  case ARRAY_TYPE_SHORT   : lua_pushnumber (L,    ((short  *)A->data)[m]); break;
  case ARRAY_TYPE_INT     : lua_pushnumber (L,    ((int    *)A->data)[m]); break;
  case ARRAY_TYPE_LONG    : lua_pushnumber (L,    ((long   *)A->data)[m]); break;
  case ARRAY_TYPE_FLOAT   : lua_pushnumber (L,    ((float  *)A->data)[m]); break;
  case ARRAY_TYPE_DOUBLE  : lua_pushnumber (L,    ((double *)A->data)[m]); break;
  case ARRAY_TYPE_COMPLEX : lunum_pushcomplex (L, ((Complex*)A->data)[m]); break;
  }

  return 1;
}


int luaC_array__newindex(lua_State *L)
{
  struct Array *A = lunum_checkarray1(L, 1);
  const int m = _get_index(L, A);

  const enum ArrayType T = A->dtype;

  void *val = lunum_tovalue(L, T);
  memcpy((char*)A->data + array_sizeof(T)*m, val, array_sizeof(T));
  free(val);

  return 0;
}

int luaC_array__add(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_ADD); }
int luaC_array__sub(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_SUB); }
int luaC_array__mul(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_MUL); }
int luaC_array__div(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_DIV); }
int luaC_array__pow(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_POW); }
int luaC_array__unm(lua_State *L) { _unary_func(L, runm, cunm, 0); return 1; }


int _array_binary_op1(lua_State *L, enum ArrayOperation op)
{
  if (!lunum_hasmetatable(L, 1, "array")) {
    struct Array *B = lunum_checkarray1(L, 2);
    lunum_upcast(L, 1, B->dtype, B->size);
    lua_replace(L, 1);
    struct Array *A = lunum_checkarray1(L, 1);
    array_resize(A, B->shape, B->ndims);
  }
  if (!lunum_hasmetatable(L, 2, "array")) {
    struct Array *A = lunum_checkarray1(L, 1);
    lunum_upcast(L, 2, A->dtype, A->size);
    lua_replace(L, 2);
    struct Array *B = lunum_checkarray1(L, 2);
    array_resize(B, A->shape, A->ndims);
  }
  return _array_binary_op2(L, op);
}

int _array_binary_op2(lua_State *L, enum ArrayOperation op)
{
  struct Array *A = lunum_checkarray1(L, 1);
  struct Array *B = lunum_checkarray1(L, 2);


  if (A->ndims != B->ndims) {
    luaL_error(L, "arrays have different dimensions");
  }
  for (int d=0; d<A->ndims; ++d) {
    if (A->shape[d] != B->shape[d]) {
      luaL_error(L, "arrays shapes do not agree");
    }
  }


  const int N = A->size;
  enum ArrayType T = (A->dtype >= B->dtype) ? A->dtype : B->dtype;

  struct Array A_ = (A->dtype == T) ? *A : array_new_copy(A, T);
  struct Array B_ = (B->dtype == T) ? *B : array_new_copy(B, T);

  struct Array C = array_new_zeros(N, T);
  array_resize(&C, A->shape, A->ndims);
  lunum_pusharray1(L, &C);

  array_binary_op(&A_, &B_, &C, op);

  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);

  if (A->dtype != T) array_del(&A_);
  if (B->dtype != T) array_del(&B_);

  return 1;
}



// *****************************************************************************
// Implementation of lunum.complex metatable
//
// *****************************************************************************
int luaC_complex__tostring(lua_State *L)
{
  Complex z = *((Complex*) luaL_checkudata(L, 1, "complex"));

  lua_pushfstring(L, "%f%s%fj", creal(z), cimag(z)>=0.0?"+":"-", fabs(cimag(z)));
  return 1;
}
int luaC_complex__add(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_ADD); }
int luaC_complex__sub(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_SUB); }
int luaC_complex__mul(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_MUL); }
int luaC_complex__div(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_DIV); }
int luaC_complex__pow(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_POW); }
int luaC_complex__unm(lua_State *L) { _unary_func(L, runm, cunm, 0); return 1; }


// -----------------------------------------------------------------------------
// Use a dictionary ordering on the complex numbers. Might not be useful too
// often, but it's better than having this behavior undefined.
// -----------------------------------------------------------------------------
#define LUA_COMPARISON(comp)				\
  {							\
    Complex z1 = lunum_checkcomplex(L, 1);		\
    Complex z2 = lunum_checkcomplex(L, 2);		\
							\
    if (creal(z1) != creal(z2)) {			\
      lua_pushboolean(L, creal(z1) comp creal(z2));	\
    }							\
    else {						\
      lua_pushboolean(L, cimag(z1) comp cimag(z2));	\
    }							\
    return 1;						\
  }							\


int luaC_complex__lt(lua_State *L) LUA_COMPARISON(<);
int luaC_complex__le(lua_State *L) LUA_COMPARISON(<=);
int luaC_complex__eq(lua_State *L)
{
  Complex z1 = lunum_checkcomplex(L, 1);
  Complex z2 = lunum_checkcomplex(L, 2);
  lua_pushboolean(L, z1==z2);
  return 1;
}


int _complex_binary_op1(lua_State *L, enum ArrayOperation op)
{
  if (lunum_hasmetatable(L, 1, "array") ||
      lunum_hasmetatable(L, 2, "array")) {
    return _array_binary_op1(L, op);
  }
  if (!lunum_hasmetatable(L, 1, "complex")) {
    lunum_pushcomplex(L, lua_tonumber(L, 1));
    lua_replace(L, 1);
  }
  if (!lunum_hasmetatable(L, 2, "complex")) {
    lunum_pushcomplex(L, lua_tonumber(L, 2));
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








int luaC_lunum_array(lua_State *L)
{
  const enum ArrayType T = (enum ArrayType) luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);
  lunum_upcast(L, 1, T, 1);
  return 1;
}

int luaC_lunum_zeros(lua_State *L)
{
  if (lua_isnumber(L, 1)) {
    const int N = luaL_checkinteger(L, 1);
    const enum ArrayType T = (enum ArrayType) luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);
    struct Array A = array_new_zeros(N, T);
    lunum_pusharray1(L, &A);
    return 1;
  }
  else if (lua_istable(L, 1) || lunum_hasmetatable(L, 1, "array")) {

    int Nd;
    int *N = (int*) lunum_checkarray2(L, 1, ARRAY_TYPE_INT, &Nd);
    const enum ArrayType T = (enum ArrayType) luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);

    int ntot = 1;
    for (int d=0; d<Nd; ++d) ntot *= N[d];
    struct Array A = array_new_zeros(ntot, T);

    array_resize(&A, N, Nd);
    lunum_pusharray1(L, &A);

    return 1;
  }
  else {
    luaL_error(L, "argument must be either number, table, or array");
    return 0;
  }
}

int luaC_lunum_range(lua_State *L)
{
  const int N = luaL_checkinteger(L, 1);
  struct Array A = array_new_zeros(N, ARRAY_TYPE_INT);
  lunum_pusharray1(L, &A);
  for (int i=0; i<N; ++i) {
    ((int*)A.data)[i] = i;
  }
  return 1;
}

int luaC_lunum_resize(lua_State *L)
{
  int Nd;
  struct Array *A = lunum_checkarray1(L, 1); // the array to resize
  int *N = (int*) lunum_checkarray2(L, 2, ARRAY_TYPE_INT, &Nd);

  int ntot = 1;
  for (int d=0; d<Nd; ++d) ntot *= N[d];

  if (A->size != ntot) {
    luaL_error(L, "new and old total sizes do not agree");
    return 0;
  }
  array_resize(A, N, Nd);

  return 0;
}

int luaC_lunum_slice(lua_State *L)
{
  int Nd0, Nd1, Nd2;
  const struct Array *A = lunum_checkarray1(L, 1); // the array to resize
  int *start  = (int*) lunum_checkarray2(L, 2, ARRAY_TYPE_INT, &Nd0);
  int *size   = (int*) lunum_checkarray2(L, 3, ARRAY_TYPE_INT, &Nd1);
  int *stride = (int*) lunum_checkarray2(L, 4, ARRAY_TYPE_INT, &Nd2);

  if (Nd0 != A->ndims || Nd1 != A->ndims || Nd2 != A->ndims) {
    luaL_error(L, "slice has wrong number of dimensions for array");
  }

  int ntot = 1;
  for (int d=0; d<A->ndims; ++d) {

    ntot *= size[d];

    if (start[d] + (size[d]-1)*stride[d] >= A->shape[d]) {
      luaL_error(L, "slice not within array extent");
    }
  }

  struct Array B = array_new_zeros(ntot, A->dtype);
  array_extract_slice(&B, A, start, size, stride, Nd0);

  lunum_pusharray1(L, &B);

  return 1;
}



int luaC_lunum_sin(lua_State *L) { _unary_func(L, sin, csin, 1); return 1; }
int luaC_lunum_cos(lua_State *L) { _unary_func(L, cos, ccos, 1); return 1; }
int luaC_lunum_tan(lua_State *L) { _unary_func(L, tan, ctan, 1); return 1; }

int luaC_lunum_asin(lua_State *L) { _unary_func(L, asin, casin, 1); return 1; }
int luaC_lunum_acos(lua_State *L) { _unary_func(L, acos, cacos, 1); return 1; }
int luaC_lunum_atan(lua_State *L) { _unary_func(L, atan, catan, 1); return 1; }

int luaC_lunum_sinh(lua_State *L) { _unary_func(L, sinh, csinh, 1); return 1; }
int luaC_lunum_cosh(lua_State *L) { _unary_func(L, cosh, ccosh, 1); return 1; }
int luaC_lunum_tanh(lua_State *L) { _unary_func(L, tanh, ctanh, 1); return 1; }

int luaC_lunum_asinh(lua_State *L) { _unary_func(L, asinh, casinh, 1); return 1; }
int luaC_lunum_acosh(lua_State *L) { _unary_func(L, acosh, cacosh, 1); return 1; }
int luaC_lunum_atanh(lua_State *L) { _unary_func(L, atanh, catanh, 1); return 1; }

int luaC_lunum_exp(lua_State *L) { _unary_func(L, exp, cexp, 1); return 1; }
int luaC_lunum_log(lua_State *L) { _unary_func(L, log, clog, 1); return 1; }
int luaC_lunum_log10(lua_State *L) { _unary_func(L, log10, NULL, 1); return 1; }
int luaC_lunum_conjugate(lua_State *L) { _unary_func(L, rconj, conj, 0); return 1; }

#define EXPR_EVALF(T,N,x) {for(int i=0;i<N;++i)((T*)(x))[i]=f(((T*)(x))[i]);}
#define EXPR_EVALG(T,N,x) {for(int i=0;i<N;++i)((T*)(x))[i]=g(((T*)(x))[i]);}

void _unary_func(lua_State *L, double(*f)(double), Complex(*g)(Complex), int cast)
{
  if (lua_isnumber(L, 1)) {
    const double x = lua_tonumber(L, 1);
    lua_pushnumber(L, f(x));
  }
  else if (lunum_hasmetatable(L, 1, "complex")) {

    if (g == NULL) {
      luaL_error(L, "complex operation not supported");
    }

    const Complex z = lunum_checkcomplex(L, 1);
    lunum_pushcomplex(L, g(z));
  }
  else if (lunum_hasmetatable(L, 1, "array")) {
    struct Array *A = (struct Array*) lunum_checkarray1(L, 1);

    if (cast == 0) {
      struct Array B = array_new_copy(A, A->dtype);

      switch (B.dtype) {
      case ARRAY_TYPE_BOOL    : EXPR_EVALF(Bool   , B.size, B.data); break;
      case ARRAY_TYPE_CHAR    : EXPR_EVALF(char   , B.size, B.data); break;
      case ARRAY_TYPE_SHORT   : EXPR_EVALF(short  , B.size, B.data); break;
      case ARRAY_TYPE_INT     : EXPR_EVALF(long   , B.size, B.data); break;
      case ARRAY_TYPE_LONG    : EXPR_EVALF(int    , B.size, B.data); break;
      case ARRAY_TYPE_FLOAT   : EXPR_EVALF(float  , B.size, B.data); break;
      case ARRAY_TYPE_DOUBLE  : EXPR_EVALF(double , B.size, B.data); break;
      case ARRAY_TYPE_COMPLEX : EXPR_EVALG(Complex, B.size, B.data); break;
      }

      lunum_pusharray1(L, &B);
    }
    else if (A->dtype <= ARRAY_TYPE_DOUBLE) {
      struct Array B = array_new_copy(A, ARRAY_TYPE_DOUBLE);
      double *b = (double*) B.data;
      for (int i=0; i<B.size; ++i) b[i] = f(b[i]);
      lunum_pusharray1(L, &B);
    }
    else if (A->dtype == ARRAY_TYPE_COMPLEX) {

      if (g == NULL) {
        luaL_error(L, "complex operation not supported");
      }

      struct Array B = array_new_copy(A, ARRAY_TYPE_COMPLEX);
      Complex *b = (Complex*) B.data;
      for (int i=0; i<B.size; ++i) b[i] = g(b[i]);
      lunum_pusharray1(L, &B);
    }
  }
}

int _get_index(lua_State *L, struct Array *A)
{
  int m = 0;

  if (lua_isnumber(L, 2)) {
    m = luaL_checkinteger(L, 2);

    if (m >= A->size || m < 0) {
      luaL_error(L, "index %d out of bounds on array of length %d", m, A->size);
    }
  }
  else if (lua_istable(L, 2)) {
    int Nd;
    int *ind = (int*) lunum_checkarray2(L, 2, ARRAY_TYPE_INT, &Nd);

    if (A->ndims != Nd) {
      luaL_error(L, "wrong number of indices (%d) on array of dimension %d",
                 Nd, A->ndims);
    }
    int *stride = (int*) malloc(A->ndims * sizeof(int));
    stride[Nd-1] = 1;

    for (int d=Nd-2; d>=0; --d) {
      stride[d] = stride[d+1] * A->shape[d+1];
    }

    for (int d=0; d<A->ndims; ++d) {
      if (ind[d] >= A->shape[d] || ind[d] < 0) {
        luaL_error(L, "array indexed out of bounds (%d) on dimension %d of size %d",
                   ind[d], d, A->shape[d]);
      }
      m += ind[d]*stride[d];
    }
    free(stride);
  }
  return m;
}
