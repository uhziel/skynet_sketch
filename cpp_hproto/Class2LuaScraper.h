#pragma once

#include "RecordsDatabase.h"
#include "clang/Tooling/Tooling.h"

class Class2LuaScraper {
public:
  explicit Class2LuaScraper(bool Verbose) : Policy(Options) {
    Policy.SuppressTagKeyword = 1;
    Policy.Bool = 1;
    //Policy.SuppressScope = 1;
    //Policy.adjustForCPlusPlus();
  }

  const RecordsDatabase& getDatabase() const;

  void setContext(clang::ASTContext *_Context);
  void scrapeTranslationUnitDecl(clang::TranslationUnitDecl *Decl);
  void scrapeCXXRecordDeclContext(clang::DeclContext *DeclContext, RecordInfo& Out);
  void scrapeCXXRecordDecl(clang::CXXRecordDecl *decl, bool IsRoot = false);
  void scrapeFieldDecl(clang::FieldDecl *decl, RecordInfo& Out);
  void scrapeSubType(clang::QualType QT);
  void parseFieldType(clang::QualType CanonicalQT, FieldTypeInfo &Out);

private:
  clang::ASTContext *Context;
  clang::LangOptions Options;
  clang::PrintingPolicy Policy;
  RecordsDatabase Database;
  bool Verbose;
};
