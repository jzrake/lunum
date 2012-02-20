

require 'lunum'

local A = lunum.range(200)
local B = A:reshape{10,10,2}

if true then
   print("testing manual slicing function")
   print(lunum.slice(A,0,10,1,0))
   print(lunum.slice(A,0,10,2,0))
   print(lunum.slice(A,0,10,3,0))
   print(lunum.slice(A,0,10,4,0))

   print("testing squeeze...")
   print("[ 5, 5, 1 ] ?= ", lunum.slice(B, {0,0,0}, {10,10,2}, {2,2,2}, {0,0,0}):shape('array'))
   print("[ 5, 5 ]    ?= ", lunum.slice(B, {0,0,0}, {10,10,2}, {2,2,2}, {0,0,1}):shape('array'))

   print("testing slicing with tables...")
   print("[ 0, 4 ]    ?= ", A[{{0,8,4}}])
   print("[ 2, 2, 2 ] ?= ", B[{{0,10,5}, {0,10,5}, {0,2,1}}]:shape('array'))
   print("[ 2, 2 ]    ?= ", B[{{0,10,5}, {0,10,5}, 0}]:shape('array'))

   print("testing slicing with strings...")
   print("[ 0, 4 ]    ?= ", A['0:8:4'])
   print("[ 2, 2, 2 ] ?= ", B['0:10:5,0:10:5,0:2:1']:shape('array'))
   print("[ 2, 2 ]    ?= ", B['0:10:5,0:10:5, 0 '  ]:shape('array'))
end

print("         45 ?= ", B['2,2,1'])
print("         45 ?= ", B[{2,2,1}])
print("         45 ?= ", B( 2,2,1 ))


print(A:shape('array'))
print(A[(A+1):astype(lunum.bool)]:shape('array'))
print(A[(A+1):astype(lunum.bool)])

