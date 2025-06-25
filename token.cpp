#include <string>

enum class TokenType
{
    // Palavras-reservadas
    PROGRAM,
    DECLARATIONS,
    ENDDECLARATIONS,
    FUNCTIONS,
    ENDFUNCTION,
    ENDFUNCTIONS,
    ENDPROGRAM,
    VARTYPE,
    FUNCTYPE,
    IF,
    ELSE,
    ENDIF,
    WHILE,
    ENDWHILE,
    RETURN,
    BREAK,
    PRINT,
    PARAMTYPE,
    TRUE,
    FALSE,

    // Operadores e símbolos
    SEMI,
    COLON,
    COMMA,
    LBRACK,
    RBRACK,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    ASSIGN,
    LE,
    LT,
    GT,
    GE,
    EQ,
    NE,
    HASH,
    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,

    // Tipos
    REAL,
    INTEGER,
    STRING,
    BOOLEAN,
    CHARACTER,
    VOID,

    // Identificadores e literais
    IDENT,
    INTCONST,
    REALCONST,
    STRINGCONST,
    CHARCONST,
    QUESTION,
    END_OF_FILE
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
};