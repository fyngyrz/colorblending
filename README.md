# c++ class: colorblending

This provides both floating point and very highly optimized 8-bit
methods and macros for blending color channels and alpha channels
correctly.

The class depends upon:

    math.h
    stdlib.h

Details for use of the class are laid out in the comments in
colorblending.cpp

In optimizing 8-bit blending, this class trades memory for speed; on a
machine with 4-byte "unsigned ints", that memory cost is 1,572,864
bytes.

What you get for that memory cost is a speed increase; instead of a
square root and a bunch of multiplies and adds, in the case of the
macros, the process consists of three table lookups. If you use the
functions instead, then there is the added overhead of the function
call.

I have done some basic benchmarking. Timing 1,000,000,000 blend
operations, the function-based floating point blend took 20.8s while the
function-based lookup blend took 14.2s, executing about 31% faster.
These numbers will vary by machine and achitecture, but you can see the
advantage in returning results with that much of a speed advantage - it
leaves more CPU for other things, always desirable when doing image
processing.

If you don't want the optimized 8-bit blending, the class supports
disabling it while leaving the real-based blending intact, resulting in
an overall negligable memory load.

In colorblending.cpp I also provide a short technical discussion of why
correct channel blending actually requires a square root and so forth,
if that's of interest to you.

The TLDR version is, if you blend in a naive manner, your channel
values will be wrong, your blended color results will look somewhat to
very sickly, and your software's lack of color-fu will quietly shame
you, even if you're not aware of it. So use the class. :)
 
