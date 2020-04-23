#pragma once

#include "RecordsDatabase.h"
#include <set>

class Class2LuaCodeGenerator {
public:
  Class2LuaCodeGenerator(const RecordsDatabase& Database) : Database(Database) {}

  void generate();

private:
  void preprocessRecord(RecordInfo& RecordInfo);
  void preprecoessComplexField(FieldTypeInfo& FieldTypeInfo);
  void generateRecord(const RecordInfo& RecordInfo);
  void generateRecordLencode(const RecordInfo& RecordInfo);
  void generateComplexFieldLencode(const FieldTypeInfo& FieldTypeInfo);
  void generateComplexTypeInfoLencode(const ComplexTypeInfo& ComplexTypeInfo);
  void generateRecordLdecode(const RecordInfo& RecordInfo);
  void generateComplexFieldLdecode(const FieldTypeInfo& FieldTypeInfo);
  void generateComplexTypeInfoLdecode(const ComplexTypeInfo& ComplexTypeInfo);

private:
  RecordsDatabase Database;
  std::set<ComplexTypeInfo, ComplexTypeInfoCmp> GenedLencodeComplexTypeInfos;
  std::set<ComplexTypeInfo, ComplexTypeInfoCmp> GenedLdecodeComplexTypeInfos;
};
