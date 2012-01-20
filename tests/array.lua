
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
   print("length is", #A)
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


test1()
test2()

test3()
test4()
test5()
test6()

