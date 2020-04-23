set(LLVM_LINK_COMPONENTS support)

add_clang_executable(cpp_hproto
  cpp_hproto.cpp
  Class2LuaTool.h
  Class2LuaTool.cpp
  Class2LuaScraper.h
  Class2LuaScraper.cpp
  RecordsDatabase.h
  RecordsDatabase.cpp
  Class2LuaCodeGenerator.h
  Class2LuaCodeGenerator.cpp
  ComplexFieldTypeParse.h
  ComplexFieldTypeParse.cpp
)

target_link_libraries(cpp_hproto
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)
