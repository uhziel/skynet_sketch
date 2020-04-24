#include "ComplexFieldTypeParse.h"
#include "RecordsDatabase.h"
#include <regex>

enum AtomKind {
  AK_Array,
  AK_Map,
  AK_Record,
};

struct ComplexAtomInfo {
public:
  ComplexAtomInfo(AtomKind Kind)
    : Kind(Kind)
  {}
  ComplexAtomInfo(AtomKind Kind, const std::string& RecordName)
    : Kind(Kind)
    , RecordName(RecordName)
  {}

  AtomKind Kind;
  std::string RecordName;
};

static void _SpiltTemplateParam(const std::string &TemplateParamStr,
  std::vector<std::string>& TemplateParams)
{
  int LeftArrayNum = 0;
  std::string::const_iterator LastStartCitr = TemplateParamStr.begin();
  for (std::string::const_iterator citr = TemplateParamStr.begin();
    citr != TemplateParamStr.end(); citr++)
  {
    if (*citr == '<') {
      LeftArrayNum += 1;
    } else if (*citr == '>') {
      LeftArrayNum -= 1;
    }
    
    if (LeftArrayNum == 0 && *citr == ',') {
      std::string TemplateParam(LastStartCitr, citr);
      TemplateParams.push_back(TemplateParam);
      if (citr+1 == TemplateParamStr.end()) {
        return;
      } else {
        LastStartCitr = citr + 1;
      }
    }
  }
}

static void _ComplexVarTypeNameParse(const std::string &VarTypeName,
  std::vector<ComplexAtomInfo> &Out) {
  size_t LeftArrowPos = VarTypeName.find_first_of("<");
  size_t RightArrowPos = VarTypeName.find_last_of(">");
  if (LeftArrowPos == std::string::npos || RightArrowPos == std::string::npos) {
    ComplexAtomInfo Info(AK_Record);
    Info.RecordName = std::regex_replace(VarTypeName, std::regex(" "), "_");
    Out.push_back(Info);
    return;
  }

  std::string Header = VarTypeName.substr(0, LeftArrowPos);
  if (Header.find("vector") != std::string::npos ||
    Header.find("set") != std::string::npos ||
    Header.find("queue") != std::string::npos) {
    std::string TemplateParamStr = VarTypeName.substr(LeftArrowPos+1, RightArrowPos-LeftArrowPos-1);
    std::vector<std::string> TemplateParams;
    _SpiltTemplateParam(TemplateParamStr, TemplateParams);
    if (!TemplateParams.empty()) {
      ComplexAtomInfo Info(AK_Array, "array");
      Out.push_back(Info);
      _ComplexVarTypeNameParse(TemplateParams[0], Out);
      return;       
    }
  }
  //TODO else if map

  ComplexAtomInfo Info(AK_Record);
  Info.RecordName = std::regex_replace(VarTypeName, std::regex(" "), "_");
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
    Info.ChildVarTypeName1 = VarTypeNameIndex[i+1];
    Out.ComplexTypeInfos.push_back(Info);
  }
  
  if (!Out.ComplexTypeInfos.empty()) {
    Out.VarTypeName = Out.ComplexTypeInfos[0].VarTypeName;
  }
  std::reverse(Out.ComplexTypeInfos.begin(), Out.ComplexTypeInfos.end());
}
