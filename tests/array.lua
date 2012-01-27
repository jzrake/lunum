
require 'lunar'

local function test1()
   local A = lunar.zeros(100)

   for k,v in pairs(getmetatable(A)) do
      print (k,v)
   end

   for k,v in pairs(lunar) do
      print (k,v)
   end

   print(getmetatable(A).__len)
   print(getmetatable(A).__gc)
   print("length is", A:size())
end

local function test2()
   local A = lunar.array({0,1,2,3,4,5,6,7,8,9})
   A[4] = 4.5
   print(A[4])
end

local function test3()
   local A = lunar.array({0,1,2,3,4,5,6,7,8,9})
   local B = lunar.array({9,8,7,6,5,4,3,2,1,0})
   print("(A + B)[3] = ", (A + B)[2])
   print("(A - B)[3] = ", (A - B)[2])
   print("(A * B)[3] = ", (A * B)[2])
   print("(A / B)[3] = ", (A / B)[2])
end

local function test4()
   local A = lunar.array({0,1,2,3,4,5,6,7,8,9}, lunar.float)
   local B = lunar.array({0,1,2,3,4,5,6,7,8,9}, lunar.double)
   print("(A + B)[3] = ", (A + B)[2])
end

local function test5()
   print("[char]    A*10 = ", lunar.array({0,1,2,3,4,5,6,7,8,9}, lunar.char) * 10)
   print("[float]   A    = ", lunar.array({0,1,2,3,4,5,6,7,8,9}, lunar.float))
   print("[complex] A    = ", lunar.array({0,1,2,3,4,5,6,7,8,9}, lunar.complex))
end

local function test6()
   local I = lunar.I
   local A = lunar.zeros(109, lunar.complex)
   A[4] = 1
   A[5] = I
   print(A[4], A[5])
   print(A)
end

local function test7()
   local I = lunar.I
   local A = lunar.array({1,2,3})
   print(lunar.sin(A))
   print(lunar.sin(A + I))
   print(lunar.cos(A + I))
   print(lunar.log(A + I))
   print(lunar.atanh(A + I))
end

local function test8()
   local A = lunar.array({1,2,3,4,5,6,7,8})
   lunar.resize(A, {4,2})
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
   local B = lunar.array({1,2,3,4,5,6,7,8,9,10,11,12})
   lunar.resize(B, {2,1,3,2})

   for i,j,k,m in B:indices() do
      print(string.format("B(%d,%d,%d,%d) = %f", i,j,k,m, B(i,j,k,m)))
   end
end


local function test10()
   print("testing apply function, and resize function")

   local B = lunar.range(100)
   local C = lunar.apply(function(x,y,z) return x+y+z end, B, B, B)

   local B = lunar.zeros({10,10})
   local C = lunar.zeros({10,10}, lunar.complex)

   B[0] = 1
   C[0] = lunar.I

   local D = lunar.apply(function(x,y) return x+y end, B, C)
   print("return array has type", D:dtype(), D)

   local C = lunar.zeros({4,4}, lunar.complex)
   print(C)
   print(unpack(C:shape()))

   lunar.resize(C, {4,4})
   print(unpack(C:shape()))
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
