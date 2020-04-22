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

enum ComplexType
{
    CT_Array,   // vector, set, list, queue etc. use ChildVarTypeName1
    CT_Map,     // use ChildVarTypeName2
};

struct ComplexTypeInfo
{
    ComplexType Type;
    std::string VarTypeName;
    std::string ChildVarTypeName1;
    std::string ChildVarTypeName2;
};

struct ComplexTypeInfoCmp {
bool operator() (const ComplexTypeInfo& lhs, const ComplexTypeInfo& rhs) {
        if (lhs.Type != rhs.Type)
            return lhs.Type < rhs.Type;
        if (lhs.VarTypeName != rhs.VarTypeName)
            return lhs.VarTypeName < rhs.VarTypeName;

        return false;
}
};


struct FieldTypeInfo
{
    FieldTypeInfo() : Kind(TK_Unknown) {}

    TypeKind Kind;
    std::string VarName;
    std::string VarTypeName;

    //TODO move logic to Class2LuaCodeGenerator
    std::vector<ComplexTypeInfo> ComplexTypeInfos;
};

struct RecordInfo // Record: class, struct
{
    RecordInfo() : IsRoot(false) {}

    std::string RecordTypeName;
    bool IsRoot;
    std::vector<FieldTypeInfo> TypeInfos;
};

class RecordsDatabase
{
public:
    RecordsDatabase() {}

    void pushInfo(const RecordInfo& Info);
    std::vector<RecordInfo> getAll() const;
    std::vector<RecordInfo>::const_iterator begin() const { return RecordInfos.begin(); }
    std::vector<RecordInfo>::const_iterator end() const { return RecordInfos.end(); }
    std::vector<RecordInfo>::iterator begin() { return RecordInfos.begin(); }
    std::vector<RecordInfo>::iterator end() { return RecordInfos.end(); }

    void dump() const;
    bool hasRecord(const std::string& RecordTypeName) const;

private:
    std::set<std::string> RecordTypeNames;
    std::vector<RecordInfo> RecordInfos;
};
