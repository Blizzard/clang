//===--- StmtCXX.h - Classes for representing C++ statements ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the C++ statement AST node classes.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_STMTCXX_H
#define LLVM_CLANG_AST_STMTCXX_H

#include "clang/AST/DeclarationName.h"
#include "clang/AST/Expr.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/Stmt.h"
#include "llvm/Support/Compiler.h"

namespace clang {

class VarDecl;

/// CXXCatchStmt - This represents a C++ catch block.
///
class CXXCatchStmt : public Stmt {
  SourceLocation CatchLoc;
  /// The exception-declaration of the type.
  VarDecl *ExceptionDecl;
  /// The handler block.
  Stmt *HandlerBlock;

public:
  CXXCatchStmt(SourceLocation catchLoc, VarDecl *exDecl, Stmt *handlerBlock)
  : Stmt(CXXCatchStmtClass), CatchLoc(catchLoc), ExceptionDecl(exDecl),
    HandlerBlock(handlerBlock) {}

  CXXCatchStmt(EmptyShell Empty)
  : Stmt(CXXCatchStmtClass), ExceptionDecl(nullptr), HandlerBlock(nullptr) {}

  SourceLocation getLocStart() const LLVM_READONLY { return CatchLoc; }
  SourceLocation getLocEnd() const LLVM_READONLY {
    return HandlerBlock->getLocEnd();
  }

  SourceLocation getCatchLoc() const { return CatchLoc; }
  VarDecl *getExceptionDecl() const { return ExceptionDecl; }
  QualType getCaughtType() const;
  Stmt *getHandlerBlock() const { return HandlerBlock; }

  static bool classof(const Stmt *T) {
    return T->getStmtClass() == CXXCatchStmtClass;
  }

  child_range children() { return child_range(&HandlerBlock, &HandlerBlock+1); }

  friend class ASTStmtReader;
};

/// CXXTryStmt - A C++ try block, including all handlers.
///
class CXXTryStmt : public Stmt {
  SourceLocation TryLoc;
  unsigned NumHandlers;

  CXXTryStmt(SourceLocation tryLoc, Stmt *tryBlock, ArrayRef<Stmt*> handlers);

  CXXTryStmt(EmptyShell Empty, unsigned numHandlers)
    : Stmt(CXXTryStmtClass), NumHandlers(numHandlers) { }

  Stmt const * const *getStmts() const {
    return reinterpret_cast<Stmt const * const*>(this + 1);
  }
  Stmt **getStmts() {
    return reinterpret_cast<Stmt **>(this + 1);
  }

public:
  static CXXTryStmt *Create(const ASTContext &C, SourceLocation tryLoc,
                            Stmt *tryBlock, ArrayRef<Stmt*> handlers);

  static CXXTryStmt *Create(const ASTContext &C, EmptyShell Empty,
                            unsigned numHandlers);

  SourceLocation getLocStart() const LLVM_READONLY { return getTryLoc(); }
  SourceLocation getLocEnd() const LLVM_READONLY { return getEndLoc(); }

  SourceLocation getTryLoc() const { return TryLoc; }
  SourceLocation getEndLoc() const {
    return getStmts()[NumHandlers]->getLocEnd();
  }

  CompoundStmt *getTryBlock() {
    return cast<CompoundStmt>(getStmts()[0]);
  }
  const CompoundStmt *getTryBlock() const {
    return cast<CompoundStmt>(getStmts()[0]);
  }

  unsigned getNumHandlers() const { return NumHandlers; }
  CXXCatchStmt *getHandler(unsigned i) {
    return cast<CXXCatchStmt>(getStmts()[i + 1]);
  }
  const CXXCatchStmt *getHandler(unsigned i) const {
    return cast<CXXCatchStmt>(getStmts()[i + 1]);
  }

  static bool classof(const Stmt *T) {
    return T->getStmtClass() == CXXTryStmtClass;
  }

  child_range children() {
    return child_range(getStmts(), getStmts() + getNumHandlers() + 1);
  }

