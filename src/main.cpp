#include <iostream>

#include "Tokenizer.h"

int main(int argc, char** argv)
{
    char c;
    std::string lsText;
    while (std::cin.get(c))
    {
        lsText += c;
    }

    Tokenizer lcTokenizer(lsText);
    lcTokenizer.tokenize();

    std::cout << lcTokenizer.tokens()->size() << " tokens:" << std::endl;

    for (const Token* pToken : *lcTokenizer.tokens())
    {
        std::cout << "    " << pToken->toString() << std::endl;
    }
}
