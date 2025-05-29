#include <iostream>
#include <fstream>
#include <sstream>
#include "token.cpp"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: ./CangaCompiler teste.251\n";
        return 1;
    }
    std::string filename = argv[1];
    std::ifstream ifs(filename);
    if (!ifs) {
        std::cerr << "Erro ao abrir arquivo: " << filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string source = buffer.str();
    
    TokenType currentType = TokenType::VOID;
    bool isArray = false;
    
    return 0;
}