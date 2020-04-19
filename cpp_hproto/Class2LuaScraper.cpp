#include "Class2LuaScraper.h"

using namespace clang;
using namespace llvm;

const std::string FILTER_KEY = "Hum";

const RecordsDatabase &Class2LuaScraper::getDatabase() const { return Database; }

void Class2LuaScraper::setContext(clang::ASTContext *_Context) {
  Context = _Context;
}

void Class2LuaScraper::scrapeTranslationUnitDecl(
    clang::TranslationUnitDecl *Decl) {
  clang::DeclContext *DeclContext = clang::dyn_cast<clang::DeclContext>(Decl);
  for (clang::DeclContext::decl_iterator itr = DeclContext->decls_begin();
       itr != DeclContext->decls_end(); itr++) {
    CXXRecordDecl *decl = clang::dyn_cast<CXXRecordDecl>(*itr);
    if (decl == nullptr) {
      continue;
    }

    if (decl->getIdentifier() == nullptr) {
      continue;
    }

    if (std::string::npos == decl->getName().find(FILTER_KEY))
    {
      continue;
    }

    scrapeCXXRecordDecl(decl);
  }
}

void Class2LuaScraper::scrapeCXXRecordDeclContext(clang::DeclContext *DeclContext, RecordInfo& Out) {
  for (DeclContext::decl_iterator itr = DeclContext->decls_begin();
       itr != DeclContext->decls_end(); itr++) {
    FieldDecl *Decl = clang::dyn_cast<FieldDecl>(*itr);
    if (Decl == nullptr) {
      continue;
    }

    scrapeFieldDecl(Decl, Out);
  }
}

void Class2LuaScraper::scrapeCXXRecordDecl(clang::CXXRecordDecl *decl) {
  if (decl->isThisDeclarationADefinition() == VarDecl::DeclarationOnly) {
    return;
  }

  RecordInfo Info;
  Info.RecordTypeName = decl->getQualifiedNameAsString();

  const clang::Type *type = decl->getTypeForDecl();
  llvm::outs() << "CXXRecordDecl " << decl->getQualifiedNameAsString()
               << " typeClass:" << type->getTypeClassName() << "\n";
  llvm::outs() << "{\n";
  scrapeCXXRecordDeclContext(decl, Info);
  Database.pushInfo(Info);
  llvm::outs() << "}\n";
}

void Class2LuaScraper::scrapeFieldDecl(clang::FieldDecl *decl, RecordInfo& Out) {
  QualType qual_type = decl->getType(); // from ValueDecl::getType()
  const clang::Type *type = qual_type.getTypePtr();
  QualType CanonicalQT = type->getCanonicalTypeInternal();
  const clang::Type *CanonicalType = CanonicalQT.getTypePtr();

  scrapeSubType(qual_type);

  FieldTypeInfo Info;
  Info.Kind = static_cast<TypeKind>(type->getTypeClass());
  Info.VarName = decl->getNameAsString();
  Info.VarTypeName = QualType::getAsString(qual_type.split(), Policy);
  Out.TypeInfos.push_back(Info);

  llvm::outs() << "FieldDecl " << decl->getNameAsString()
               << " typeClass:(" << type->getTypeClass() << ","
               << type->getTypeClassName()
               << " type:" << QualType::getAsString(qual_type.split(), Policy)
               << ") CanonicalType:(" << CanonicalType->getTypeClass() << ","
               << CanonicalType->getTypeClassName()
               << " type:" << QualType::getAsString(CanonicalQT.split(), Policy)
               << ")\n";
}

void Class2LuaScraper::scrapeSubType(QualType QT) {
  const clang::Type *type = QT.getTypePtr();
  switch (type->getTypeClass())
  {
  case clang::Type::TypeClass::Record:
    {
      scrapeCXXRecordDecl(type->getAsCXXRecordDecl());
    }
    break;
  case clang::Type::TypeClass::TemplateSpecialization:
    {
      const clang::TemplateSpecializationType *TST = type->getAs<clang::TemplateSpecializationType>();
      for (clang::TemplateSpecializationType::iterator itr = TST->begin();
       itr != TST->end(); itr++) {
         if (itr->getKind() != clang::TemplateArgument::Type) {
           continue;
         }

         QualType TemplateArgQT = itr->getAsType();
         scrapeSubType(TemplateArgQT);
       }
    }
    break;
  case clang::Type::TypeClass::Typedef:
    {
      const clang::TypedefType *TT = type->getAs<clang::TypedefType>();
      QualType DesugarQT = TT->desugar();
      scrapeSubType(DesugarQT);
    }
    break;
  case clang::Type::TypeClass::Elaborated:
    {
      const clang::ElaboratedType *ET = type->getAs<clang::ElaboratedType>();
      QualType DesugarQT = ET->desugar();
      scrapeSubType(DesugarQT);
    }
    break;
  default:
    break;
  }
}
