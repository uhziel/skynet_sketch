#pragma once

#include <vector>
#include <string>

enum TypeKind
{
    TK_Int,
    TK_Short,
    TK_Long,
    TK_Bool,
    TK_Float,
    TK_Double,
    TK_Vector,
};

struct FieldTypeInfo
{
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

private:
    std::vector<RecordInfo> RecordInfos;
};
