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

private:
    std::stack<Context> contextStack_;
};

void _teamHeader(std::ofstream &stream)
{
    stream << "Código da Equipe: 2" << std::endl
           << "Componentes:" << std::endl
           << "    Davih de Andrade Machado Borges Santos; _@aln.senaicimatec.edu.br" << std::endl
           << "    Felipe Azevedo Ribeiro; felipe.r@aln.senaicimatec.edu.br" << std::endl
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
        case TokenType::VARTYPE:
            typeContext.pushContext(TypeContext::Context::VARIABLE_DECL);
            break;
        case TokenType::FUNCTYPE:
            typeContext.pushContext(TypeContext::Context::FUNCTION_DECL);
            {
                Token nextTok;
                do {
                    nextTok = lexer.nextToken();
                } while (nextTok.type != TokenType::LPAREN && nextTok.type != TokenType::END_OF_FILE && nextTok.type != TokenType::ENDFUNCTIONS && nextTok.type != TokenType::LBRACE);
                if (nextTok.type == TokenType::LPAREN) {
                    
                    while (true) {
                        Token paramTok = lexer.nextToken();
                        if (paramTok.type == TokenType::RPAREN) break;
                       
                        if (paramTok.type == TokenType::IDENT) {
                            int idx = symtab.defineOrGet(paramTok.lexeme, paramTok.line, TokenType::IDENT);
                            LexemeRecord paramRecord;
                            paramRecord.type = paramTok.type;
                            paramRecord.lexeme = paramTok.lexeme;
                            paramRecord.tableIndex = symtab.getIndex(paramTok.lexeme);
                            paramRecord.line = paramTok.line;
                            lexemes.push_back(paramRecord);
                        }
                        
                        else if (paramTok.type != TokenType::RPAREN) {
                            LexemeRecord paramRecord;
                            paramRecord.type = paramTok.type;
                            paramRecord.lexeme = paramTok.lexeme;
                            paramRecord.tableIndex = -1;
                            paramRecord.line = paramTok.line;
                            lexemes.push_back(paramRecord);
                        }
                        if (paramTok.type == TokenType::END_OF_FILE || paramTok.type == TokenType::ENDFUNCTIONS) {
                            throw std::runtime_error("Erro: fim inesperado ao processar parametros da funcao (linha " + std::to_string(tok.line) + ")");
                        }
                    }
                    
                    nextTok = lexer.nextToken();
                }

                while (nextTok.type != TokenType::LBRACE && nextTok.type != TokenType::END_OF_FILE && nextTok.type != TokenType::ENDFUNCTIONS) {
                    
                    LexemeRecord skippedRecord;
                    skippedRecord.type = nextTok.type;
                    skippedRecord.lexeme = nextTok.lexeme;
                    skippedRecord.tableIndex = -1;
                    skippedRecord.line = nextTok.line;
                    lexemes.push_back(skippedRecord);
                    nextTok = lexer.nextToken();
                }
                if (nextTok.type != TokenType::LBRACE) {
                    throw std::runtime_error("Erro: funcao nao possui corpo iniciado por '{' (linha " + std::to_string(tok.line) + ")");
                }
                
                int braceCount = 1;
                while (braceCount > 0) {
                    Token bodyTok = lexer.nextToken();
                    if (bodyTok.type == TokenType::END_OF_FILE) {
                        throw std::runtime_error("Erro: funcao nao termina com '}' (linha " + std::to_string(tok.line) + ")");
                    }
                    if (bodyTok.type == TokenType::LBRACE) braceCount++;
                    if (bodyTok.type == TokenType::RBRACE) braceCount--;
                    if (bodyTok.type == TokenType::ENDFUNCTIONS && braceCount > 0) {
                        throw std::runtime_error("Erro: funcao nao termina com '}' antes de ENDFUNCTIONS (linha " + std::to_string(tok.line) + ")");
                    }
                }
                
                typeContext.popContext();
                break;
            }
        case TokenType::PARAMTYPE:
            if (typeContext.currentContext() == TypeContext::Context::FUNCTION_PARAMS)
            {
                typeContext.pushContext(TypeContext::Context::VARIABLE_DECL);
            }
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
        case TokenType::COLON:
            if (typeContext.currentContext() == TypeContext::Context::VARIABLE_DECL)
            {
                break;
            }
            break;
        case TokenType::COMMA:
            if (typeContext.currentContext() == TypeContext::Context::VARIABLE_DECL)
            {
                break;
            }
            break;
        case TokenType::REAL:
        case TokenType::INTEGER:
        case TokenType::STRING:
        case TokenType::BOOLEAN:
        case TokenType::CHARACTER:
        case TokenType::VOID:
            currentType = tok.type;
            {
                if (currentType != TokenType::VOID) {
                    Token lookahead = lexer.nextToken();
                    if (lookahead.type == TokenType::LBRACK) {
                        Token lookahead2 = lexer.nextToken();
                        if (lookahead2.type == TokenType::RBRACK) {
                            isArray = true;
                        } else {
                            lexer.putBackToken(lookahead2);
                            lexer.putBackToken(lookahead);
                            isArray = false;
                        }
                    } else {
                        lexer.putBackToken(lookahead);
                        isArray = false;
                    }
                }
            }
            break;
        case TokenType::TRUE:
        case TokenType::FALSE:
            break;
        case TokenType::IDENT:
            lastIdentifier = tok.lexeme;
            int idx = symtab.defineOrGet(tok.lexeme, tok.line, TokenType::IDENT);

            Token nextTok = lexer.nextToken();
            if (nextTok.type == TokenType::LBRACK)
            {
                Token sizeTok = lexer.nextToken();
                if (sizeTok.type != TokenType::INTCONST)
                {
                    throw std::runtime_error("Erro na linha " + std::to_string(tok.line) +
                                           ": Tamanho do array deve ser uma constante inteira");
                }
                Token rbrack = lexer.nextToken();
                if (rbrack.type != TokenType::RBRACK)
                {
                    throw std::runtime_error("Erro na linha " + std::to_string(tok.line) +
                                           ": Esperava ']' apos tamanho do array");
                }
            }
            else
            {
                lexer.putBackToken(nextTok);
            }

            if (typeContext.currentContext() == TypeContext::Context::VARIABLE_DECL)
            {
                std::string typeCode;
                if (isArray)
                {
                    switch (currentType)
                    {
                    case TokenType::REAL:
                        typeCode = "AF";
                        break;
                    case TokenType::INTEGER:
                        typeCode = "AI";
                        break;
                    case TokenType::STRING:
                        typeCode = "AS";
                        break;
                    case TokenType::CHARACTER:
                        typeCode = "AC";
                        break;
                    case TokenType::BOOLEAN:
                        typeCode = "AB";
                        break;
                    default:
                        typeCode = "VD";
                    }
                }
                else
                {
                    switch (currentType)
                    {
                    case TokenType::REAL:
                        typeCode = "FP";
                        break;
                    case TokenType::INTEGER:
                        typeCode = "IN";
                        break;
                    case TokenType::STRING:
                        typeCode = "ST";
                        break;
                    case TokenType::CHARACTER:
                        typeCode = "CH";
                        break;
                    case TokenType::BOOLEAN:
                        typeCode = "BL";
                        break;
                    case TokenType::VOID:
                        typeCode = "VD";
                        break;
                    default:
                        typeCode = "VD";
                    }
                }
                symtab.setType(tok.lexeme, typeCode);
            }
            break;
        }

        LexemeRecord record;
        record.type = tok.type;
        record.lexeme = tok.lexeme;
        record.tableIndex = symtab.getIndex(tok.lexeme);
        record.line = tok.line;
        lexemes.push_back(record);
    }

    _generateLexFile(filename.substr(0, filename.find_last_of('.')), lexemes);
    _generateTabFile(filename.substr(0, filename.find_last_of('.')), symtab);

    std::cout << "Arquivos gerados: " << filename.substr(0, filename.find_last_of('.')) << ".LEX e " << filename.substr(0, filename.find_last_of('.')) << ".TAB\n";
    return 0;
}