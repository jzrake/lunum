

require 'lunum'

local A = lunum.range(50)

print(A[{{2,10,2}}])
print(A)

print(lunum.slice(A,0,10,1))
print(lunum.slice(A,0,10,2))
print(lunum.slice(A,0,10,3))
print(lunum.slice(A,0,10,4))

