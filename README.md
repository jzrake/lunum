


# Introduction

Lunar ('lua' + 'array') is a numeric extension for the Lua programming
language. Its goals are to provide flexible and robust facilities for
the manipulation of multi-dimensional numeric arrays, using a syntax
and style that feels native to Lua, but also *guessable* to Numpy
users. All of the hard work is done in C, making it prefectly
acceptable for use in scientific software development.

Consistent with the Lua philosophy, Lunar is easy to embed in C
applications. This means that C applications may readily embed the Lua
interpereter along with the Lunar API, rather than relying on building
shared modules as in Python. This has important consequences,
especially for distributed memory parallel computing where the
embedding application may be using MPI. Thus far, there is no way of
(simply) accomplishing this with Python and Numpy.

At this time, Lunar does not offer a linear algebra or FFT
package. When these things are implemented, they will be built
optionally so as not to add unnecessary bulk or build dependencies to
the code.



# Conventions

Unlike Lua tables with numeric keys, lunar arrays start from index
0. This is to in order to be consistent with C, Python, and numpy. The
Luar C API provides functions for pushing new lunar arrays onto the
stack, and obtaining them from the stack.



# Features

* Arrays of arbitrary dimension are supported, however there is no
  support for slicing operations at this time.

* Vectorized arithmetic operations are fully supported, and are
  carried out entirely in C. These include addition, subtraction,
  multiplication, division, and exponentiation (through '^'). Mixed
  operations on scalar and array quantities work as expected. C
  casting rules are applied to binary operations between mixed array
  types.

* The C math library is exposed and overloaded through lunar. For
  example, lunar.asinh operates correctly on all lunar data
  types. Integral arrays are upcast to double precision, and complex
  arrays use the C complex math functions.

* Lunar provides 7 atomic data types: char, short, int, long, float,
  double, and complex. Complex values are all double precision, and
  are built by adding the imaginary unit to a number, e.g. z = 1 +
  2*lunar.I is the complex number (1,2).



# Lunar API Documentation

## Array element access

Data elements in an array may be accessed as an assignable (l-value)
by using square brackets. If the index is a number, then it is
interpreted as an absolute offset into the internal buffer, i.e. it
works with a 'flattened' view of the array. Multidimensional arrays
may be accessed with a table of indices {i,j,k}. In this case, the
table must have the same length as the array's dimension.

    local A = lunar.zeros({10,10,3})
    A[{9,9,2}] = 3.141592653589
    print(A[{9,9,2}]) -- > '3.141592653589'
    print(A(9,9,2)) -- > '3.141592653589', short-hand for r-value access

Slicing operations are not supported at this time, but may be added
in the future.


## Array member functions

### array:dtype([kind])
***

Returns a string containing the data type of the array. If the
optional argument 'kind' is the string 'enum', instead returns the
type code. Useful for example,

    local B = lunar.zeros(A:shape(), A:dtype('enum'))

### array:shape([kind])
***

Returns an array with the array's dimensions as a table. If the
optional argument 'kind' is the string 'array', instead the shape as
a lunar array. Two ways of printing the shape of an array are

    print(unpack(A:shape())
    print(A:shape('array'))

### array:astable()
***

Returns the array, converted to a Lua table. All data types except
complex are converted to Lua Numbers. The returned table is a
flattened copy of the array.

### array:astype(type)
***

Returns the array, converted to 'type'.

### array:real()
***

Returns the real part of an array. If it is not complex, then a simple
copy of the array is returned.

### array:imag()
***

Returns the imaginary part of an array. If it is not complex, then an
array of zeros with the same shape and data type is returned.

### array:conj()
***

Returns the complex conjugate of an array. If it is not complex, then
a simple copy of the array is returned.

### array:copy(newshape)
***
Returns a deep-copy of the array.

### array:resize(newshape)
***

Same as lunar.resize(A, newshape). Changes the array 'A' in-place.

### array:reshape(newshape)
***

Returns a copy of the array 'A' with the shape 'newshape'. Does not
change 'A' at all.

### array:setasflat()
***
Resizes 'A' in-place to a flattened version.

### array:indices([kind])
***

Returns an iterator over all permutations of valid indices. If the
optional argument 'kind' is the string 'table', then returns a table
containing the indices instead of unpacking them. For example:


    local B = lunar.range(24)
    lunar.resize(B, {2,2,3,2})

    for i,j,k,m in B:indices() do
       print(i,j,k,m, B(i,j,k,m))
    end

    for I in B:indices('table') do
       -- I := { i,j,k,m }
       B[I] = some_function(I[1], I[4])
    end



## Lunar functions

### lunar.array(tab, [dtype]) *** Returns a new array from the table
`tab` of type `dtype`. Default is double precision.

### lunar.zeros(N, [dtype])
***

Returns a new array with `N` elements, initialized to zero, of type
`dtype`. Default is double precision.

### lunar.range(N)
***
Returns the integer array [0,1,...N-1]

### lunar.resize(A, newshape)
***

Resizes the array `A` (in-place) to have the dimensions given in the
table `newshape`. The total size must be unchanged. Arrays of
arbitrary dimension are supported.

### lunar.apply(f, A, B, ...)
***

Returns the lunar array 'C', where C[i] = f(A[i], B[i], ...) for any
number of array input arguments. Arguments must all have the same
shape. The returned array has the highest data type of any of the
inputs.

### lunar.sin, cos, ...(val)
***

Lunar math library function call. Accepts as arguments lunar arrays,
or single numbers of all data types. Overloaded for complex values by
calling the appropriate functions in the C math library. All
functions in the C math library are provided.
