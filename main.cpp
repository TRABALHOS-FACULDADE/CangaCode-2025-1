#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <algorithm>
#include "symbolTable.cpp"

class TypeContext
{
public:
    enum class Context
    {
        GLOBAL,
        DECLARATIONS,
        FUNCTIONS,
        FUNCTION_PARAMS,
        VARIABLE_DECL,
        ARRAY_DECL,
        FUNCTION_DECL
    };

    TypeContext()
    {
        contextStack_.push(Context::GLOBAL);
    }

    void pushContext(Context ctx)
    {
        contextStack_.push(ctx);
    }

    void popContext()
    {
        if (contextStack_.size() > 1)
        {
            contextStack_.pop();
        }
    }

    Context currentContext() const
    {
        return contextStack_.top();
    }

    static std::string mapTypeToCode(TokenType type, bool isArray = false)
    {
        if (isArray)
        {
            switch (type)
            {
            case TokenType::REAL:
                return "AF";
            case TokenType::INTEGER:
                return "AI";
            case TokenType::STRING:
                return "AS";
            case TokenType::CHARACTER:
                return "AC";
            case TokenType::BOOLEAN:
                return "AB";
            default:
                return "";
            }
        }
        else
        {
            switch (type)
            {
            case TokenType::REAL:
                return "FP";
            case TokenType::INTEGER:
                return "IN";
            case TokenType::STRING:
                return "ST";
            case TokenType::CHARACTER:
                return "CH";
            case TokenType::BOOLEAN:
                return "BL";
            case TokenType::VOID:
                return "VD";
            default:
                return "";
            }
        }
    }

private:
    std::stack<Context> contextStack_;
};

void _teamHeader(std::ofstream &stream)
{
    stream << "Código da Equipe: 2" << std::endl
           << "Componentes:" << std::endl
           << "    Davih de Andrade Machado Borges Santos; _@aln.senaicimatec.edu.br" << std::endl
           << "    Felipe Azevedo Ribeiro; _@aln.senaicimatec.edu.br" << std::endl
           << "    Gabriel Trindade Santana; _@aln.senaicimatec.edu.br" << std::endl
           << "    Pedro Quadros de Freitas; _@aln.senaicimatec.edu.br" << std::endl
           << std::endl;
}

void _generateLexFile(std::string base, std::vector<LexemeRecord> lexemes)
{
    std::ofstream lexOut(base + ".LEX");

    _teamHeader(lexOut);

    for (auto &r : lexemes)
    {
        lexOut
            << "Lexeme: " << r.lexeme << ", Código: "
            << (SymbolTable::tokenTypeToString(r.type)) << ", ÍndiceTabSimb: "
            << (r.tableIndex > 0 ? std::to_string(r.tableIndex) : "-") << ", Linha: "
            << r.line << ".\n";
    }
    lexOut.close();
}

