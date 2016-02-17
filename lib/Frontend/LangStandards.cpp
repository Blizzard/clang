//===--- LangStandards.cpp - Language Standard Definitions ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/Frontend/LangStandard.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/ErrorHandling.h"
using namespace clang;
using namespace clang::frontend;

#define LANGSTANDARD(id, name, desc, features) \
  static const LangStandard Lang_##id = { name, desc, features };
#include "clang/Frontend/LangStandards.def"

static const LangStandard::Kind LatestLangStandardForFamily[LangFamily::langfamily_Count] = {
  LangStandard::lang_unspecified,
#define LANGSTANDARD(id, name, desc, features)
#define LANGFAMILY(id, latest_id) \
  LangStandard::lang_##latest_id,
#include "clang/Frontend/LangStandards.def"
};

LangStandard::Kind LangStandard::getLatestLangStandardKindForFamily(LangFamily::Kind K) {
  return LatestLangStandardForFamily[K];
}

const LangStandard &LangStandard::getLangStandardForKind(Kind K) {
  switch (K) {
  case lang_unspecified:
    llvm::report_fatal_error("getLangStandardForKind() on unspecified kind");
#define LANGSTANDARD(id, name, desc, features) \
    case lang_##id: return Lang_##id;
#include "clang/Frontend/LangStandards.def"
  }
  llvm_unreachable("Invalid language kind!");
}

const LangStandard *LangStandard::getLangStandardForName(StringRef Name) {
  Kind K = llvm::StringSwitch<Kind>(Name)
#define LANGSTANDARD(id, name, desc, features) \
    .Case(name, lang_##id)
#include "clang/Frontend/LangStandards.def"
    .Default(lang_unspecified);
  if (K == lang_unspecified)
    return nullptr;

  return &getLangStandardForKind(K);
}


