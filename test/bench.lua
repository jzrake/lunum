

require 'lunum'


local function Test1(N)

   local start = os.clock()

   local A = lunum.zeros({N,N})
   for i=0,50 do
      local B = lunum.sin(A) + lunum.cos(A)
   end

   print(string.format("lunum: test took %f seconds", os.clock() - start))
end

collectgarbage("setpause", 50)
collectgarbage("setstepmul", 200)
Test1(512)


-- v0.5.1, no GC: 229
-- v0.5.1, w/ GC: 232

-- v0.5.2, no GC: 201
-- v0.5.2, w/ GC: 197

