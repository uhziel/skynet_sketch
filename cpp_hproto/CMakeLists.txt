set(LLVM_LINK_COMPONENTS support)

add_clang_executable(cpp_hproto
  cpp_hproto.cpp
)

target_link_libraries(cpp_hproto
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)
