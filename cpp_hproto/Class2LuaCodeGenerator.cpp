#include "Class2LuaCodeGenerator.h"
#include "RecordsDatabase.h"
#include "clang/Tooling/Tooling.h"
#include <regex>
#include "ComplexFieldTypeParse.h"

//FILE
static const char FILE_HEADER[] =
"#include \"cpp_hproto_utils.h\"\n"
"\n";

//RECORD_LENCODE
static const char RECORD_LENCODE_HEADER[] =
"static void\n"
"lencode_%s(lua_State *L, int index, Stream *stream) {\n";
static const char RECORD_LENCODE_FIELD[] =
"    lencode_%s(L, index, \"%s\", stream);\n";
static const char RECORD_LENCODE_FOOTER[] =
"}\n"
"\n";
static const char RECORD_LENCODE_NOT_ROOT_FOOTER[] =
"static void\n"
"lencode_%s(lua_State *L, int index, const char* key, Stream *stream) {\n"
"    lua_pushstring(L, key);\n"
"    lua_gettable(L, index);\n"
"\n"
"    lencode_%s(L, index, stream);\n"
"\n"
"    lua_pop(L, 1);\n"
"}\n"
"\n";

//RECORD_LDECODE
static const char RECORD_LDECODE_HEADER[] =
"static void\n"
"ldecode_%s(lua_State *L, ReadStream *stream) {\n";
static const char RECORD_LDECODE_NOT_ROOT_HEADER[] =
"    lua_newtable(L);\n"
"\n";
static const char RECORD_LDECODE_FIELD[] =
"    ldecode_%s(L, \"%s\", stream);\n";
static const char RECORD_LDECODE_FOOTER[] =
"}\n"
"\n";
static const char RECORD_LDECODE_NOT_ROOT_FOOTER[] =
"static void\n"
"ldecode_%s(lua_State *L, const char* key, ReadStream *stream) {\n"
"    lua_pushstring(L, key);\n"
"\n"
"    ldecode_%s(L, stream);\n"
"\n"
"    lua_settable(L, -3);\n"
"}\n"
"\n";

//ARRAY_LENCODE
static const char ARRAY_LENCODE[] =
"static void\n"
"lencode_%s(lua_State *L, int index, const char* key, Stream *stream) {\n"
"    lua_pushstring(L, key);\n"
"    lua_gettable(L, index);\n"
"\n"
"    lua_Integer size = 0;\n"
"    if (lua_istable(L, -1))\n"
"    {\n"
"        lua_len(L, -1);\n"
"        size = lua_tointeger(L, -1);\n"
"        lua_pop(L, 1);\n"
"    }\n"
"\n"
"    stream->Copy(&size, sizeof(size));\n"
"    for (lua_Integer i = 1; i <= size; i++) {\n"
"        lua_pushinteger(L, i);\n"
"        lua_gettable(L, -2);\n"
"\n"
"        lencode_%s(L, lua_gettop(L), stream);\n"
"\n"
"        lua_pop(L, 1);\n"
"    }\n"
"\n"
"    lua_pop(L, 1);\n"
"}\n"
"\n";

//ARRAY_LDECODE
static const char ARRAY_LDECODE[] =
"static void\n"
"ldecode_%s(lua_State *L, const char* key, ReadStream *stream) {\n"
"    lua_Integer size = 0;\n"
"    stream->Read(&size, sizeof(size));\n"
"\n"
"    lua_pushstring(L, key);\n"
"    lua_createtable(L, size, 0);\n"
"    for (lua_Integer i = 1; i <= size; i++) {\n"
"        lua_pushinteger(L, i);\n"
"\n"
"        ldecode_%s(L, stream);\n"
"        \n"
"        lua_settable(L, -3);\n"
"    }    \n"
"\n"
"    lua_settable(L, -3);\n"
"}\n"
"\n";

void Class2LuaCodeGenerator::generate() {
  llvm::outs() << FILE_HEADER;

  for (std::vector<RecordInfo>::iterator itr = Database.begin();
       itr != Database.end(); itr++) {
    preprocessRecord(*itr);
    generateRecord(*itr);
  }
}

void Class2LuaCodeGenerator::preprocessRecord(RecordInfo& RecordInfo) {
  for (std::vector<FieldTypeInfo>::iterator itr = RecordInfo.TypeInfos.begin();
    itr != RecordInfo.TypeInfos.end(); itr++)
  {
    if (itr->Kind == TK_Builtin) {
      itr->VarTypeName = std::regex_replace(itr->VarTypeName, std::regex(" "), "_");
    } else if (itr->Kind == TK_Complex) {
      preprecoessComplexField(*itr);
    }
  }
}

void Class2LuaCodeGenerator::preprecoessComplexField(FieldTypeInfo& FieldTypeInfo) {
  ComplexFieldTypeParse(FieldTypeInfo);
}

