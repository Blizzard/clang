//===----------------------------------------------------------------------===//
// C++ Packaging Development Branch Notes
//===----------------------------------------------------------------------===//

Welcome to the C++ Package Spec proof of concept branch for Clang.  In this
repository, we are implementing the behavior defined in P0235RX specification.

We encourage you to read up on the spec itself, but the short version is that
it adds some new syntax to allow you to use libraries simply by adding a 
directive to your code, without having to futz about with modifying a build
system.  (If the library is organized in the proper layout.)

This project is fairly usable.  In particular, the following features have
been implemented:

- All syntax defined in P0235R0:
  - #using package
  - #using path
  - #using option
- Package semantics:
  - Load package directories
  - Load zip-formatted packages
  - MANIFEST file parsing
  - Package options
  - Package search paths
  - Packages using packages
  - Silence warnings for packages
  - Compile packages with optimizations enabled
- Using Path semantics
  - Use a file
  - Use a path (non-recursive)
  - Use a path (recursive)




There are a number of features and projects still left to implement:

- Linkers other than Visual Studio
  - We build up a list of output files as an addendum to the linker.  Currently
    we've only written the code to pass the extra output files to Visual Studio's
    linker.
- Reading cached precompiled files
  - For '#using path' and '#using package', we want to make sure that it's
    efficent.  Specifically, we want to not compile a file if it's already been
    compiled with the same build settings.  The problem is in recursive
    dependencies.  If a translation unit does a '#using path' on another file,
    and that file itself does a '#using path' on a third file, we have no
    way of knowing that dependency exists if we try to use the cached object
    file.  Same thing for packages.  So, the code exists to check file time-
    stamps, but it's disabled because of this recursive dependency issue.
- Modules
  - Packages are supposed to work with modules, but we have done no work yet
    in this implementation to support Clang's implementation of modules.
- Versions
  - The requested version number for a package is parsed and stored, but we are
    not doing anything with it yet.
- Output of intermediate files
  - We need some canonical location to output intermediate object files for both
    packages and 'using path' files.  Currently, 'using path' files are placed
    next to their source files, and packages are output into an "obj" directory
    inside of the package.
- #pragma package_source
  - Implement the syntax, and implement the semantics.  Ideally, we'd have the
    following sources available:
    - git
    - svn
    - http/https
    - ftp/ftps
    - A plugin model to allow developers to write their own (e.g. Perforce, TFS,
      CVS, Mercurial, etc.)
- #pragma package_version_scheme
  - Implement the syntax, and implement the semantics.  We should definitely
    implement semantic versioning.  Should we implement any other vesion scheme?
- Built-in #using options
  - We should have ARCHITECTURE, PLATFORM, and COMPILER at least
- Package build settings
  - Implement some way to pass package build settings on the commandline.  We may
    want to turn on debugging a particular package, or build it with warnings
    enabled.
- On-Disk package repository
  - There should be a multi-tier package repository: a system-global package cache,
    a per-user package cache, and a per-project package cache.  Each level
    should supercede the previous.
- Search paths for '#using path'
  - We need to have a set of search paths to discover files requested with the
    'using path' directive
    


There are also a few random issues:

- We're overriding the user's choice of language standard version in favor of 
  the latest version of the standard.  This code was written to make C packages
  compile cleanly, but ended up overriding the user's choice.
- The linker file list is always a temporary file, even if you passed the option
  to the driver or requested to save temporaries.

 




//===----------------------------------------------------------------------===//
// C Language Family Front-end
//===----------------------------------------------------------------------===//

Welcome to Clang.  This is a compiler front-end for the C family of languages
(C, C++, Objective-C, and Objective-C++) which is built as part of the LLVM
compiler infrastructure project.

Unlike many other compiler frontends, Clang is useful for a number of things
beyond just compiling code: we intend for Clang to be host to a number of
different source-level tools.  One example of this is the Clang Static Analyzer.

If you're interested in more (including how to build Clang) it is best to read
the relevant web sites.  Here are some pointers:

Information on Clang:              http://clang.llvm.org/
Building and using Clang:          http://clang.llvm.org/get_started.html
Clang Static Analyzer:             http://clang-analyzer.llvm.org/
Information on the LLVM project:   http://llvm.org/

If you have questions or comments about Clang, a great place to discuss them is
on the Clang development mailing list:
  http://lists.llvm.org/mailman/listinfo/cfe-dev

If you find a bug in Clang, please file it in the LLVM bug tracker:
  http://llvm.org/bugs/