  friend class ASTStmtReader;
};

/// CXXForRangeStmt - This represents C++0x [stmt.ranged]'s ranged for
/// statement, represented as 'for (range-declarator : range-expression)'.
///
/// This is stored in a partially-desugared form to allow full semantic
/// analysis of the constituent components. The original syntactic components
/// can be extracted using getLoopVariable and getRangeInit.
class CXXForRangeStmt : public Stmt {
  SourceLocation ForLoc;
  enum { RANGE, BEGINEND, COND, INC, LOOPVAR, BODY, END };
  // SubExprs[RANGE] is an expression or declstmt.
  // SubExprs[COND] and SubExprs[INC] are expressions.
  Stmt *SubExprs[END];
  SourceLocation CoawaitLoc;
  SourceLocation ColonLoc;
  SourceLocation RParenLoc;

  friend class ASTStmtReader;
public:
  CXXForRangeStmt(DeclStmt *Range, DeclStmt *BeginEnd,
                  Expr *Cond, Expr *Inc, DeclStmt *LoopVar, Stmt *Body,
                  SourceLocation FL, SourceLocation CAL, SourceLocation CL,
                  SourceLocation RPL);
  CXXForRangeStmt(EmptyShell Empty) : Stmt(CXXForRangeStmtClass, Empty) { }


  VarDecl *getLoopVariable();
  Expr *getRangeInit();

  const VarDecl *getLoopVariable() const;
  const Expr *getRangeInit() const;


  DeclStmt *getRangeStmt() { return cast<DeclStmt>(SubExprs[RANGE]); }
  DeclStmt *getBeginEndStmt() {
    return cast_or_null<DeclStmt>(SubExprs[BEGINEND]);
  }
  Expr *getCond() { return cast_or_null<Expr>(SubExprs[COND]); }
  Expr *getInc() { return cast_or_null<Expr>(SubExprs[INC]); }
  DeclStmt *getLoopVarStmt() { return cast<DeclStmt>(SubExprs[LOOPVAR]); }
  Stmt *getBody() { return SubExprs[BODY]; }

  const DeclStmt *getRangeStmt() const {
    return cast<DeclStmt>(SubExprs[RANGE]);
  }
  const DeclStmt *getBeginEndStmt() const {
    return cast_or_null<DeclStmt>(SubExprs[BEGINEND]);
  }
  const Expr *getCond() const {
    return cast_or_null<Expr>(SubExprs[COND]);
  }
  const Expr *getInc() const {
    return cast_or_null<Expr>(SubExprs[INC]);
  }
  const DeclStmt *getLoopVarStmt() const {
    return cast<DeclStmt>(SubExprs[LOOPVAR]);
  }
  const Stmt *getBody() const { return SubExprs[BODY]; }

  void setRangeInit(Expr *E) { SubExprs[RANGE] = reinterpret_cast<Stmt*>(E); }
  void setRangeStmt(Stmt *S) { SubExprs[RANGE] = S; }
  void setBeginEndStmt(Stmt *S) { SubExprs[BEGINEND] = S; }
  void setCond(Expr *E) { SubExprs[COND] = reinterpret_cast<Stmt*>(E); }
  void setInc(Expr *E) { SubExprs[INC] = reinterpret_cast<Stmt*>(E); }
  void setLoopVarStmt(Stmt *S) { SubExprs[LOOPVAR] = S; }
  void setBody(Stmt *S) { SubExprs[BODY] = S; }

  SourceLocation getForLoc() const { return ForLoc; }
  SourceLocation getCoawaitLoc() const { return CoawaitLoc; }
  SourceLocation getColonLoc() const { return ColonLoc; }
  SourceLocation getRParenLoc() const { return RParenLoc; }

  SourceLocation getLocStart() const LLVM_READONLY { return ForLoc; }
  SourceLocation getLocEnd() const LLVM_READONLY {
    return SubExprs[BODY]->getLocEnd();
  }

  static bool classof(const Stmt *T) {
    return T->getStmtClass() == CXXForRangeStmtClass;
  }

