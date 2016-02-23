//===--- CodeGenOptions.cpp -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/Frontend/CodeGenOptions.h"
#include <string.h>

namespace clang {

CodeGenOptions::CodeGenOptions() {
#define CODEGENOPT(Name, Bits, Default) Name = Default;
#define ENUM_CODEGENOPT(Name, Type, Bits, Default) set##Name(Default);
#include "clang/Frontend/CodeGenOptions.def"

  RelocationModel = "pic";
  memcpy(CoverageVersion, "402*", 4);
}

bool CodeGenOptions::isNoBuiltinFunc(const char *Name) const {
  StringRef FuncName(Name);
  for (unsigned i = 0, e = NoBuiltinFuncs.size(); i != e; ++i)
    if (FuncName.equals(NoBuiltinFuncs[i]))
      return true;
  return false;
}

void CodeGenOptions::getHash(llvm::MD5::MD5Result& HashResult) const {
  llvm::MD5 md5;

#define CODEGENOPT(Name, Bits, Default) { uint32_t val = Name; ArrayRef<uint8_t> OptionsArray(reinterpret_cast<const uint8_t*>(&val), sizeof(uint32_t)); md5.update(OptionsArray); }
#include "clang/Frontend/CodeGenOptions.def"

  md5.final(HashResult);
}

}  // end namespace clang
