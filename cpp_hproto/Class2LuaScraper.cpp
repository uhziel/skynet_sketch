#include "Class2LuaScraper.h"

using namespace clang;
using namespace llvm;

void Class2LuaScraper::SetContext(clang::ASTContext *_Context) {
  Context = _Context;
}

bool Class2LuaScraper::ScrapeTranslationUnitDecl(
    clang::TranslationUnitDecl *Decl) {
  return ScrapeDeclContext(Decl);
}

bool Class2LuaScraper::ScrapeDeclContext(clang::DeclContext *DeclContext) {
  for (DeclContext::decl_iterator itr = DeclContext->decls_begin();
       itr != DeclContext->decls_end(); itr++) {
    NamedDecl *decl = clang::dyn_cast<NamedDecl>(*itr);
    if (decl == nullptr) {
      continue;
    }

    ScapeNamedDecl(decl);
  }
  return true;
}

bool Class2LuaScraper::ScapeNamedDecl(clang::NamedDecl *decl) {
  switch (decl->getKind()) {
  case Decl::Kind::CXXRecord:
    ScapeCXXRecordDecl(clang::dyn_cast<CXXRecordDecl>(decl));
    break;
  case Decl::Kind::Field:
    ScapeFieldDecl(clang::dyn_cast<FieldDecl>(decl));
    break;
  case Decl::Kind::CXXConstructor:
    ScapeCXXConstructorDecl(clang::dyn_cast<CXXConstructorDecl>(decl));
    break;
  default: {
    llvm::outs() << "Name:" << decl->getQualifiedNameAsString() << " "
                 << "Kind:" << decl->getKind() << " "
                 << "KindName:" << decl->getDeclKindName() << "\n";
    break;
  }
  }
  return true;
}

bool Class2LuaScraper::ScapeCXXRecordDecl(clang::CXXRecordDecl *decl) {
  if (decl->isThisDeclarationADefinition() == VarDecl::DeclarationOnly) {
    return false;
  }
  if (decl->getQualifiedNameAsString() == "Human") {
    decl->dumpColor();
  }
  const clang::Type *type = decl->getTypeForDecl();
  llvm::outs() << "CXXRecordDecl " << decl->getQualifiedNameAsString()
               << " typeClass:" << type->getTypeClassName() << "\n";
  llvm::outs() << "{\n";
  ScrapeDeclContext(decl);
  llvm::outs() << "}\n";

  return true;
}

bool Class2LuaScraper::ScapeFieldDecl(clang::FieldDecl *decl) {
  QualType qual_type = decl->getType(); // from ValueDecl::getType()
  const clang::Type *type = qual_type.getTypePtr();

  llvm::outs() << "FieldDecl " << decl->getQualifiedNameAsString()
               << " typeClass:(" << type->getTypeClass() << ","
               << type->getTypeClassName()
               << " type:" << QualType::getAsString(qual_type.split(), policy)
               << ")\n";
  return true;
}

bool Class2LuaScraper::ScapeCXXConstructorDecl(
    clang::CXXConstructorDecl *decl) {
  if (!decl->isUserProvided()) {
    return false;
  }
  llvm::outs() << "CXXConstructorDecl " << decl->getQualifiedNameAsString()
               << "\n";
  for (CXXConstructorDecl::init_iterator itr = decl->init_begin();
       itr != decl->init_end(); itr++) {
    CXXCtorInitializer *init = *itr;
    ScapeCXXCtorInitializer(init);
  }
  return true;
}

bool Class2LuaScraper::ScapeCXXCtorInitializer(
    clang::CXXCtorInitializer *init) {
  if (!init->isMemberInitializer()) {
    return false;
  }

  FieldDecl *member = init->getMember();
  Expr *expr = init->getInit();
  Expr::EvalResult result;
  expr->EvaluateAsInt(result, *Context);
  APSInt value = result.Val.getInt();
  llvm::outs() << "CXXCtorInitializer " << member->getQualifiedNameAsString()
               << " default_value:" << value << "\n";

  return true;
}
