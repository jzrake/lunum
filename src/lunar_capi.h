

#ifndef __NumluaCapi_HEADER__
#define __NumluaCapi_HEADER__

#include "numarray.h"
#include "lualib.h"

void *lunar_checkarray(lua_State *L, int pos, enum ArrayType T, size_t *N);
void  lunar_pusharray(lua_State *L, void *data, enum ArrayType T, size_t N);

#endif // __NumluaCapi_HEADER__
