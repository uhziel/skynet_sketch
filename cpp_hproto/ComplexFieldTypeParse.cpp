#include "ComplexFieldTypeParse.h"
#include "RecordsDatabase.h"

enum AtomKind {
  AK_Array,
  AK_Map,
  AK_Record,
};

struct ComplexAtomInfo {
public:
  ComplexAtomInfo(AtomKind _Kind, const std::string& _RecordName)
    : Kind(_Kind)
    , RecordName(_RecordName)
  {}

  AtomKind Kind;
  std::string RecordName;
};

static void _ComplexVarTypeNameParse(const std::string &VarTypeName,
  std::vector<ComplexAtomInfo> &Out) {
  size_t LeftArrowPos = VarTypeName.find_first_of("<");
  size_t RightArrowPos = VarTypeName.find_last_of(">");
  if (LeftArrowPos == std::string::npos || RightArrowPos == std::string::npos) {
    ComplexAtomInfo Info(AK_Record, VarTypeName);
    Out.push_back(Info);
    return;
  }

  std::string Header = VarTypeName.substr(0, LeftArrowPos);
  if (Header.find("vector") != std::string::npos ||
    Header.find("set") != std::string::npos ||
    Header.find("queue") != std::string::npos) {
    size_t Pos = VarTypeName.find_first_of(",", LeftArrowPos);
    if (Pos != std::string::npos) {
      ComplexAtomInfo Info(AK_Array, "array");
      Out.push_back(Info);
      _ComplexVarTypeNameParse(VarTypeName.substr(LeftArrowPos+1, Pos-LeftArrowPos-1), Out);
      return;      
    }
  }
  //TODO else if map

  ComplexAtomInfo Info(AK_Record, VarTypeName);
  Out.push_back(Info);
  return;  
}

void ComplexFieldTypeParse(FieldTypeInfo &Out) {
  std::vector<ComplexAtomInfo> AtomInfos;
  _ComplexVarTypeNameParse(Out.VarTypeName, AtomInfos);

  std::vector<std::string> VarTypeNameIndex(AtomInfos.size());
  for (int i = static_cast<int>(AtomInfos.size()) - 1; i >= 0; i--)
  {
    if (i == static_cast<int>(AtomInfos.size()) - 1) {
      VarTypeNameIndex[i] = AtomInfos[i].RecordName;
    }
    else {
      VarTypeNameIndex[i] = AtomInfos[i].RecordName + "_" + VarTypeNameIndex[i+1];
    }
  }

  for (size_t i = 0; i < AtomInfos.size() - 1; i++) {
    ComplexTypeInfo Info;
    if (AtomInfos[i].Kind == AK_Array) {
      Info.Type = CT_Array;
    } else if (AtomInfos[i].Kind == AK_Map) {
      Info.Type = CT_Map;
    } else {
      continue;
    }
    Info.VarTypeName = VarTypeNameIndex[i];
    Info.ChildVarTypeName1 = VarTypeNameIndex[i-1];
    Out.ComplexTypeInfos.push_back(Info);
  }
  
  if (!Out.ComplexTypeInfos.empty()) {
    Out.VarTypeName = Out.ComplexTypeInfos[0].VarTypeName;
  }
}
