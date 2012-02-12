

require 'lunum'

local A = lunum.range(50)
local B = A:reshape{10,5}

print(A[':'])
print(B['0:2,0:2']:shape('array'))
print(B[':,:']:shape('array'))
print(B['2:,:']:shape('array'))

print(lunum.slice(A,0,10,1))
print(lunum.slice(A,0,10,2))
print(lunum.slice(A,0,10,3))
print(lunum.slice(A,0,10,4))