  // Iterators
  child_range children() {
    return child_range(&SubExprs[0], &SubExprs[END]);
  }
};

/// \brief Representation of a Microsoft __if_exists or __if_not_exists
/// statement with a dependent name.
///
/// The __if_exists statement can be used to include a sequence of statements
/// in the program only when a particular dependent name does not exist. For
/// example:
///
/// \code
/// template<typename T>
/// void call_foo(T &t) {
///   __if_exists (T::foo) {
///     t.foo(); // okay: only called when T::foo exists.
///   }
/// }
/// \endcode
///
/// Similarly, the __if_not_exists statement can be used to include the
/// statements when a particular name does not exist.
///
/// Note that this statement only captures __if_exists and __if_not_exists
/// statements whose name is dependent. All non-dependent cases are handled
/// directly in the parser, so that they don't introduce a new scope. Clang
/// introduces scopes in the dependent case to keep names inside the compound
/// statement from leaking out into the surround statements, which would
/// compromise the template instantiation model. This behavior differs from
/// Visual C++ (which never introduces a scope), but is a fairly reasonable
/// approximation of the VC++ behavior.
class MSDependentExistsStmt : public Stmt {
  SourceLocation KeywordLoc;
  bool IsIfExists;
  NestedNameSpecifierLoc QualifierLoc;
  DeclarationNameInfo NameInfo;
  Stmt *SubStmt;

  friend class ASTReader;
  friend class ASTStmtReader;

public:
  MSDependentExistsStmt(SourceLocation KeywordLoc, bool IsIfExists,
                        NestedNameSpecifierLoc QualifierLoc,
                        DeclarationNameInfo NameInfo,
                        CompoundStmt *SubStmt)
  : Stmt(MSDependentExistsStmtClass),
    KeywordLoc(KeywordLoc), IsIfExists(IsIfExists),
    QualifierLoc(QualifierLoc), NameInfo(NameInfo),
    SubStmt(reinterpret_cast<Stmt *>(SubStmt)) { }

  /// \brief Retrieve the location of the __if_exists or __if_not_exists
  /// keyword.
  SourceLocation getKeywordLoc() const { return KeywordLoc; }

  /// \brief Determine whether this is an __if_exists statement.
  bool isIfExists() const { return IsIfExists; }

  /// \brief Determine whether this is an __if_exists statement.
  bool isIfNotExists() const { return !IsIfExists; }

  /// \brief Retrieve the nested-name-specifier that qualifies this name, if
  /// any.
  NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }

  /// \brief Retrieve the name of the entity we're testing for, along with
  /// location information
  DeclarationNameInfo getNameInfo() const { return NameInfo; }

  /// \brief Retrieve the compound statement that will be included in the
  /// program only if the existence of the symbol matches the initial keyword.
  CompoundStmt *getSubStmt() const {
    return reinterpret_cast<CompoundStmt *>(SubStmt);
  }

  SourceLocation getLocStart() const LLVM_READONLY { return KeywordLoc; }
  SourceLocation getLocEnd() const LLVM_READONLY { return SubStmt->getLocEnd();}

  child_range children() {
    return child_range(&SubStmt, &SubStmt+1);
  }

  static bool classof(const Stmt *T) {
    return T->getStmtClass() == MSDependentExistsStmtClass;
  }
};

/// \brief Represents the body of a coroutine. This wraps the normal function
/// body and holds the additional semantic context required to set up and tear
/// down the coroutine frame.
class CoroutineBodyStmt : public Stmt {
  enum SubStmt {
    Body,          ///< The body of the coroutine.
    Promise,       ///< The promise statement.
    InitSuspend,   ///< The initial suspend statement, run before the body.
    FinalSuspend,  ///< The final suspend statement, run after the body.
    OnException,   ///< Handler for exceptions thrown in the body.
    OnFallthrough, ///< Handler for control flow falling off the body.
    ReturnValue,   ///< Return value for thunk function.
    FirstParamMove ///< First offset for move construction of parameter copies.
  };
  Stmt *SubStmts[SubStmt::FirstParamMove];

