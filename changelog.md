



# Version 0.5.3
--------------------------------------------------------------------------------

## Features

+ Slicing operations as r-values
+ Read ASCII tables from disk with `lunum.loadtxt`
+ Read binary tables from disk with `lunum.fromfile`
+ Write binary files with `array:tofile`


## Performance

+ There is now a small series of tests that tests performace against the same
  operations in Numpy. Results are that Numpy is faster by factors of between 1
  and 3. This will evenually be improved by reducing the number of `memcpy`
  operations.



# Version 0.5.2
--------------------------------------------------------------------------------

## Bug fixes
	
+ The C API fixes the lunum_upcast function, which was handling scalars assuming
  they were doubles


## Performance

+ The Lua library now owns the data buffers used by Lunum arrays. This makes
  garbage collection able to operate correctly, since Lua is aware of how much
  memory it is using


	
# Version 0.5.1
--------------------------------------------------------------------------------
First stable public release of Lunum
