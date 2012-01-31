
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lualib.h"
#include "lauxlib.h"
#include "lunar.h"



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



static int luaC_lunar_array(lua_State *L);
static int luaC_lunar_zeros(lua_State *L);
static int luaC_lunar_range(lua_State *L);
static int luaC_lunar_resize(lua_State *L);


static int luaC_lunar_sin(lua_State *L);
static int luaC_lunar_cos(lua_State *L);
static int luaC_lunar_tan(lua_State *L);

static int luaC_lunar_sin(lua_State *L);
static int luaC_lunar_cos(lua_State *L);
static int luaC_lunar_tan(lua_State *L);

static int luaC_lunar_asin(lua_State *L);
static int luaC_lunar_acos(lua_State *L);
static int luaC_lunar_atan(lua_State *L);

static int luaC_lunar_sinh(lua_State *L);
static int luaC_lunar_cosh(lua_State *L);
static int luaC_lunar_tanh(lua_State *L);

static int luaC_lunar_asinh(lua_State *L);
static int luaC_lunar_acosh(lua_State *L);
static int luaC_lunar_atanh(lua_State *L);

static int luaC_lunar_exp(lua_State *L);
static int luaC_lunar_log(lua_State *L);
static int luaC_lunar_log10(lua_State *L);
static int luaC_lunar_conjugate(lua_State *L);



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





int luaopen_lunar(lua_State *L)
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
  lua_pop(L, 1);


  // Create the 'lunar' table
  // ---------------------------------------------------------------------------
  lua_newtable(L);
  LUA_NEW_MODULEMETHOD(L, lunar, array);
  LUA_NEW_MODULEMETHOD(L, lunar, zeros);
  LUA_NEW_MODULEMETHOD(L, lunar, range);
  LUA_NEW_MODULEMETHOD(L, lunar, resize);

  LUA_NEW_MODULEMETHOD(L, lunar, sin);
  LUA_NEW_MODULEMETHOD(L, lunar, cos);
  LUA_NEW_MODULEMETHOD(L, lunar, tan);

  LUA_NEW_MODULEMETHOD(L, lunar, asin);
  LUA_NEW_MODULEMETHOD(L, lunar, acos);
  LUA_NEW_MODULEMETHOD(L, lunar, atan);

  LUA_NEW_MODULEMETHOD(L, lunar, sinh);
  LUA_NEW_MODULEMETHOD(L, lunar, cosh);
  LUA_NEW_MODULEMETHOD(L, lunar, tanh);

  LUA_NEW_MODULEMETHOD(L, lunar, asinh);
  LUA_NEW_MODULEMETHOD(L, lunar, acosh);
  LUA_NEW_MODULEMETHOD(L, lunar, atanh);

  LUA_NEW_MODULEMETHOD(L, lunar, exp);
  LUA_NEW_MODULEMETHOD(L, lunar, log);
  LUA_NEW_MODULEMETHOD(L, lunar, log10);

  LUA_NEW_MODULEMETHOD(L, lunar, conjugate);


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

  lua_setglobal(L, "lunar");
#include "array_class.lc" // sets the lunar.__array_methods table

  return 0;
}




// *****************************************************************************
// Implementation of lunar.array metatable
//
// *****************************************************************************
int luaC_array__gc(lua_State *L)
{
  struct Array *A = lunar_checkarray1(L, 1);
  array_del(A);
  return 0;
}

