#include <string>
#include <vector>
#include "lexer.cpp"
#include <bits/algorithmfwd.h>
#include <iostream>

class SymbolTable
{
public:
    struct SymbolInfo
    {
        int entry;
        std::string atomCode;
        std::string lexeme;
        int lenBefore;
        int lenAfter;
        std::string type;
        std::vector<int> lines;
    };

    SymbolTable() : nextEntry_(1) {}

    void checkIfIdentifierIsReservedKeyword(std::string truncatedLex, int line)
    {
        static const std::map<std::string, TokenType> reservedKeywords = {
            {"PROGRAM", TokenType::PROGRAM},
            {"DECLARATIONS", TokenType::DECLARATIONS},
            {"ENDDECLARATIONS", TokenType::ENDDECLARATIONS},
            {"FUNCTIONS", TokenType::FUNCTIONS},
            {"ENDFUNCTIONS", TokenType::ENDFUNCTIONS},
            {"ENDPROGRAM", TokenType::ENDPROGRAM},
            {"VAR", TokenType::VAR},
            {"IF", TokenType::IF},
            {"ELSE", TokenType::ELSE},
            {"ENDIF", TokenType::ENDIF},
            {"WHILE", TokenType::WHILE},
            {"ENDWHILE", TokenType::ENDWHILE},
            {"RETURN", TokenType::RETURN},
            {"BREAK", TokenType::BREAK},
            {"PRINT", TokenType::PRINT},
            {"REAL", TokenType::REAL},
            {"INTEGER", TokenType::INTEGER},
            {"STRING", TokenType::STRING},
            {"BOOLEAN", TokenType::BOOLEAN},
            {"CHARACTER", TokenType::CHARACTER},
            {"VOID", TokenType::VOID},
            {"FUNCTYPE", TokenType::FUNCTYPE}};

        std::string upperLex = truncatedLex;
        for (auto &ch : upperLex)
            ch = std::toupper(ch);

        if (reservedKeywords.find(upperLex) != reservedKeywords.end())
        {
            throw std::runtime_error("Erro na linha " + std::to_string(line) +
                                     ": Nao pode utilizar a palavra reservada '" + truncatedLex +
                                     "' como nome de variavel");
        }
    }

    int defineOrGet(const std::string &lex, int line, TokenType type)
    {
        std::string truncatedLex = lex.substr(0, 35);

        checkIfIdentifierIsReservedKeyword(truncatedLex, line);

        auto it = table_.find(truncatedLex);
        if (it == table_.end())
        {
            SymbolInfo info;
            info.entry = nextEntry_++;
            info.atomCode = tokenTypeToString(type);
            info.lexeme = truncatedLex;
            info.lenBefore = (int)lex.size();
            info.lenAfter = (int)truncatedLex.size();
            info.type = tokenTypeToString(TokenType::VOID);
            info.lines.push_back(line);
            table_[truncatedLex] = info;
            return info.entry;
        }
        else
        {
            SymbolInfo &info = it->second;
            if ((int)info.lines.size() < 5 && info.lines.back() != line)
                info.lines.push_back(line);
            return info.entry;
        }
    }

    void setType(const std::string &lex, const std::string &type)
    {
        auto it = table_.find(lex.substr(0, 35));
        if (it != table_.end() && isValidType(type))
        {
            it->second.type = type;
        }
    }

    const std::map<std::string, SymbolInfo> &all() const
    {
        return table_;
    }

    static bool isValidType(const std::string &type)
    {
        static const std::vector<std::string> validTypes = {
            "FP", // Floating Point
            "IN", // Integer
            "ST", // String
            "CH", // Character
            "BL", // Boolean
            "VD", // Void
            "AF", // Array of Floating Point
            "AI", // Array of Integer
            "AS", // Array of String
            "AC", // Array of Character
            "AB", // Array of Boolean
        };
        return std::find(validTypes.begin(), validTypes.end(), type) != validTypes.end();
    }

    static std::string tokenTypeToString(TokenType t)
    {
        switch (t)
        {
        case TokenType::IDENT:
            return "IDENT";
        case TokenType::INTCONST:
            return "INTCONST";
        case TokenType::REALCONST:
            return "REALCONST";
        case TokenType::STRINGCONST:
            return "STRINGCONST";
        case TokenType::CHARCONST:
            return "CHARCONST";
        case TokenType::PROGRAM:
            return "PROGRAM";
        case TokenType::DECLARATIONS:
            return "DECLARATIONS";
        case TokenType::ENDDECLARATIONS:
            return "ENDDECLARATIONS";
        case TokenType::FUNCTIONS:
            return "FUNCTIONS";
        case TokenType::ENDFUNCTIONS:
            return "ENDFUNCTIONS";
        case TokenType::ENDPROGRAM:
            return "ENDPROGRAM";
        case TokenType::VAR:
            return "VAR";
        case TokenType::IF:
            return "IF";
        case TokenType::ELSE:
            return "ELSE";
        case TokenType::ENDIF:
            return "ENDIF";
        case TokenType::WHILE:
            return "WHILE";
        case TokenType::ENDWHILE:
            return "ENDWHILE";
        case TokenType::RETURN:
            return "RETURN";
        case TokenType::BREAK:
            return "BREAK";
        case TokenType::PRINT:
            return "PRINT";
        case TokenType::REAL:
            return "REAL";
        case TokenType::INTEGER:
            return "INTEGER";
        case TokenType::STRING:
            return "STRING";
        case TokenType::BOOLEAN:
            return "BOOLEAN";
        case TokenType::CHARACTER:
            return "CHARACTER";
        case TokenType::VOID:
            return "VOID";
        case TokenType::FUNCTYPE:
            return "FUNCTYPE";
        case TokenType::SEMI:
            return "SEMI";
        case TokenType::COLON:
            return "COLON";
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::LBRACK:
            return "LBRACK";
        case TokenType::RBRACK:
            return "RBRACK";
        case TokenType::LPAREN:
            return "LPAREN";
        case TokenType::RPAREN:
            return "RPAREN";
        case TokenType::LBRACE:
            return "LBRACE";
        case TokenType::RBRACE:
            return "RBRACE";
        case TokenType::ASSIGN:
            return "ASSIGN";
        case TokenType::LE:
            return "LE";
        case TokenType::LT:
            return "LT";
        case TokenType::GT:
            return "GT";
        case TokenType::GE:
            return "GE";
        case TokenType::EQ:
            return "EQ";
        case TokenType::NE:
            return "NE";
        case TokenType::HASH:
            return "HASH";
        case TokenType::PLUS:
            return "PLUS";
        case TokenType::MINUS:
            return "MINUS";
        case TokenType::MUL:
            return "MUL";
        case TokenType::DIV:
            return "DIV";
        case TokenType::MOD:
            return "MOD";
        case TokenType::QUESTION:
            return "QUESTION";
        case TokenType::END_OF_FILE:
            return "END_OF_FILE";
        default:
            return std::to_string(int(t));
        }
    }

private:
    std::map<std::string, SymbolInfo> table_;
    int nextEntry_;
};

struct LexemeRecord
{
    std::string lexeme;
    TokenType type;
    int tableIndex; // -1 se não for identificador
    int line;
};