

local function indices(A)
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
   return f
end

local function apply(f,...)
   local A = {...}
   local B = lunar.zeros(A[1]:size(), A[1]:dtype('enum'))
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
