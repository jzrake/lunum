

require 'lunum'

local A = lunum.range(50)

print(A[{{0,10,2},{0,6},12}])
print(A)

print(lunum.slice(A,0,10,1))
print(lunum.slice(A,0,10,2))
print(lunum.slice(A,0,10,3))
print(lunum.slice(A,0,10,4))

