#pragma once

#include <vector>
#include <string>
#include <set>

enum TypeKind
{
    TK_Unknown,
    TK_Builtin, // int, float, bool etc
    TK_String,
    TK_Complex,
};

struct FieldTypeInfo
{
    FieldTypeInfo() : Kind(TK_Unknown) {}

    TypeKind Kind;
    std::string VarName;
    std::string VarTypeName;
};

struct RecordInfo // Record: class, struct
{
    std::string RecordTypeName;
    std::vector<FieldTypeInfo> TypeInfos;
};

class RecordsDatabase
{
public:
    RecordsDatabase() {}

    void pushInfo(const RecordInfo& Info);
    std::vector<RecordInfo> getAll() const;
    void dump() const;
    bool hasRecord(const std::string& RecordTypeName) const;

private:
    std::set<std::string> RecordTypeNames;
    std::vector<RecordInfo> RecordInfos;
};