int luaC_array__tostring(lua_State *L)
{
  struct Array *A = lunar_checkarray1(L, 1);

  lua_pushstring(L, "  [ ");
  int nstr = 1;
  for (int n=0; n<A->size; ++n) {

    char s[64];

    switch (A->dtype) {
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
  struct Array *A = lunar_checkarray1(L, 1);
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
  struct Array *A = lunar_checkarray1(L, 1);
  const int m = _get_index(L, A);

  switch (A->dtype) {
  case ARRAY_TYPE_CHAR    : lua_pushnumber(L,    ((char   *)A->data)[m]); break;
  case ARRAY_TYPE_SHORT   : lua_pushnumber(L,    ((short  *)A->data)[m]); break;
  case ARRAY_TYPE_INT     : lua_pushnumber(L,    ((int    *)A->data)[m]); break;
  case ARRAY_TYPE_LONG    : lua_pushnumber(L,    ((long   *)A->data)[m]); break;
  case ARRAY_TYPE_FLOAT   : lua_pushnumber(L,    ((float  *)A->data)[m]); break;
  case ARRAY_TYPE_DOUBLE  : lua_pushnumber(L,    ((double *)A->data)[m]); break;
  case ARRAY_TYPE_COMPLEX : lunar_pushcomplex(L, ((Complex*)A->data)[m]); break;
  }

  return 1;
}


int luaC_array__newindex(lua_State *L)
{
  struct Array *A = lunar_checkarray1(L, 1);
  const int m = _get_index(L, A);

  const enum ArrayType T = A->dtype;

  void *val = lunar_tovalue(L, T);
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
  if (!lunar_hasmetatable(L, 1, "array")) {
    struct Array *B = lunar_checkarray1(L, 2);
    lunar_upcast(L, 1, B->dtype, B->size);
    lua_replace(L, 1);
  }
  if (!lunar_hasmetatable(L, 2, "array")) {
    struct Array *A = lunar_checkarray1(L, 1);
    lunar_upcast(L, 2, A->dtype, A->size);
    lua_replace(L, 2);
  }
  return _array_binary_op2(L, op);
}

int _array_binary_op2(lua_State *L, enum ArrayOperation op)
{
  struct Array *A = lunar_checkarray1(L, 1);
  struct Array *B = lunar_checkarray1(L, 2);

  if (A->size != B->size) {
    luaL_error(L, "arrays could not be broadcast together with shapes (%d) (%d)",
               A->size, B->size);
  }
  const int N = A->size;
  enum ArrayType T = (A->dtype >= B->dtype) ? A->dtype : B->dtype;

  struct Array A_ = (A->dtype == T) ? *A : array_new_copy(A, T);
  struct Array B_ = (B->dtype == T) ? *B : array_new_copy(B, T);

  struct Array C = array_new_zeros(N, T);
  lunar_pusharray1(L, &C);

  array_binary_op(&A_, &B_, &C, op);

  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);

  if (A->dtype != T) array_del(&A_);
  if (B->dtype != T) array_del(&B_);

  return 1;
}



// *****************************************************************************
// Implementation of lunar.complex metatable
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


int _complex_binary_op1(lua_State *L, enum ArrayOperation op)
{
  if (lunar_hasmetatable(L, 1, "array") ||
      lunar_hasmetatable(L, 2, "array")) {
    return _array_binary_op1(L, op);
  }
  if (!lunar_hasmetatable(L, 1, "complex")) {
    lunar_pushcomplex(L, lua_tonumber(L, 1));
    lua_replace(L, 1);
  }
  if (!lunar_hasmetatable(L, 2, "complex")) {
    lunar_pushcomplex(L, lua_tonumber(L, 2));
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








int luaC_lunar_array(lua_State *L)
{
  const enum ArrayType T = (enum ArrayType) luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);
  lunar_upcast(L, 1, T, 1);
  return 1;
}

int luaC_lunar_zeros(lua_State *L)
{
  if (lua_isnumber(L, 1)) {
    const int N = luaL_checkinteger(L, 1);
    const enum ArrayType T = (enum ArrayType) luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);
    struct Array A = array_new_zeros(N, T);
    lunar_pusharray1(L, &A);
    return 1;
  }
  else if (lua_istable(L, 1) || lunar_hasmetatable(L, 1, "array")) {

    int Nd;
    int *N = (int*) lunar_checkarray2(L, 1, ARRAY_TYPE_INT, &Nd);
    const enum ArrayType T = (enum ArrayType) luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);

    int ntot = 1;
    for (int d=0; d<Nd; ++d) ntot *= N[d];
    struct Array A = array_new_zeros(ntot, T);

    array_resize(&A, N, Nd);
    lunar_pusharray1(L, &A);

    return 1;
  }
  else {
    luaL_error(L, "argument must be either number, table, or array");
    return 0;
  }
}

int luaC_lunar_range(lua_State *L)
{
  const int N = luaL_checkinteger(L, 1);
  struct Array A = array_new_zeros(N, ARRAY_TYPE_INT);
  lunar_pusharray1(L, &A);
  for (int i=0; i<N; ++i) {
    ((int*)A.data)[i] = i;
  }
  return 1;
}

int luaC_lunar_resize(lua_State *L)
{
  int Nd;
  struct Array *A = lunar_checkarray1(L, 1); // the array to resize
  int *N = (int*) lunar_checkarray2(L, 2, ARRAY_TYPE_INT, &Nd);

  int ntot = 1;
  for (int d=0; d<Nd; ++d) ntot *= N[d];

  if (A->size != ntot) {
    luaL_error(L, "new and old total sizes do not agree");
    return 0;
  }
  array_resize(A, N, Nd);

  return 0;
}



