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

What you get for that memory cost is an enormous speedup; instead of a
square root and a bunch of multiplies and adds, in the case of the macros,
the process consists of three table lookups. If you use the functions
instead, then there is the added overhead of the function call.

In colorblending.cpp I also provide a short technical discussion of why
correct channel blending actually requires a square root and so forth,
if that's of interest to you.

The TLDR version is, if you blend in a naieve manner, your channel
values will be wrong, your blended color results will look somewhat to
very sickly, and your software's lack of color-fu will quietly shame
you, even if you're not aware of it. So use the class. :)
 
