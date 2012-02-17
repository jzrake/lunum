

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