void _generateTabFile(std::string base, SymbolTable symtab)
{
    std::ofstream tabOut(base + ".TAB");

    std::vector<std::pair<std::string, SymbolTable::SymbolInfo>> syms;
    for (auto &p : symtab.all())
        syms.push_back(p);
    std::sort(syms.begin(), syms.end(),
              [](auto &a, auto &b)
              { return a.second.entry < b.second.entry; });

    _teamHeader(tabOut);

    for (size_t i = 0; i < syms.size(); ++i)
    {
        auto &p = syms[i];
        auto &info = p.second;
        tabOut
            << "Entrada: " << info.entry << ", Codigo: "
            << info.atomCode << ", Lexeme: " << info.lexeme << ",\n"
            << "QtdCharAntesTrunc: " << info.lenBefore << ", QtdCharDepoisTrunc: "
            << info.lenAfter << ",\n"
            << "TipoSimb: " << info.type << ", Linhas: {";
        for (size_t j = 0; j < info.lines.size(); ++j)
        {
            if (j)
                tabOut << ", ";
            tabOut << info.lines[j];
        }
        tabOut << "}.\n";
        if (i < syms.size() - 1)
        {
            tabOut << "----------------------------------------------------------------------------------------------------\n";
        }
    }
    tabOut.close();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Use: ./CangaCompiler <file_name>.251\n";
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
    SymbolTable symtab;
    TypeContext typeContext;
    std::vector<LexemeRecord> lexemes;

    TokenType currentType = TokenType::VOID;
    bool isArray = false;
    std::string lastIdentifier;

    while (true)
    {
        Token tok = lexer.nextToken();
        if (tok.type == TokenType::END_OF_FILE)
            break;

        switch (tok.type)
        {
        case TokenType::PROGRAM:
            typeContext.pushContext(TypeContext::Context::GLOBAL);
            break;
        case TokenType::DECLARATIONS:
            typeContext.pushContext(TypeContext::Context::DECLARATIONS);
            break;
        case TokenType::ENDDECLARATIONS:
            typeContext.popContext();
            break;
        case TokenType::FUNCTIONS:
            typeContext.pushContext(TypeContext::Context::FUNCTIONS);
            break;
        case TokenType::ENDFUNCTIONS:
            typeContext.popContext();
            break;
        case TokenType::VAR:
            typeContext.pushContext(TypeContext::Context::VARIABLE_DECL);
            break;
        case TokenType::FUNCTYPE:
            typeContext.pushContext(TypeContext::Context::FUNCTION_DECL);
            break;
        case TokenType::LPAREN:
            if (typeContext.currentContext() == TypeContext::Context::FUNCTION_DECL)
            {
                typeContext.pushContext(TypeContext::Context::FUNCTION_PARAMS);
            }
            break;
        case TokenType::RPAREN:
            if (typeContext.currentContext() == TypeContext::Context::FUNCTION_PARAMS)
            {
                typeContext.popContext();
            }
            break;
        case TokenType::LBRACK:
            isArray = true;
            break;
        case TokenType::RBRACK:
            isArray = false;
            break;
        case TokenType::SEMI:
            if (typeContext.currentContext() == TypeContext::Context::VARIABLE_DECL ||
                typeContext.currentContext() == TypeContext::Context::FUNCTION_DECL)
            {
                typeContext.popContext();
            }
            break;
        case TokenType::REAL:
        case TokenType::INTEGER:
        case TokenType::STRING:
        case TokenType::BOOLEAN:
        case TokenType::CHARACTER:
        case TokenType::VOID:
            currentType = tok.type;
            break;

        case TokenType::IDENT:
            lastIdentifier = tok.lexeme;
            int idx = symtab.defineOrGet(tok.lexeme, tok.line, TokenType::IDENT);

            Token nextTok = lexer.nextToken();
            if (nextTok.type == TokenType::LPAREN)
            {
                while (true)
                {
                    Token callTok = lexer.nextToken();
                    if (callTok.type == TokenType::RPAREN)
                    {
                        break;
                    }
                }
                lexer.nextToken();
            }
            else
            {
                std::string typeCode;
                switch (typeContext.currentContext())
                {
                case TypeContext::Context::GLOBAL:
                    typeCode = "VD";
                    break;
                case TypeContext::Context::VARIABLE_DECL:
                case TypeContext::Context::FUNCTION_PARAMS:
                    typeCode = TypeContext::mapTypeToCode(currentType, isArray);
                    break;
                case TypeContext::Context::FUNCTION_DECL:
                    typeCode = TypeContext::mapTypeToCode(currentType, false);
                    break;
                default:
                    break;
                }

                if (!typeCode.empty())
                {
                    symtab.setType(tok.lexeme, typeCode);
                }
            }

            lexemes.push_back({tok.lexeme, tok.type, idx, tok.line});
            continue;
        }

        if (tok.type != TokenType::IDENT)
        {
            lexemes.push_back({tok.lexeme, tok.type, -1, tok.line});
        }
    }

    // >>>>>> Geração dos arquivos .LEX e .TAB <<<<<<
    auto posdot = filename.find_last_of('.');
    std::string base = (posdot == std::string::npos
                            ? filename
                            : filename.substr(0, posdot));

    _generateLexFile(filename, lexemes);
    _generateTabFile(filename, symtab);

    std::cout << "Arquivos gerados: " << base << ".LEX e " << base << ".TAB\n";
    return 0;
}