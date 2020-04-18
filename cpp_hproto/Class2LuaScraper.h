#pragma once

#include "clang/Tooling/Tooling.h"

class Class2LuaScraper
{
public:
  explicit Class2LuaScraper()
    : policy(options) {}

  void SetContext(clang::ASTContext *_Context);
  bool ScrapeTranslationUnitDecl(clang::TranslationUnitDecl* Decl);
  bool ScrapeDeclContext(clang::DeclContext* DeclContext);
  bool ScapeNamedDecl(clang::NamedDecl* decl);
  bool ScapeCXXRecordDecl(clang::CXXRecordDecl* decl);
  bool ScapeFieldDecl(clang::FieldDecl* decl);
  bool ScapeCXXConstructorDecl(clang::CXXConstructorDecl* decl);
  bool ScapeCXXCtorInitializer(clang::CXXCtorInitializer* init);

private:
  clang::ASTContext *Context;
  clang::LangOptions options;
  clang::PrintingPolicy policy;
};
