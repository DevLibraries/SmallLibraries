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


### The Lists

The lists links to all of the original libraries repositories or homepages, I have kept the names the same in this repository should you need to compare.
They are organised into 2 lists Public domain and Permissive, they are also sorted by compiler; 

- C
- C++
- C/C++


&nbsp;
######PublicDomain 

category          | library                                                               | license              | API |files| description
----------------- | --------------------------------------------------------------------- |:--------------------:|:---:|:---:| -----------
argv              |  [parg](https://github.com/jibsen/parg)                               |  public&nbsp;domain      |  C  |  1  | command-line argument parsing
crypto            |  [TweetNaCl](http://tweetnacl.cr.yp.to/software.html)                 |   public domain      |  C  |  2  | high-quality tiny cryptography library
data structures   |  [PackedArray](https://github.com/gpakosz/PackedArray)                |   WTFPLv2            |  C  |  2  | memory-efficient array of elements with non-pow2 bitcount
data structures   |  [minilibs](https://github.com/ccxvii/minilibs)                       |   public domain      |  C  |  2  | two-file binary tress (also regex, etc)
images            |  [gif.h](https://github.com/ginsweater/gif-h)                         |   public domain      |  C  |  1  | animated GIF writer (can only include once)
parsing           |  [minilibs](https://github.com/ccxvii/minilibs)                       |   public domain      |  C  |  2  | two-file regex (also binary tree, etc)
geometry file     |  [tinyply](https://github.com/ddiakopoulos/tinyply)                   |   public&nbsp;domain      | C++ |  2  | PLY mesh file loader
graphics(3D)    |  [debug-draw](https://github.com/glampert/debug-draw)                  |   public domain      | C++ |  1  | API-agnostic immediate-mode debug rendering
images            |  [jo_gif.cpp](http://www.jonolick.com/home/gif-writer)                |   public domain      | C++ |  1  | animated GIF writer (CPP file can also be used as H file)
images            |  [miniexr](https://github.com/aras-p/miniexr)                         |   public domain      | C++ |  2  | OpenEXR writer, needs header file
images            |  [jpeg-compressor](https://github.com/richgel999/jpeg-compressor)     |   public domain      | C++ |  2  | 2-file jpeg compress, 2-file jpeg decompress
math              |  [linalg.h](https://github.com/sgorsten/linalg)                       |   unlicense          | C++ |  1  | vector/matrix/quaternion math
profiling         |  [MicroProfile](https://bitbucket.org/jonasmeyer/microprofile)        |   unlicense          | C++ | 2-4 | CPU (and GPU?) profiler, 1-3 header files, uses miniz internally
tests             |  [pempek_assert.cpp](https://github.com/gpakosz/Assert)               |   WTFPLv2            | C++ |  2  | flexible assertions
_misc_            |  [stb](https://github.com/nothings/stb)                               |   public domain      | C++ |  1  | Mixed bag of different libraries
_misc_            |  [MakeID.h](http://www.humus.name/3D/MakeID.h)                        |   public domain      | C++ |  1  | allocate/deallocate small integer IDs efficiently
_misc_            |  [loguru](https://github.com/emilk/loguru)                            |   public domain      | C++ |  1  | flexible logging
audio             |  [dr_flac](https://github.com/mackron/dr_libs)                        |  public&nbsp;domain       |C/C++|  1  | FLAC audio decoder
compression       |  [miniz.c](https://github.com/richgel999/miniz)                       |  public domain       |C/C++|  1  | compression,decompression, zip file, png writing
files & filenames |  [DG_misc.h](https://github.com/DanielGibson/Snippets/)               |  public domain       |C/C++|  1  | Daniel Gibson's stb.h-esque cross-platform helpers: path/file, strings
files & filenames |  [whereami](https://github.com/gpakosz/whereami)                      |  WTFPLv2             |C/C++|  2  | get path/filename of executable or module
geometry math     |  [nv_voronoi.h](http://www.icculus.org/~mordred/nvlib/)               |  public domain       |C/C++|  1  | find voronoi regions on lattice w/ integer inputs
geometry math     |  [sobol.h](https://github.com/Marc-B-Reynolds/Stand-alone-junk/)      |  public domain       |C/C++|  1  | sobol & stratified sampling sequences
geometry math     |  [Tomas Akenine-Moller snippets](http://tinyurl.com/ht79ndj)          |  public domain       |C/C++|  2  | various 3D intersection calculations, not lib-ified
graphics (2D)     |  [tigr](https://bitbucket.org/rmitton/tigr/src)                       |  public domain       |C/C++|  2  | quick-n-dirty window text/graphics for Windows and OSX
graphics (3D)    |  [lightmapper](https://github.com/ands/lightmapper#lightmapper)       |  public domain       |C/C++|  1  | use your OpenGL renderer to offline bake lightmaps
hardware          |  [EasyTab](https://github.com/ApoorvaJ/EasyTab)                       |  public domain       |C/C++|  1  | multi-platform tablet input
images            |  [tiny_jpeg.h](https://github.com/serge-rgb/TinyJPEG/)                |  public domain       |C/C++|  1  | JPEG encoder
images            |  [cro_mipmap.h](https://github.com/thebeast33/cro_lib)                |  public domain       |C/C++|  1  | average, min, max mipmap generators
math              |  [prns.h](http://marc-b-reynolds.github.io/shf/2016/04/19/prns.html)  |  public domain       |C/C++|  1  | seekable pseudo-random number sequences
network           |  [zed_net](https://github.com/ZedZull/zed_net)                        |  public domain       |C/C++|  1  | cross-platform socket wrapper
network           |  [yocto](https://github.com/tom-seddon/yhs)                           |  public domain       |C/C++|  2  | non-production-use http server
parsing           |  [json.h](https://github.com/sheredom/json.h)                         |  public domain       |C/C++|  2  | JSON parser
strings           |  [DG_misc.h](https://github.com/DanielGibson/Snippets/)               |  public domain       |C/C++|  1  | Daniel Gibson's stb.h-esque cross-platform helpers: path/file, strings         
strings           |  [utf8](https://github.com/sheredom/utf8.h)                           |  public domain       |C/C++|  1  | utf8 string library
strings           |  [strpool.h](https://github.com/mattiasgustavsson/libs)               |  public domain       |C/C++|  1  | string interning
strings           |  [gb_string.h](https://github.com/gingerBill/gb)                      |  public domain       |C/C++|  1  | dynamic strings
user interface    |  [nuklear](https://github.com/vurtun/nuklear)                         |  public domain       |C/C++|  1  | minimal GUI toolkit
                                                                                                                       



&nbsp;
######Permissive 
category          | library                                                               | license              | API |files| description
----------------- | --------------------------------------------------------------------- |:--------------------:|:---:|:---:| -----------
files & filenames |  [TinyDir](https://github.com/cxong/tinydir)                          | BSD                  |  C  |  1  | cross-platform directory reader
math              |  [TinyExpr](https://github.com/codeplea/tinyexpr)                     | zlib                 |  C  |  2  | evaluation of math expressions from strings
tests             |  [minctest](https://github.com/codeplea/minctest)                     | zlib                 |  C  |  1  | unit testing
tests             |  [greatest](https://github.com/silentbicycle/greatest)                | iSC                  |  C  |  1  | unit testing
tests             |  [µnit](https://github.com/nemequ/munit)                              | MIT                  |  C  |  1  | unit testing
_misc_            |  [stmr](https://github.com/wooorm/stmr.c)                             | MIT                  |  C  |  2  | extract English word stems
_misc_            |  [levenshtein](https://github.com/wooorm/levenshtein.c)               | MIT                  |  C  |  2  | compute edit distance between two strings
AI                |  [micropather](http://www.grinninglizard.com/MicroPather/)            | zlib                 | C++ |  2  | pathfinding with A*
geometry file     |  [tinyobjloader](https://github.com/syoyo/tinyobjloader)              | BSD                  | C++ |  1  | wavefront OBJ file loader
geometry math     |  [nanoflann](https://github.com/jlblancoc/nanoflann)                  | BSD                  | C++ |  1  | build KD trees for point clouds
geometry math     |  [Clipper](http://www.angusj.com/delphi/clipper.php)                  | Boost                | C++ |  2  | line & polygon clipping & offsetting
geometry math     |  [PolyPartition](https://github.com/ivanfratric/polypartition)        | MIT                  | C++ |  2  | polygon triangulation, partitioning
images            |  [picopng.cpp](http://lodev.org/lodepng/picopng.cpp)                  | zlib                 | C++ |  2  | tiny PNG loader
images            |  [easyexif](https://github.com/mayanklahiri/easyexif)                 | MIT                  | C++ |  2  | EXIF metadata extractor for JPEG images
math              |  [ShaderFastLibs](https://github.com/michaldrobot/ShaderFastLibs)     | MIT                  | C++ |  1  | (also HLSL) approximate transcendental functions optimized for shaders (esp. GCN)
math              |  [PoissonGenerator.h](https://github.com/corporateshark/poisson-disk-generator) | MIT        | C++ |  1  | Poisson disk points generator (disk or rect)
network           |  [happyhttp](http://scumways.com/happyhttp/happyhttp.html)            | zlib                 | C++ |  2  | http client requests
network           |  [LUrlParser](https://github.com/corporateshark/LUrlParser)           | MIT                  | C++ |  2  | lightweight URL & URI parser RFC 1738, RFC 3986
parsing           |  [PicoJSON](https://github.com/kazuho/picojson)                       | BSD                  | C++ |  1  | JSON parse/serializer
parsing           |  [jzon.h](https://github.com/Zguy/Jzon)                               | MIT                  | C++ |  2  | JSON parser
scripting         |  [lualite](https://github.com/janezz55/lualite/)                      | MIT                  | C++ |  1  | generate lua bindings in C++
tests             |  [catch](https://github.com/philsquared/Catch)                        | Boost                | C++ |  1  | unit testing
user interface    |  [dear imgui](https://github.com/ocornut/imgui)                       | MIT                  | C++ |  9  | an immediate-mode GU
_misc_            |  [tinyformat](https://github.com/c42f/tinyformat)                     | Boost                | C++ |  1  | typesafe printf
audio             |  [aw_ima.h](https://github.com/afterwise/aw-ima/blob/master/aw-ima.h) |MIT                  |C/C++|  1  | IMA-ADPCM audio decoder
compression       |  [lz4](https://github.com/Cyan4973/lz4)                               | BSD                  |C/C++|  2  | fast but larger LZ compression
compression       |  [fastlz](https://code.google.com/archive/p/fastlz/source/default/source) | MIT              |C/C++|  2  | fast but larger LZ compression
compression       |  [pithy](https://github.com/johnezang/pithy)                          | BSD                  |C/C++|  2  | fast but larger LZ compression
data&nbsp;structures|[klib](http://attractivechaos.github.io/klib/)                       | MIT                  |C/C++|  2  | many 2-file libs: hash, sort, b-tree, etc
data structures   |  [uthash](https://github.com/troydhanson/uthash)                      | BSD                  |C/C++|  2  | several 1-header, 1-license-file libs: generic hash, list, etc
files & filenames |  [noc_file_dialog.h](https://github.com/guillaumechereau/noc)         | MIT                  |C/C++|  1  | file open/save dialogs (Linux/OSX/Windows)
files & filenames |  [dirent](https://github.com/tronkko/dirent)                          | MIT                  |C/C++|  1  | dirent for windows: retrieve file & dir info
geometry file     |  [tk_objfile](https://github.com/joeld42/tk_objfile)                  | MIT                  |C/C++|  1  | OBJ file loader
geometry file     |  [yocto_obj.h](https://github.com/xelatihy/yocto-gl)                  | MIT                  |C/C++|  1  | wavefront OBJ file loader
geometry math     |  [sdf.h](https://github.com/memononen/SDF)                            | MIT                  |C/C++|  1  | compute signed-distance field from antialiased image
geometry math     |  [jc_voronoi](https://github.com/JCash/voronoi)                       | MIT                  |C/C++|  1  | find voronoi regions on float/double data
geometry math     |  [par_msquares](https://github.com/prideout/par)                      | MIT                  |C/C++|  1  | convert (binarized) image to triangles
geometry math     |  [par_shapes](http://github.prideout.net/shapes)                      | MIT                  |C/C++|  1  | generate various 3d geometric shapes
geometry math     |  [Voxelizer](https://github.com/karimnaaji/voxelizer)                 | MIT                  |C/C++|  1  | convert triangle mesh to voxel triangle mesh
geometry math     |  [yocto_bvh.h](https://github.com/xelatihy/yocto-gl)                  | MIT                  |C/C++|  1  | ray-casting of bounding-volume hierarchy
geometry math     |  [yocto_shape.h](https://github.com/xelatihy/yocto-gl)                | MIT                  |C/C++|  1  | shape generation, tesselation, normals, etc.
geometry math     |  [yocto_trace.h](https://github.com/xelatihy/yocto-gl)                | MIT                  |C/C++|  1  | physically-based unidirectional path tracer w/ MIS for direct lights
graphics (2D)     |  [blendish](https://bitbucket.org/duangle/oui-blendish/src)           | MIT                  |C/C++|  1  | blender-style widget rendering
graphics (2D)     |  [noc_turtle](https://github.com/guillaumechereau/noc)                | MIT                  |C/C++|  2  | procedural graphics generator
graphics (3D)    |  [mikktspace](http://tinyurl.com/z6xtucm)                             | zlib                 |C/C++|  2  | compute tangent space for normal mapping
images            |  [tinyexr](https://github.com/syoyo/tinyexr)                          | BSD                  |C/C++|  1  | EXR image read/write, uses miniz internally  
images            |  [lodepng](http://lodev.org/lodepng/)                                 | zlib                 |C/C++|  2  | PNG encoder/decoder
images            |  [nanoSVG](https://github.com/memononen/nanosvg)                      | zlib                 |C/C++|  1  | 1-file SVG parser; 1-file SVG rasterizer
math              |  [mm_vec.h](https://github.com/vurtun/mmx)                            | BSD                  |C/C++|  1  | SIMD vector math
multithreading    |  [mm_sched.h](https://github.com/vurtun/mmx)                          | zlib                 |C/C++|  1  | cross-platform multithreaded task scheduler
network           |  [mm_web.h](https://github.com/vurtun/mmx)                            | BSD                  |C/C++|  1  | lightweight webserver, fork of webby
network           |  [par_easycurl.h](https://github.com/prideout/par)                    | MIT                  |C/C++|  1  | curl wrapper
parsing           |  [mm_lexer.h](https://github.com/vurtun/mmx)                          | zlib                 |C/C++|  1  | C-esque language lexer
parsing           |  [parson](https://github.com/kgabis/parson)                           | MIT                  |C/C++|  2  | JSON parser and serializer
profiling         |  [Remotery](https://github.com/Celtoys/Remotery)                      | Apache 2.0           |C/C++|  2  | CPU/GPU profiler Win/Mac/Linux, using web browser for viewer
scripting         |  [LIL](http://runtimelegend.com/rep/lil/)                             | zlib                 |C/C++|  2  | interpreter for a Tcl-like scripting language
scripting         |  [Picol](https://chiselapp.com/user/dbohdan/repository/picol/)        | BSD                  |C/C++|  1  | interpreter for a Tcl-like scripting language
strings           |  [dfa](http://bjoern.hoehrmann.de/utf-8/decoder/dfa/)                 | MIT                  |C/C++|  2  | fast utf8 decoder (need a header file)
tests             |  [utest](https://github.com/evolutional/utest)                        | MIT                  |C/C++|  1  | unit testing
tests             |  [SPUT](http://www.lingua-systems.com/unit-testing/)                  | BSD                  |C/C++|  1  | unit testing
_misc_            |  [dbgtools](https://github.com/wc-duck/dbgtools)                      | zlib                 |C/C++|  2  | cross-platform debug util libraries


&nbsp;
######EOF
