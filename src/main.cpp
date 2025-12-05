#include <iostream>

#include "Tokenizer.h"

int main(int argc, char** argv)
{
    std::string lsText(argv[1]);
    Tokenizer lcTokenizer(lsText);
    lcTokenizer.tokenize();

    std::cout << lcTokenizer.tokens()->size() << " tokens:" << std::endl;

    for (const Token* pToken : *lcTokenizer.tokens())
    {
        std::cout << "    " << pToken->toString() << std::endl;
    }
}
