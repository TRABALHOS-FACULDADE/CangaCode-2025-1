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
           << "    Davih de Andrade Machado Borges Santos; davih.santos@ba.estudante.senai.br" << std::endl
           << "    Felipe Azevedo Ribeiro; felipe.r@aln.senaicimatec.edu.br" << std::endl
           << "    Gabriel Trindade Santana; gabriel.t.santana@ba.estudante.senai.br" << std::endl
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

// Lógica principal do compilador:
// - Leitura do arquivo fonte
// - Análise léxica e sintática
// - Preenchimento da tabela de símbolos
// - Geração dos arquivos .LEX e .TAB
int main(int argc, char *argv[])
{
    // Verifica se o nome do arquivo foi passado como argumento
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

    // Lê todo o conteúdo do arquivo fonte para uma string
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string source = buffer.str();

    // Inicializa o analisador léxico, tabela de símbolos e contexto de tipos
    Lexer lexer(source);
    SymbolTable symtab;
    TypeContext typeContext;
    std::vector<LexemeRecord> lexemes;

    TokenType currentType = TokenType::VOID;
    bool isArray = false;
    std::string lastIdentifier;

    // ===============================
    //  Loop principal de análise
    // ===============================
    // Lê tokens um a um e executa ações conforme o tipo do token
    while (true)
    {
        Token tok = lexer.nextToken();
        if (tok.type == TokenType::END_OF_FILE)
            break;

        // Switch principal para tratar cada tipo de token
        switch (tok.type)
        {
        // ====== Seções e Contextos ======
        case TokenType::PROGRAM:
            // Início do programa
            typeContext.pushContext(TypeContext::Context::GLOBAL);
            break;
        case TokenType::DECLARATIONS:
            // Início da seção de declarações
            typeContext.pushContext(TypeContext::Context::DECLARATIONS);
            break;
        case TokenType::ENDDECLARATIONS:
            // Fim da seção de declarações
            typeContext.popContext();
            break;
        case TokenType::FUNCTIONS:
            // Início da seção de funções
            typeContext.pushContext(TypeContext::Context::FUNCTIONS);
            break;
        case TokenType::ENDFUNCTIONS:
            // Fim da seção de funções
            typeContext.popContext();
            break;
        case TokenType::VARTYPE:
            // Início de declaração de variáveis
            typeContext.pushContext(TypeContext::Context::VARIABLE_DECL);
            break;
        // ====== Declaração de Função ======
        case TokenType::FUNCTYPE:
            typeContext.pushContext(TypeContext::Context::FUNCTION_DECL);
            {
                // Espera um tipo válido após FUNCTYPE
                Token typeTok = lexer.nextToken();
                if (typeTok.type != TokenType::REAL && typeTok.type != TokenType::INTEGER && typeTok.type != TokenType::STRING && typeTok.type != TokenType::BOOLEAN && typeTok.type != TokenType::CHARACTER && typeTok.type != TokenType::VOID) {
                    throw std::runtime_error("Erro: FUNCTYPE deve ser seguido de um tipo valido (linha " + std::to_string(tok.line) + ")");
                }
                // Espera ':' após o tipo
                Token colonTok = lexer.nextToken();
                if (colonTok.type != TokenType::COLON) {
                    throw std::runtime_error("Erro: Esperado ':' apos o tipo na declaracao de funcao (linha " + std::to_string(tok.line) + ")");
                }
                // Processa o nome da função, parâmetros e corpo
                Token nextTok = lexer.nextToken();
                do {
                    nextTok = lexer.nextToken();
                } while (nextTok.type != TokenType::LPAREN && nextTok.type != TokenType::END_OF_FILE && nextTok.type != TokenType::ENDFUNCTIONS && nextTok.type != TokenType::LBRACE);
                if (nextTok.type == TokenType::LPAREN) {
                    // Processa parâmetros seguindo a BNF: <Parameters> ::= <ParamTypeList> | "?"
                    Token paramStartTok = lexer.nextToken();
                    if (paramStartTok.type == TokenType::QUESTION) {
                        // Parâmetros vazios (?)
                        LexemeRecord paramRecord;
                        paramRecord.type = paramStartTok.type;
                        paramRecord.lexeme = paramStartTok.lexeme;
                        paramRecord.tableIndex = -1;
                        paramRecord.line = paramStartTok.line;
                        lexemes.push_back(paramRecord);
                    } else {
                        // Processa lista de parâmetros
                        lexer.putBackToken(paramStartTok);
                        
                        while (true) {
                            Token paramTok = lexer.nextToken();
                            if (paramTok.type == TokenType::RPAREN) {
                                lexer.putBackToken(paramTok);
                                break;
                            }
                            if (paramTok.type == TokenType::PARAMTYPE) {
                                // Espera tipo do parâmetro
                                Token paramTypeTok = lexer.nextToken();
                                TokenType paramType = paramTypeTok.type;
                                
                                // Verifica se é um tipo válido
                                if (paramTypeTok.type != TokenType::REAL && paramTypeTok.type != TokenType::INTEGER && 
                                    paramTypeTok.type != TokenType::STRING && paramTypeTok.type != TokenType::BOOLEAN && 
                                    paramTypeTok.type != TokenType::CHARACTER && paramTypeTok.type != TokenType::VOID) {
                                    throw std::runtime_error("Erro: Tipo invalido para parametro (linha " + std::to_string(paramTypeTok.line) + ")");
                                }
                               
                                // Espera ':'
                                Token paramColonTok = lexer.nextToken();
                                if (paramColonTok.type != TokenType::COLON) {
                                    throw std::runtime_error("Erro: Esperado ':' apos o tipo do parametro (linha " + std::to_string(paramTypeTok.line) + ")");
                                }
                                
                                // Processa lista de parâmetros
                                while (true) {
                                    Token paramIdentTok = lexer.nextToken();
                                    if (paramIdentTok.type == TokenType::IDENT) {
                                
                                        int idx = symtab.defineOrGet(paramIdentTok.lexeme, paramIdentTok.line, TokenType::IDENT);
                                        
                                        // Verifica se é array
                                        Token nextParamTok = lexer.nextToken();
                                        
                                        bool isParamArray = false;
                                        
                                        if (nextParamTok.type == TokenType::LBRACK) {
                                            Token sizeTok = lexer.nextToken();
                                            
                                            if (sizeTok.type != TokenType::INTCONST) {
                                                throw std::runtime_error("Erro: Tamanho do array deve ser constante inteira (linha " + std::to_string(sizeTok.line) + ")");
                                            }
                                            Token rbrack = lexer.nextToken();
                                            
                                            if (rbrack.type != TokenType::RBRACK) {
                                                throw std::runtime_error("Erro: Esperado ']' apos tamanho do array (linha " + std::to_string(sizeTok.line) + ")");
                                            }
                                            
                                            isParamArray = true;
                                        } else {
                                            lexer.putBackToken(nextParamTok);
                                        }
                                        
                                        // Define o tipo correto do parâmetro na tabela de símbolos
                                        std::string typeCode = TypeContext::mapTypeToCode(paramType, isParamArray);
                                        symtab.setType(paramIdentTok.lexeme, typeCode);
                                        
                                        LexemeRecord paramRecord;
                                        paramRecord.type = paramIdentTok.type;
                                        paramRecord.lexeme = paramIdentTok.lexeme;
                                        paramRecord.tableIndex = symtab.getIndex(paramIdentTok.lexeme);
                                        paramRecord.line = paramIdentTok.line;
                                        lexemes.push_back(paramRecord);
                                    } else if (paramIdentTok.type == TokenType::COMMA) {
                                        continue;
                                    } else if (paramIdentTok.type == TokenType::SEMI) {
                                        // Fim deste grupo de parâmetros, continua para o próximo grupo
                                        break;
                                    } else if (paramIdentTok.type == TokenType::RPAREN) {
                                        // Fim de todos os parâmetros
                                        lexer.putBackToken(paramIdentTok);
                                        break;
                                    } else {
                                        // Se não for identificador, vírgula, ponto e vírgula ou parêntese, volta o token e sai do loop
                                        lexer.putBackToken(paramIdentTok);
                                        break;
                                    }
                                }
                            } else if (paramTok.type == TokenType::IDENT) {
                                // Só identificador, sem tipo explícito
                                
                                int idx = symtab.defineOrGet(paramTok.lexeme, paramTok.line, TokenType::IDENT);
                                
                                LexemeRecord paramRecord;
                                
                                paramRecord.type = paramTok.type;
                                paramRecord.lexeme = paramTok.lexeme;
                                paramRecord.tableIndex = symtab.getIndex(paramTok.lexeme);
                                paramRecord.line = paramTok.line;
                                lexemes.push_back(paramRecord);
                            } else if (paramTok.type != TokenType::RPAREN) {
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
                    }
                    nextTok = lexer.nextToken();
                }

                // Pula tokens até encontrar o início do corpo da função
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
                
                // Processa o corpo da função
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
                
                // Espera ENDFUNCTION após o corpo
                Token endFuncTok = lexer.nextToken();
                if (endFuncTok.type != TokenType::ENDFUNCTION) {
                    throw std::runtime_error("Erro: funcao deve terminar com ENDFUNCTION (linha " + std::to_string(tok.line) + ")");
                }
                typeContext.popContext();
                break;
            }
        
        // ====== Parâmetros de Função ======
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
        // ====== Declaração de Arrays ======
        case TokenType::LBRACK:
            isArray = true;
            break;
        case TokenType::RBRACK:
            isArray = false;
            break;
        // ====== Fim de Declaração ======
        case TokenType::SEMI:
            if (typeContext.currentContext() == TypeContext::Context::VARIABLE_DECL ||
                typeContext.currentContext() == TypeContext::Context::FUNCTION_DECL)
            {
                typeContext.popContext();
            }
            break;
        // ====== Tipos de Variáveis ======
        case TokenType::REAL:
        case TokenType::INTEGER:
        case TokenType::STRING:
        case TokenType::BOOLEAN:
        case TokenType::CHARACTER:
        case TokenType::VOID:
            // Atualiza o tipo atual para declaração
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
        // ====== Constantes Booleanas ======
        case TokenType::TRUE:
        case TokenType::FALSE:
            break;
        // ====== Identificadores ======
        case TokenType::IDENT: {
            // Processa identificadores (variáveis, nomes de função, etc.)
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
        // ====== Estrutura de Repetição WHILE ======
        case TokenType::WHILE: {
            // Processa a condição do WHILE (entre parênteses)
            Token nextTok = lexer.nextToken();
            if (nextTok.type != TokenType::LPAREN) {
                throw std::runtime_error("Erro: WHILE deve ser seguido de '(' (linha " + std::to_string(tok.line) + ")");
            }
            int parenCount = 1;
            while (parenCount > 0) {
                Token condTok = lexer.nextToken();
                if (condTok.type == TokenType::END_OF_FILE) {
                    throw std::runtime_error("Erro: WHILE sem fechamento de ')' (linha " + std::to_string(tok.line) + ")");
                }
                if (condTok.type == TokenType::LPAREN) parenCount++;
                if (condTok.type == TokenType::RPAREN) parenCount--;
                // Atualiza tabela de símbolos para identificadores na condição
                if (condTok.type == TokenType::IDENT) {
                    symtab.defineOrGet(condTok.lexeme, condTok.line, TokenType::IDENT);
                }
            }
            // Espera o início do bloco '{'
            Token braceTok = lexer.nextToken();
            if (braceTok.type != TokenType::LBRACE) {
                throw std::runtime_error("Erro: WHILE deve ter bloco iniciado por '{' (linha " + std::to_string(tok.line) + ")");
            }
            // Processa o bloco do WHILE
            int braceCount = 1;
            while (braceCount > 0) {
                Token bodyTok = lexer.nextToken();
                if (bodyTok.type == TokenType::END_OF_FILE) {
                    throw std::runtime_error("Erro: WHILE sem fechamento de '}' (linha " + std::to_string(tok.line) + ")");
                }
                if (bodyTok.type == TokenType::LBRACE) braceCount++;
                if (bodyTok.type == TokenType::RBRACE) braceCount--;
                // Atualiza tabela de símbolos para identificadores no bloco
                if (bodyTok.type == TokenType::IDENT) {
                    symtab.defineOrGet(bodyTok.lexeme, bodyTok.line, TokenType::IDENT);
                }
                LexemeRecord record;
                record.type = bodyTok.type;
                record.lexeme = bodyTok.lexeme;
                record.tableIndex = symtab.getIndex(bodyTok.lexeme);
                record.line = bodyTok.line;
                lexemes.push_back(record);
            }
            // Espera ENDWHILE após o bloco
            Token endWhileTok = lexer.nextToken();
            if (endWhileTok.type != TokenType::ENDWHILE) {
                throw std::runtime_error("Erro: WHILE deve terminar com ENDWHILE (linha " + std::to_string(tok.line) + ")");
            }
            // Registra o token ENDWHILE
            LexemeRecord record;
            record.type = endWhileTok.type;
            record.lexeme = endWhileTok.lexeme;
            record.tableIndex = symtab.getIndex(endWhileTok.lexeme);
            record.line = endWhileTok.line;
            lexemes.push_back(record);
            break;
        }
        }

        // Registra cada token lido para o relatório .LEX
        LexemeRecord record;
        record.type = tok.type;
        record.lexeme = tok.lexeme;
        record.tableIndex = symtab.getIndex(tok.lexeme);
        record.line = tok.line;
        lexemes.push_back(record);
    }

    // ===============================
    //  Geração dos arquivos de saída
    // ===============================
    _generateLexFile(filename.substr(0, filename.find_last_of('.')), lexemes);
    _generateTabFile(filename.substr(0, filename.find_last_of('.')), symtab);

    std::cout << "Arquivos gerados: " << filename.substr(0, filename.find_last_of('.')) << ".LEX e " << filename.substr(0, filename.find_last_of('.')) << ".TAB\n";
    return 0;
}
