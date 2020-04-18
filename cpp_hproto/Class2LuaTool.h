#pragma once

#include "clang/Tooling/Tooling.h"

class Class2LuaScraper;

class Class2LuaTool {
public:
  Class2LuaTool(const clang::tooling::CompilationDatabase &Compilations,
                llvm::ArrayRef<std::string> SourcePaths);
  void Run(Class2LuaScraper *Scraper);

private:
  clang::tooling::ClangTool Tool;
};
