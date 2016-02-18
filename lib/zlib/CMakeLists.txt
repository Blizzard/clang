include_directories(include)

add_clang_library(zlib
  source/adler32.c
  source/compress.c
  source/crc32.c
  source/deflate.c
  source/gzclose.c
  source/gzlib.c
  source/gzread.c
  source/infback.c
  source/inffast.c
  source/inflate.c
  source/inftrees.c
  source/trees.c
  source/uncompr.c
  source/zutil.c
  )

if (MSVC)
  set_target_properties(zlib PROPERTIES COMPILE_FLAGS "/wd4131")
endif()
