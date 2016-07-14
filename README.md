# Small 1-2 file, public-domain/open source libraries with minimal dependencies

The original lists were created by  [nothings](https://github.com/nothings) who has a small library of his own as well as a list of others; This is mostly his original work with non permissive GPL items removed

The following is a list of small, easy-to-integrate, portable libraries
which are usable from C and/or C++, and should be able to be compiled on both
32-bit and 64-bit platforms.

### Rules

- Libraries must be usable from C or C++, ideally both
- Libraries should be usable from more than one platform (ideally, all major desktops and/or all major mobile)
- Libraries should compile and work on both 32-bit and 64-bit platforms
- Libraries should use at most two files


For the API column, "C" means C only, "C++" means C++ only, and "C/C++" means C/C++ usable
from either; some files may require *building* as C or C++ but still qualify as "C/C++" as
long as the header file uses `extern "C"` to make it work. (In some cases, a header-file-only
library may compile as both C or C++, but produce an implementation that can only be called from
one or the other, because of a lack of use of `extern "C"`; in this case the table still qualifies it
as C/C++, as this is not an obstacle to most users.)


The Libraries were all added here for my usage, and for simple downloading in one large easy package, 
however should you find them useful as a package then download away,  thats why this was made public :)