int luaC_lunar_sin(lua_State *L) { _unary_func(L, sin, csin, 1); return 1; }
int luaC_lunar_cos(lua_State *L) { _unary_func(L, cos, ccos, 1); return 1; }
int luaC_lunar_tan(lua_State *L) { _unary_func(L, tan, ctan, 1); return 1; }

int luaC_lunar_asin(lua_State *L) { _unary_func(L, asin, casin, 1); return 1; }
int luaC_lunar_acos(lua_State *L) { _unary_func(L, acos, cacos, 1); return 1; }
int luaC_lunar_atan(lua_State *L) { _unary_func(L, atan, catan, 1); return 1; }

int luaC_lunar_sinh(lua_State *L) { _unary_func(L, sinh, csinh, 1); return 1; }
int luaC_lunar_cosh(lua_State *L) { _unary_func(L, cosh, ccosh, 1); return 1; }
int luaC_lunar_tanh(lua_State *L) { _unary_func(L, tanh, ctanh, 1); return 1; }

int luaC_lunar_asinh(lua_State *L) { _unary_func(L, asinh, casinh, 1); return 1; }
int luaC_lunar_acosh(lua_State *L) { _unary_func(L, acosh, cacosh, 1); return 1; }
int luaC_lunar_atanh(lua_State *L) { _unary_func(L, atanh, catanh, 1); return 1; }

int luaC_lunar_exp(lua_State *L) { _unary_func(L, exp, cexp, 1); return 1; }
int luaC_lunar_log(lua_State *L) { _unary_func(L, log, clog, 1); return 1; }
int luaC_lunar_log10(lua_State *L) { _unary_func(L, log10, NULL, 1); return 1; }
int luaC_lunar_conjugate(lua_State *L) { _unary_func(L, rconj, conj, 0); return 1; }

#define EXPR_EVALF(T,N,x) {for(int i=0;i<N;++i)((T*)(x))[i]=f(((T*)(x))[i]);}
#define EXPR_EVALG(T,N,x) {for(int i=0;i<N;++i)((T*)(x))[i]=g(((T*)(x))[i]);}

void _unary_func(lua_State *L, double(*f)(double), Complex(*g)(Complex), int cast)
{
  if (lua_isnumber(L, 1)) {
    const double x = lua_tonumber(L, 1);
    lua_pushnumber(L, f(x));
  }
  else if (lunar_hasmetatable(L, 1, "complex")) {

    if (g == NULL) {
      luaL_error(L, "complex operation not supported");
    }

    const Complex z = lunar_checkcomplex(L, 1);
    lunar_pushcomplex(L, g(z));
  }
  else if (lunar_hasmetatable(L, 1, "array")) {
    struct Array *A = (struct Array*) lunar_checkarray1(L, 1);

    if (cast == 0) {
      struct Array B = array_new_copy(A, A->dtype);

      switch (B.dtype) {
      case ARRAY_TYPE_CHAR    : EXPR_EVALF(char   , B.size, B.data); break;
      case ARRAY_TYPE_SHORT   : EXPR_EVALF(short  , B.size, B.data); break;
      case ARRAY_TYPE_INT     : EXPR_EVALF(long   , B.size, B.data); break;
      case ARRAY_TYPE_LONG    : EXPR_EVALF(int    , B.size, B.data); break;
      case ARRAY_TYPE_FLOAT   : EXPR_EVALF(float  , B.size, B.data); break;
      case ARRAY_TYPE_DOUBLE  : EXPR_EVALF(double , B.size, B.data); break;
      case ARRAY_TYPE_COMPLEX : EXPR_EVALG(Complex, B.size, B.data); break;
      }

      lunar_pusharray1(L, &B);
    }
    else if (A->dtype <= ARRAY_TYPE_DOUBLE) {
      struct Array B = array_new_copy(A, ARRAY_TYPE_DOUBLE);
      double *b = (double*) B.data;
      for (int i=0; i<B.size; ++i) b[i] = f(b[i]);
      lunar_pusharray1(L, &B);
    }
    else if (A->dtype == ARRAY_TYPE_COMPLEX) {

      if (g == NULL) {
        luaL_error(L, "complex operation not supported");
      }

      struct Array B = array_new_copy(A, ARRAY_TYPE_COMPLEX);
      Complex *b = (Complex*) B.data;
      for (int i=0; i<B.size; ++i) b[i] = g(b[i]);
      lunar_pusharray1(L, &B);
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
    int *ind = (int*) lunar_checkarray2(L, 2, ARRAY_TYPE_INT, &Nd);

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
