#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

class Class2LuaVisitor
  : public RecursiveASTVisitor<Class2LuaVisitor> {
public:
  explicit Class2LuaVisitor(ASTContext *Context)
    : Context(Context) {}

  bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    llvm::outs() << "CXXRecordName:" << Declaration->getQualifiedNameAsString() << "\n";
    return true;
  }

private:
  ASTContext *Context;
};

class Class2LuaConsumer : public clang::ASTConsumer {
public:
  explicit Class2LuaConsumer(ASTContext *Context)
    : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  Class2LuaVisitor Visitor;
};

class Class2LuaAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new Class2LuaConsumer(&Compiler.getASTContext()));
  }
};

int main(int argc, char **argv) {
  if (argc > 1) {
    clang::tooling::runToolOnCode(new Class2LuaAction, argv[1]);
  }
}
