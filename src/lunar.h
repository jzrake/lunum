

#ifndef __NumluaCapi_HEADER__
#define __NumluaCapi_HEADER__

#include "numarray.h"
#include "lualib.h"

int luaopen_lunar(lua_State *L);

struct Array *lunar_checkarray1(lua_State *L, int pos);
void         *lunar_checkarray2(lua_State *L, int pos, enum ArrayType T, int *N);
void          lunar_pusharray1(lua_State *L, struct Array *B);
void          lunar_pusharray2(lua_State *L, void *data, enum ArrayType T, int N);
void          lunar_astable(lua_State *L, int pos);
int           lunar_upcast(lua_State *L, int pos, enum ArrayType T, int N);
int           lunar_hasmetatable(lua_State *L, int pos, const char *name);
void         *lunar_tovalue(lua_State *L, enum ArrayType T);

#ifndef LUNAR_API_NOCOMPLEX
#include <complex.h>
typedef double complex Complex;
Complex       lunar_checkcomplex(lua_State *L, int n);
void          lunar_pushcomplex(lua_State *L, Complex z);
#endif // LUNAR_API_COMPLEX

#ifdef LUNAR_PRIVATE_API
void         _lunar_register_array(lua_State *L, struct Array *B);
#endif // LUNAR_PRIVATE_API

#endif // __NumluaCapi_HEADER__
