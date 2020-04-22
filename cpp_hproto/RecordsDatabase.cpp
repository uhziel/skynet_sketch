#include "RecordsDatabase.h"
#include "clang/Tooling/Tooling.h"

void RecordsDatabase::pushInfo(const RecordInfo &Info) {
  if (hasRecord(Info.RecordTypeName)) {
    return;
  }
  RecordTypeNames.insert(Info.RecordTypeName);
  RecordInfos.push_back(Info);
}

std::vector<RecordInfo> RecordsDatabase::getAll() const { return RecordInfos; }

void RecordsDatabase::dump() const {
  for (std::vector<RecordInfo>::const_iterator citr = RecordInfos.begin();
       citr != RecordInfos.end(); citr++) {
    llvm::outs() << "RecordTypeName:" << citr->RecordTypeName
                 << " IsRoot:" << citr->IsRoot << "\n";
    for (std::vector<FieldTypeInfo>::const_iterator fcitr =
             citr->TypeInfos.begin();
         fcitr != citr->TypeInfos.end(); fcitr++) {
      llvm::outs() << "Kind:" << fcitr->Kind << " VarName:" << fcitr->VarName
                   << " VarTypeName:" << fcitr->VarTypeName << "\n";
    }
  }
}

bool RecordsDatabase::hasRecord(const std::string &RecordTypeName) const {
  return RecordTypeNames.find(RecordTypeName) != RecordTypeNames.end();
}
