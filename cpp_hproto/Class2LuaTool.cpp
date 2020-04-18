#include "Class2LuaTool.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "Class2LuaScraper.h"

class Class2LuaConsumer : public clang::ASTConsumer {
public:
  explicit Class2LuaConsumer(Class2LuaScraper *Scraper) : Scraper(Scraper) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Scraper->ScrapeTranslationUnitDecl(Context.getTranslationUnitDecl());
  }

private:
  Class2LuaScraper *Scraper;
};

class Class2LuaAction : public clang::ASTFrontendAction {
public:
  Class2LuaAction(Class2LuaScraper *Scraper) : Scraper(Scraper) {}
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    Scraper->SetContext(&Compiler.getASTContext());
    return std::unique_ptr<clang::ASTConsumer>(
        new Class2LuaConsumer(Scraper)
    );
  }

private:
  Class2LuaScraper *Scraper;
};

class Class2LuaActionFactory : public clang::tooling::FrontendActionFactory {
public:
  Class2LuaActionFactory(Class2LuaScraper *Scraper) : Scraper(Scraper) {}
  clang::FrontendAction *create() override {
    return new Class2LuaAction(Scraper);
  }

private:
  Class2LuaScraper *Scraper;
};

Class2LuaTool::Class2LuaTool(
    const clang::tooling::CompilationDatabase &Compilations,
    llvm::ArrayRef<std::string> SourcePaths)
    : Tool(Compilations, SourcePaths) {}

void Class2LuaTool::Run(Class2LuaScraper *Scraper) {
  auto Factory = std::unique_ptr<clang::tooling::FrontendActionFactory>(
    new Class2LuaActionFactory(Scraper)
  );
  Tool.run(Factory.get());
}
