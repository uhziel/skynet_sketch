#pragma once

#include "RecordsDatabase.h"
#include "clang/Tooling/Tooling.h"

class Class2LuaScraper {
public:
  explicit Class2LuaScraper() : Policy(Options) {}

  const RecordsDatabase& getDatabase() const;

  void setContext(clang::ASTContext *_Context);
  void scrapeTranslationUnitDecl(clang::TranslationUnitDecl *Decl);
  void scrapeCXXRecordDeclContext(clang::DeclContext *DeclContext, RecordInfo& Out);
  void scrapeCXXRecordDecl(clang::CXXRecordDecl *decl);
  void scrapeFieldDecl(clang::FieldDecl *decl, RecordInfo& Out);
  void scrapeSubType(clang::QualType QT);

private:
  clang::ASTContext *Context;
  clang::LangOptions Options;
  clang::PrintingPolicy Policy;
  RecordsDatabase Database;
};
