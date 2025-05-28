#include <iostream>
#include <fstream>

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

    return 0;
}