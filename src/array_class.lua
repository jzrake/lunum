

-- *****************************************************************************
-- Local utility function, never exported
-- *****************************************************************************
local function check_shapes_agree(A, B)
   local go = true
   local As = A:shape()
   local Bs = B:shape()

   if #As ~= #Bs then go = false end
   for d=1,#As do
      if As[d] ~= Bs[d] then go = false end
   end
   if not go then
      error('arguments of incompatible shapes.')
   end
end


local function copy(A)
   -- --------------------------------------------------------------------------
   -- Returns a deepcopy of the array 'A'.
   -- --------------------------------------------------------------------------
   local B = A:astype(A:dtype('enum'))
   return B
end

local function min(A)
   -- --------------------------------------------------------------------------
   -- Returns the minimum value of the array 'A'.
   -- --------------------------------------------------------------------------
   local x = A[0]
   for I in A:indices('table') do
      if A[I] < x then x = A[I] end
   end
   return x
end

local function max(A)
   -- --------------------------------------------------------------------------
   -- Returns the maximum value of the array 'A'.
   -- --------------------------------------------------------------------------
   local x = A[0]
   for I in A:indices('table') do
      if A[I] > x then x = A[I] end
   end
   return x
end

local function conj(A)
   -- --------------------------------------------------------------------------
   -- Returns the complex conjugate of the array 'A'.
   -- --------------------------------------------------------------------------
   return lunum.conjugate(A)
end

local function real(A)
   -- --------------------------------------------------------------------------
   -- Returns the real part of the array 'A'.
   -- --------------------------------------------------------------------------
   if A:dtype('enum') ~= lunum.complex then return A:copy()
   else
      return A:astype(lunum.double)
   end
end

local function imag(A)
   -- --------------------------------------------------------------------------
   -- Returns the imaginary part of the array 'A'.
   -- --------------------------------------------------------------------------
   if A:dtype('enum') ~= lunum.complex then
      return lunum.zeros(A:shape(), A:dtype('enum'))
   else
      return (-lunum.I*A):astype(lunum.double)
   end
end

local function resize(A, newshape)
   -- --------------------------------------------------------------------------
   -- Resizes the array 'A' in-place to have the shape 'newshape'.
   -- --------------------------------------------------------------------------
   lunum.resize(A, newshape)
   return B
end

local function reshape(A, newshape)
   -- --------------------------------------------------------------------------
   -- Returns a copy of 'A' with the shape 'newshape'. Similar to array:resize,
   -- but does not change 'A'.
   -- --------------------------------------------------------------------------
   local B = A:copy()
   B:resize(newshape)
   return B
end

local function setasflat(A)
   -- --------------------------------------------------------------------------
   -- Resizes 'A' in-place to a flattened version.
   -- --------------------------------------------------------------------------
   A:resize({A:size()})
end

local function indices(A, kind)
   -- --------------------------------------------------------------------------
   -- Returns an iterator over all permutations of valid indices. If the
   -- optional argument 'kind' is the string 'table', then returns a table
   -- containing the indices instead of unpacking them.
   -- --------------------------------------------------------------------------
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


-- -----------------------------------------------------------------------------
-- Array comparison operators.
-- -----------------------------------------------------------------------------
local function eq(A, B)
   check_shapes_agree(A, B)
   local C = lunum.zeros(A:shape(), lunum.bool)
   for I in A:indices('table') do C[I] = A[I]==B[I] end
   return C
end

local function ne(A, B)
   check_shapes_agree(A, B)
   local C = lunum.zeros(A:shape(), lunum.bool)
   for I in A:indices('table') do C[I] = A[I]~=B[I] end
   return C
end

local function lt(A, B)
   check_shapes_agree(A, B)
   local C = lunum.zeros(A:shape(), lunum.bool)
   for I in A:indices('table') do C[I] = A[I]< B[I] end
   return C
end

local function le(A, B)
   check_shapes_agree(A, B)
   local C = lunum.zeros(A:shape(), lunum.bool)
   for I in A:indices('table') do C[I] = A[I]<=B[I] end
   return C
end

local function gt(A, B)
   check_shapes_agree(A, B)
   local C = lunum.zeros(A:shape(), lunum.bool)
   for I in A:indices('table') do C[I] = A[I]> B[I] end
   return C
end

local function ge(A, B)
   check_shapes_agree(A, B)
   local C = lunum.zeros(A:shape(), lunum.bool)
   for I in A:indices('table') do C[I] = A[I]>=B[I] end
   return C
end


local function apply(f,...)
   -- --------------------------------------------------------------------------
   -- Returns the lunum array 'C', where C[i] = f(A[i], B[i], ...) for any
   -- number of array input arguments. Arguments must all have the same
   -- shape. The returned array has the highest data type of any of the inputs.
   -- --------------------------------------------------------------------------
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

   local B = lunum.zeros(s1, T)
   for i=0,B:size()-1 do
      local x = { }
      for d=1,#A do x[d] = A[d][i] end
      B[i] = f(unpack(x))
   end
   return B
end

local function __build_slice(A,s)

   slice = { }
--   print("checking slice...", #s)

   for k,v in pairs(s) do
      if type(v) == 'number' then
	 slice[k] = {v, A:shape()[k], 1}
      elseif type(v) == 'table' then
	 slice[k] = { v[1], v[2], v[3] }
      end
   end

   sliceT = { {},{},{} }

   for i=1,#slice do
      sliceT[1][i] = slice[i][1] or 0
      sliceT[2][i] = slice[i][2] or A:shape()[i]
      sliceT[3][i] = slice[i][3] or 1
   end

   return lunum.slice(A, unpack(sliceT))
end

-- -----------------------------------------------------------------------------
-- This function gets called from C code to register an array's class methods
-- have been were implemented in Lua.
-- -----------------------------------------------------------------------------
local function __register_array(t)
   t.copy      = copy
   t.min       = min
   t.max       = max
   t.real      = real
   t.imag      = imag
   t.conj      = conj
   t.resize    = resize
   t.setasflat = setasflat
   t.reshape   = reshape
   t.indices   = indices
   t.eq        = eq
   t.ne        = ne
   t.lt        = lt
   t.le        = le
   t.gt        = gt
   t.ge        = ge
end


-- -----------------------------------------------------------------------------
-- Registering the functions with the lunum table.
-- -----------------------------------------------------------------------------
lunum.__build_slice    = __build_slice
lunum.__register_array = __register_array
lunum.apply = apply

