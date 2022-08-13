#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace clang::tooling;

/*
/home/sam/llvm/llvm-project/build/bin/clang++ eg1.cpp -I/home/sam/llvm/llvm-project/clang/include -I/home/sam/llvm/llvm-project/build/tools/clang/include $(/home/sam/llvm/llvm-project/build/bin/llvm-config --cxxflags) $(/home/sam/llvm/llvm-project/build/bin/llvm-config --ldflags --libs --system-libs) -lclangAST -lclangASTMatchers -lclangAnalysis -lclangBasic -lclangDriver -lclangEdit -lclangFrontend -lclangFrontendTool -lclangLex -lclangParse -lclangSema -lclangEdit -lclangRewrite -lclangRewriteFrontend -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangSerialization -lclangToolingCore -lclangTooling -lclangFormat

LD_LIBRARY_PATH=/home/sam/llvm/llvm-project/build/lib ./a.out test.cpp --
*/

class FindNamedCallVisitor : public RecursiveASTVisitor<FindNamedCallVisitor> {
 public:
  explicit FindNamedCallVisitor(ASTContext *Context, std::string fName)
      : Context(Context), fName(fName) {}

  bool VisitCallExpr(CallExpr *CallExpression) {
    QualType q = CallExpression->getType();
    const Type *t = q.getTypePtrOrNull();

    if (t != NULL) {
      FunctionDecl *func = CallExpression->getDirectCallee();
      const std::string funcName = func->getNameInfo().getAsString();
      if (fName == funcName) {
        FullSourceLoc FullLocation =
            Context->getFullLoc(CallExpression->getBeginLoc());
        if (FullLocation.isValid())
          llvm::outs() << "Found call at "
                       << FullLocation.getSpellingLineNumber() << ":"
                       << FullLocation.getSpellingColumnNumber() << "\n";
      }
    }

    return true;
  }

 private:
  ASTContext *Context;
  std::string fName;
};

class FindNamedCallConsumer : public clang::ASTConsumer {
 public:
  explicit FindNamedCallConsumer(ASTContext *Context, std::string fName)
      : Visitor(Context, fName) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

 private:
  FindNamedCallVisitor Visitor;
};

class FindNamedCallAction : public clang::ASTFrontendAction {
 public:
  FindNamedCallAction() {}

  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    const std::string fName = "doSomething";
    return std::unique_ptr<clang::ASTConsumer>(
        new FindNamedCallConsumer(&Compiler.getASTContext(), fName));
  }
};

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
  const std::string fName = "doSomething";

  auto OptionsParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!OptionsParser)
  {
    return -1;
  }

  ClangTool Tool(OptionsParser->getCompilations(),
                 OptionsParser->getSourcePathList());

  // run the Clang Tool, creating a new FrontendAction (explained below)
  int result = Tool.run(newFrontendActionFactory<FindNamedCallAction>().get());
  return result;
}
