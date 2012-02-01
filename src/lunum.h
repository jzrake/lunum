

#ifndef __NumluaCapi_HEADER__
#define __NumluaCapi_HEADER__

#include "numarray.h"
#include "lualib.h"

typedef      unsigned char Bool;
int luaopen_lunum(lua_State *L);

struct Array *lunum_checkarray1(lua_State *L, int pos);
void         *lunum_checkarray2(lua_State *L, int pos, enum ArrayType T, int *N);
void          lunum_pusharray1(lua_State *L, struct Array *B);
void          lunum_pusharray2(lua_State *L, void *data, enum ArrayType T, int N);
void          lunum_astable(lua_State *L, int pos);
int           lunum_upcast(lua_State *L, int pos, enum ArrayType T, int N);
int           lunum_hasmetatable(lua_State *L, int pos, const char *name);
void         *lunum_tovalue(lua_State *L, enum ArrayType T);

#ifndef LUNUM_API_NOCOMPLEX
#include <complex.h>
typedef double complex Complex;
Complex       lunum_checkcomplex(lua_State *L, int n);
void          lunum_pushcomplex(lua_State *L, Complex z);
#endif // LUNUM_API_COMPLEX

#ifdef LUNUM_PRIVATE_API
void         _lunum_register_array(lua_State *L, struct Array *B);
#endif // LUNUM_PRIVATE_API

#endif // __NumluaCapi_HEADER__