  friend class ASTStmtReader;
public:
  CoroutineBodyStmt(Stmt *Body, Stmt *Promise, Stmt *InitSuspend,
                    Stmt *FinalSuspend, Stmt *OnException, Stmt *OnFallthrough,
                    Expr *ReturnValue, ArrayRef<Expr *> ParamMoves)
      : Stmt(CoroutineBodyStmtClass) {
    SubStmts[CoroutineBodyStmt::Body] = Body;
    SubStmts[CoroutineBodyStmt::Promise] = Promise;
    SubStmts[CoroutineBodyStmt::InitSuspend] = InitSuspend;
    SubStmts[CoroutineBodyStmt::FinalSuspend] = FinalSuspend;
    SubStmts[CoroutineBodyStmt::OnException] = OnException;
    SubStmts[CoroutineBodyStmt::OnFallthrough] = OnFallthrough;
    SubStmts[CoroutineBodyStmt::ReturnValue] = ReturnValue;
    // FIXME: Tail-allocate space for parameter move expressions and store them.
    assert(ParamMoves.empty() && "not implemented yet");
  }

  /// \brief Retrieve the body of the coroutine as written. This will be either
  /// a CompoundStmt or a TryStmt.
  Stmt *getBody() const {
    return SubStmts[SubStmt::Body];
  }

  Stmt *getPromiseDeclStmt() const { return SubStmts[SubStmt::Promise]; }
  VarDecl *getPromiseDecl() const {
    return cast<VarDecl>(cast<DeclStmt>(getPromiseDeclStmt())->getSingleDecl());
  }

  Stmt *getInitSuspendStmt() const { return SubStmts[SubStmt::InitSuspend]; }
  Stmt *getFinalSuspendStmt() const { return SubStmts[SubStmt::FinalSuspend]; }

  Stmt *getExceptionHandler() const { return SubStmts[SubStmt::OnException]; }
  Stmt *getFallthroughHandler() const {
    return SubStmts[SubStmt::OnFallthrough];
  }

  Expr *getReturnValueInit() const {
    return cast<Expr>(SubStmts[SubStmt::ReturnValue]);
  }

  SourceLocation getLocStart() const LLVM_READONLY {
    return getBody()->getLocStart();
  }
  SourceLocation getLocEnd() const LLVM_READONLY {
    return getBody()->getLocEnd();
  }

  child_range children() {
    return child_range(SubStmts, SubStmts + SubStmt::FirstParamMove);
  }

  static bool classof(const Stmt *T) {
    return T->getStmtClass() == CoroutineBodyStmtClass;
  }
};

/// \brief Represents a 'co_return' statement in the C++ Coroutines TS.
///
/// This statament models the initialization of the coroutine promise
/// (encapsulating the eventual notional return value) from an expression
/// (or braced-init-list), followed by termination of the coroutine.
///
/// This initialization is modeled by the evaluation of the operand
/// followed by a call to one of:
///   <promise>.return_value(<operand>)
///   <promise>.return_void()
/// which we name the "promise call".
class CoreturnStmt : public Stmt {
  SourceLocation CoreturnLoc;

  enum SubStmt { Operand, PromiseCall, Count };
  Stmt *SubStmts[SubStmt::Count];

  friend class ASTStmtReader;
public:
  CoreturnStmt(SourceLocation CoreturnLoc, Stmt *Operand, Stmt *PromiseCall)
      : Stmt(CoreturnStmtClass), CoreturnLoc(CoreturnLoc) {
    SubStmts[SubStmt::Operand] = Operand;
    SubStmts[SubStmt::PromiseCall] = PromiseCall;
  }

  SourceLocation getKeywordLoc() const { return CoreturnLoc; }

  /// \brief Retrieve the operand of the 'co_return' statement. Will be nullptr
  /// if none was specified.
  Expr *getOperand() const { return static_cast<Expr*>(SubStmts[Operand]); }

  /// \brief Retrieve the promise call that results from this 'co_return'
  /// statement. Will be nullptr if either the coroutine has not yet been
  /// finalized or the coroutine has no eventual return type.
  Expr *getPromiseCall() const {
    return static_cast<Expr*>(SubStmts[PromiseCall]);
  }

  SourceLocation getLocStart() const LLVM_READONLY { return CoreturnLoc; }
  SourceLocation getLocEnd() const LLVM_READONLY {
    return getOperand()->getLocEnd();
  }

  child_range children() {
    return child_range(SubStmts, SubStmts + SubStmt::Count);
  }

  static bool classof(const Stmt *T) {
    return T->getStmtClass() == CoreturnStmtClass;
  }
};

}  // end namespace clang

#endif
