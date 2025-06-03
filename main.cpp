#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.cpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Uso: ./CangaCompiler teste.251\n";
        return 1;
    }
    std::string filename = argv[1];
    std::ifstream ifs(filename);
    if (!ifs)
    {
        std::cerr << "Erro ao abrir arquivo: " << filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);

    TokenType currentType = TokenType::VOID;
    bool isArray = false;
    std::string lastIdentifier;

    while (true)
    {
        Token tok = lexer.nextToken();
        if (tok.type == TokenType::END_OF_FILE)
            break;

        if (tok.type != TokenType::IDENT)
        {
            //
        }
    }

    // >>>>>> Geração dos arquivos .LEX e .TAB <<<<<<
    auto posdot = filename.find_last_of('.');
    std::string base = (posdot == std::string::npos
                            ? filename
                            : filename.substr(0, posdot));
    std::ofstream lexOut(base + ".LEX");

    lexOut << "LEX report for " << filename << "\n";
    lexOut << "Lexeme\tAtomCode\tTableIdx\tLine\n";

    // Preenchimento do arquivo .LEX

    lexOut.close();

    std::ofstream tabOut(base + ".TAB");
    tabOut << "Entry\tAtomCode\tLexeme\tLenBefore\tLenAfter\tType\tLines\n";

    // Preenchimento do arquivo .TAB

    tabOut.close();

    std::cout << "Arquivos gerados: " << base << ".LEX e " << base << ".TAB\n";
    return 0;
}