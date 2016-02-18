include_directories(include ../zlib/include)

add_clang_library(minizip
  source/ioapi.c
  source/iowin32.c
  source/mztools.c
  source/unzip.c
  source/zip.c

  LINK_LIBS
  zlib
  )

if (MSVC)
  set_target_properties(minizip PROPERTIES COMPILE_FLAGS "/wd4131 /wd4189")
endif()
