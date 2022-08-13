// clang++ -fsyntax-only -fplugin=/home/fed/gist/clang_ast/eg_plugin/build/TernaryConverterPlugin.so -Xclang -plugin -Xclang ternary-converter test.cpp

int foo(int c){
    if (c > 10)
    {
        return c + 100;
    }
    else
    {
        return 94;
    }
}

int main()
{
    return foo(42);
}
