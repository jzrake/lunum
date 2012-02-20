
require 'lunum'

local function test1()
   local A = lunum.zeros(100)

   for k,v in pairs(getmetatable(A)) do
      print (k,v)
   end

   for k,v in pairs(lunum) do
      print (k,v)
   end

   print(getmetatable(A).__len)
   print(getmetatable(A).__gc)
   print("length is", A:size())
end

local function test2()
   local A = lunum.array({0,1,2,3,4,5,6,7,8,9})
   A[4] = 4.5
   print(A[4])
end

local function test3()
   local A = lunum.array({0,1,2,3,4,5,6,7,8,9})
   local B = lunum.array({9,8,7,6,5,4,3,2,1,0})
   print("(A + B)[3] = ", (A + B)[2])
   print("(A - B)[3] = ", (A - B)[2])
   print("(A * B)[3] = ", (A * B)[2])
   print("(A / B)[3] = ", (A / B)[2])
end

local function test4()
   local A = lunum.array({0,1,2,3,4,5,6,7,8,9}, lunum.float)
   local B = lunum.array({0,1,2,3,4,5,6,7,8,9}, lunum.double)
   print("(A + B)[3] = ", (A + B)[2])
end

local function test5()
   print("[char]    A*10 = ", lunum.array({0,1,2,3,4,5,6,7,8,9}, lunum.char) * 10)
   print("[float]   A    = ", lunum.array({0,1,2,3,4,5,6,7,8,9}, lunum.float))
   print("[complex] A    = ", lunum.array({0,1,2,3,4,5,6,7,8,9}, lunum.complex))
end

local function test6()
   local I = lunum.I
   local A = lunum.zeros(109, lunum.complex)
   A[4] = 1
   A[5] = I
   print(A[4], A[5])
   print(A)
end

local function test7()
   local I = lunum.I
   local A = lunum.array({1,2,3})
   print(lunum.sin(A))
   print(lunum.sin(A + I))
   print(lunum.cos(A + I))
   print(lunum.log(A + I))
   print(lunum.atanh(A + I))
end

local function test8()
   local A = lunum.array({1,2,3,4,5,6,7,8})
   lunum.resize(A, {4,2})
   print(unpack(A:shape()))
   print("A(0,0) = ", A(0,0))
   print("A(0,1) = ", A(0,1))
   print("A(1,0) = ", A(1,0))
   print("A(1,1) = ", A(1,1))
   print("A(2,0) = ", A(2,0))
   print("A(2,1) = ", A(2,1))
   print("A(3,0) = ", A(3,0))
   print("A(3,1) = ", A(3,1))
end


local function test9()
   local B = lunum.array({1,2,3,4,5,6,7,8,9,10,11,12})
   lunum.resize(B, {2,1,3,2})

   for i,j,k,m in B:indices() do
      print(string.format("B(%d,%d,%d,%d) = %f", i,j,k,m, B(i,j,k,m)))
   end

   for I in B:indices('table') do
      print(string.format("B(%d,%d,%d,%d) = %f", I[1],I[2],I[3],I[4], B[I]))
   end
end


local function test10()
   print("testing apply function, and resize function")

   local B = lunum.range(100)
   local C = lunum.apply(function(x,y,z) return x+y+z end, B, B, B)

   local B = lunum.zeros({10,10})
   local C = lunum.zeros({10,10}, lunum.complex)

   B[{1,1}] = 1
   C[{1,1}] = lunum.I

   local D = lunum.apply(function(x,y) return x+y end, B, C)
   print("return array has type", D:dtype())

   local C = lunum.zeros({4,4}, lunum.complex)
   print(C)
   print(C:shape('array'))

   lunum.resize(C, {4,4})
   print(C:shape('array'))
end


local function test11()
   local C = lunum.array({0,1,true,false}, lunum.bool)
   print("{0,1,true,false} = ", C)

   local A = lunum.array({1,2,3})
   local B = lunum.array({3,2,1})
   print("A = ", A, "B = ", B)
   print("A == A ? ", A:eq(B))
   print("A ~= A ? ", A:ne(B))
   print("A <  A ? ", A:lt(B))
   print("A <= A ? ", A:le(B))
   print("A >  A ? ", A:gt(B))
   print("A >= A ? ", A:ge(B))
end

local function test12()
   print "testing type flags"
   local C = lunum.array({1,2,3}, 'd')
   print("double  ?= ", C:dtype())
   print("complex ?= ", C:astype('z'):dtype())
   print("[ 1+0j, 2+0j, 3+0j ] ?= ", C:astype('z'))
end

test1()
test2()

test3()
test4()
test5()
test6()
test7()
test8()
test9()
test10()

test11()
test12()
