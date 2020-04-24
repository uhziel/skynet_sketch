#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include "Class2LuaTool.h"
#include "Class2LuaScraper.h"
#include "Class2LuaCodeGenerator.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static cl::OptionCategory CppHprotoCategory("cpp_hproto options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, CppHprotoCategory);

  Class2LuaTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  auto Scraper = make_unique<Class2LuaScraper>(false);
  Tool.Run(Scraper.get());

  const RecordsDatabase& Database = Scraper->getDatabase();
  Class2LuaCodeGenerator CodeGenerator(Database);
  CodeGenerator.generate();
  return 0;
}
