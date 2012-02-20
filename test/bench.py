#!/usr/bin/env python


import numpy as np


def Test1(N):
    A = np.zeros([N,N])
    for i in range(50):
        B = np.sin(A) + np.cos(A)


if __name__ == "__main__":
    from timeit import Timer
    t = Timer("Test1(512)", "from __main__ import Test1")
    print "numpy: test took %f seconds" % t.timeit(number=1)
