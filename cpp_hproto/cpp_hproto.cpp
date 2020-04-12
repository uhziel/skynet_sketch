#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static cl::OptionCategory CppHprotoCategory("cpp_hproto options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

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

class Class2LuaScraper
{
public:
  explicit Class2LuaScraper(ASTContext *Context)
    : Context(Context) {}
  bool ScrapeTranslationUnitDecl(TranslationUnitDecl* Decl)
  {
    return ScrapeDeclContext(Decl);
  }
  bool ScrapeDeclContext(DeclContext* DeclContext)
  {
    for (DeclContext::decl_iterator itr = DeclContext->decls_begin();
      itr != DeclContext->decls_end(); itr++) {
      NamedDecl* decl = clang::dyn_cast<NamedDecl>(*itr);
      if (decl == nullptr) {
        continue;
      }

      ScapeNamedDecl(decl);
    }
    return true;
  }
  bool ScapeNamedDecl(NamedDecl* decl)
  {
    switch (decl->getKind())
    {
      case Decl::Kind::CXXRecord:
        ScapeCXXRecordDecl(clang::dyn_cast<CXXRecordDecl>(decl));
        break;
      case Decl::Kind::Field:
        ScapeFieldDecl(clang::dyn_cast<FieldDecl>(decl));
        break;
      case Decl::Kind::CXXConstructor:
        ScapeCXXConstructorDecl(clang::dyn_cast<CXXConstructorDecl>(decl));
        break;
      default:
      {
        llvm::outs() << "Name:" << decl->getQualifiedNameAsString()
          << " " << "Kind:" << decl->getKind()
          << " " << "KindName:" << decl->getDeclKindName()
          << "\n";
        break;
      }
    }
    return true;
  }
  bool ScapeCXXRecordDecl(CXXRecordDecl* decl)
  {
    if (decl->isThisDeclarationADefinition() == VarDecl::DeclarationOnly)
    {
      return false;
    }
    const clang::Type* type = decl->getTypeForDecl();
    llvm::outs() << "CXXRecordDecl " << decl->getQualifiedNameAsString()
      << " typeClass:" << type->getTypeClassName()
      << "\n";
    llvm::outs() << "{\n";
    ScrapeDeclContext(decl);
    llvm::outs() << "}\n";
  
    return true;
  }
  bool ScapeFieldDecl(FieldDecl* decl)
  {
    QualType qual_type = decl->getType(); //from ValueDecl::getType()
    const clang::Type* type = qual_type.getTypePtr();
    
    llvm::outs() << "FieldDecl " << decl->getQualifiedNameAsString()
      << " typeClass:(" << type->getTypeClass() << "," << type->getTypeClassName()
      << ")\n";
    return true;
  }
  bool ScapeCXXConstructorDecl(CXXConstructorDecl* decl)
  {
    if (!decl->isUserProvided())
    {
      return false;
    }
    llvm::outs() << "CXXConstructorDecl " << decl->getQualifiedNameAsString()
      << "\n";
    for (CXXConstructorDecl::init_iterator itr = decl->init_begin();
      itr != decl->init_end(); itr++)
    {
      CXXCtorInitializer* init = *itr;
      ScapeCXXCtorInitializer(init);
    }
    return true;
  }
  bool ScapeCXXCtorInitializer(CXXCtorInitializer* init)
  {
    if (!init->isMemberInitializer())
    {
      return false;
    }

    FieldDecl* member = init->getMember();
    llvm::outs() << "CXXCtorInitializer " << member->getQualifiedNameAsString()
      << "\n";

    return true;
  }

private:
  ASTContext *Context;
};

class Class2LuaConsumer : public clang::ASTConsumer {
public:
  explicit Class2LuaConsumer(ASTContext *Context)
    : Scraper(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Scraper.ScrapeTranslationUnitDecl(Context.getTranslationUnitDecl());
  }
private:
  Class2LuaScraper Scraper;
};

class Class2LuaAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new Class2LuaConsumer(&Compiler.getASTContext()));
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, CppHprotoCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  Tool.run(newFrontendActionFactory<Class2LuaAction>().get());
}
