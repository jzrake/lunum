

require 'lunum'

local A = lunum.loadtxt("test/ascii.dat")
print("[ 33, 9 ] ?= ", A:shape('array'))

local B = lunum.range(4):astype(lunum.double)
B:tofile("A.bin")
local C = lunum.fromfile("A.bin", lunum.double)
print("[ true, true, true, true ] ?= ", B:eq(C))

