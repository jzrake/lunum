


local array = lunum.array


local A = upcast({1,2,3}, lunum.char)
local B = upcast(array({1,2,3}), lunum.double)
local C = upcast(array({1,2,3}, lunum.int), lunum.long)
local D = upcast({1,2,3}, lunum.complex)


print(A:dtype(), A)
print(B:dtype(), B)
print(C:dtype(), C)
print(D:dtype(), D)

print(A + {1,2,3})