void Class2LuaCodeGenerator::generateRecord(const RecordInfo& RecordInfo) {
  generateRecordLencode(RecordInfo);
  generateRecordLdecode(RecordInfo);
}

void Class2LuaCodeGenerator::generateRecordLencode(const RecordInfo& RecordInfo) {
  for (std::vector<FieldTypeInfo>::const_iterator citr = RecordInfo.TypeInfos.begin();
    citr != RecordInfo.TypeInfos.end(); citr++)
  {
    if (citr->Kind == TK_Complex) {
      generateComplexFieldLencode(*citr);
    }
  }

  llvm::outs() << llvm::format(RECORD_LENCODE_HEADER, RecordInfo.RecordTypeName.c_str());
  for (std::vector<FieldTypeInfo>::const_iterator citr = RecordInfo.TypeInfos.begin();
    citr != RecordInfo.TypeInfos.end(); citr++)
  {
    llvm::outs() << llvm::format(RECORD_LENCODE_FIELD, citr->VarTypeName.c_str(), citr->VarName.c_str());
  }
  llvm::outs() << RECORD_LENCODE_FOOTER;
  if (!RecordInfo.IsRoot) {
    llvm::outs() << llvm::format(RECORD_LENCODE_NOT_ROOT_FOOTER, RecordInfo.RecordTypeName.c_str(), RecordInfo.RecordTypeName.c_str());
  }
}

void Class2LuaCodeGenerator::generateComplexFieldLencode(const FieldTypeInfo& FieldTypeInfo) {
  for (std::vector<ComplexTypeInfo>::const_iterator citr = FieldTypeInfo.ComplexTypeInfos.begin();
    citr != FieldTypeInfo.ComplexTypeInfos.end(); citr++) {
      generateComplexTypeInfoLencode(*citr);
    }
}

void Class2LuaCodeGenerator::generateComplexTypeInfoLencode(const ComplexTypeInfo& ComplexTypeInfo) {
  if (GenedLencodeComplexTypeInfos.find(ComplexTypeInfo) != GenedLencodeComplexTypeInfos.end()) {
    return;
  }
  GenedLencodeComplexTypeInfos.insert(ComplexTypeInfo);

  if (ComplexTypeInfo.Type == CT_Array) {
    llvm::outs() << llvm::format(ARRAY_LENCODE, ComplexTypeInfo.VarTypeName.c_str(), ComplexTypeInfo.ChildVarTypeName1.c_str());
  }
}

void Class2LuaCodeGenerator::generateRecordLdecode(const RecordInfo& RecordInfo) {
  for (std::vector<FieldTypeInfo>::const_iterator citr = RecordInfo.TypeInfos.begin();
    citr != RecordInfo.TypeInfos.end(); citr++)
  {
    if (citr->Kind == TK_Complex) {
      generateComplexFieldLdecode(*citr);
    }
  }

  llvm::outs() << llvm::format(RECORD_LDECODE_HEADER, RecordInfo.RecordTypeName.c_str());
  if (!RecordInfo.IsRoot) {
    llvm::outs() << RECORD_LDECODE_NOT_ROOT_HEADER;
  }
  for (std::vector<FieldTypeInfo>::const_iterator citr = RecordInfo.TypeInfos.begin();
    citr != RecordInfo.TypeInfos.end(); citr++)
  {
    llvm::outs() << llvm::format(RECORD_LDECODE_FIELD, citr->VarTypeName.c_str(), citr->VarName.c_str());
  }
  llvm::outs() << RECORD_LDECODE_FOOTER;
  if (!RecordInfo.IsRoot) {
    llvm::outs() << llvm::format(RECORD_LDECODE_NOT_ROOT_FOOTER, RecordInfo.RecordTypeName.c_str(), RecordInfo.RecordTypeName.c_str());
  }
}

void Class2LuaCodeGenerator::generateComplexFieldLdecode(const FieldTypeInfo& FieldTypeInfo) {
  for (std::vector<ComplexTypeInfo>::const_iterator citr = FieldTypeInfo.ComplexTypeInfos.begin();
    citr != FieldTypeInfo.ComplexTypeInfos.end(); citr++) {
      generateComplexTypeInfoLdecode(*citr);
    }
}

void Class2LuaCodeGenerator::generateComplexTypeInfoLdecode(const ComplexTypeInfo& ComplexTypeInfo) {
  if (GenedLdecodeComplexTypeInfos.find(ComplexTypeInfo) != GenedLdecodeComplexTypeInfos.end()) {
    return;
  }
  GenedLdecodeComplexTypeInfos.insert(ComplexTypeInfo);

  if (ComplexTypeInfo.Type == CT_Array) {
    llvm::outs() << llvm::format(ARRAY_LDECODE, ComplexTypeInfo.VarTypeName.c_str(), ComplexTypeInfo.ChildVarTypeName1.c_str());
  }
}
