


local array = lunar.array


local A = upcast({1,2,3}, lunar.char)
local B = upcast(array({1,2,3}), lunar.double)
local C = upcast(array({1,2,3}, lunar.int), lunar.long)
local D = upcast({1,2,3}, lunar.complex)


print(lunar.dtype(A), A)
print(lunar.dtype(B), B)
print(lunar.dtype(C), C)
print(lunar.dtype(D), D)

print(A + {1,2,3})

