

local function indices(A, kind)
   local N =  A:shape()
   local I = { }

   for d=1,#N-1 do I[d] = 0 end
   I[#N] = -1

   local function f()
      I[#N] = I[#N] + 1
      for d=#N,2,-1 do
         if I[d] == N[d] then
            I[d] = 0
            I[d-1] = I[d-1] + 1
         end
      end
      if I[1] == N[1] then return nil end
      return unpack(I)
   end

   local function g()
      I[#N] = I[#N] + 1
      for d=#N,2,-1 do
         if I[d] == N[d] then
            I[d] = 0
            I[d-1] = I[d-1] + 1
         end
      end
      if I[1] == N[1] then return nil end
      return I
   end
   if kind == 'table' then return g
   else return f
   end
end

local function apply(f,...)
   local A  = {...}
   local T  = A[1]:dtype('enum')
   local s1 = A[1]:shape()
   local go = true

   for d=2,#A do
      local Td = A[d]:dtype('enum')
      if T < Td then T = Td end
      local sd = A[d]:shape()
      if #s1 ~= #sd then go = false end
      for m=1,#sd do if s1[m] ~= sd[m] then go = false end end
   end

   if not go then
      error('arguments of incompatible shapes.')
   end

   local B = lunar.zeros(s1, T)
   for i=0,B:size()-1 do
      local x = { }
      for d=1,#A do x[d] = A[d][i] end
      B[i] = f(unpack(x))
   end
   return B
end

lunar.__array_methods = { }
lunar.__array_methods.indices = indices
lunar.apply = apply
