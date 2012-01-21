

#ifndef __NumluaCapi_HEADER__
#define __NumluaCapi_HEADER__

#include "numarray.h"
#include "lualib.h"

struct Array *lunar_checkarray1(lua_State *L, int pos);
void         *lunar_checkarray2(lua_State *L, int pos, enum ArrayType T, size_t *N);
void          lunar_pusharray1(lua_State *L, struct Array *A);
void          lunar_pusharray2(lua_State *L, void *data, enum ArrayType T, size_t N);
int           lunar_upcast(lua_State *L, int pos, enum ArrayType T, size_t N);
int           lunar_hasmetatable(lua_State *L, int pos, const char *name);
void         *lunar_tovalue(lua_State *L, enum ArrayType T);
Complex       lunar_checkcomplex(lua_State *L, int n);
void          lunar_pushcomplex(lua_State *L, Complex z);

#endif // __NumluaCapi_HEADER__
