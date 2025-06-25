#include <string>
#include <map>
#include "token.cpp"
#include <stdexcept>

class Lexer
{
public:
    Lexer(const std::string &src)
        : src_(src), pos_(0), line_(1) {}

    Token nextToken()
    {

        skipWhitespaceAndComments();

        if (pos_ >= src_.size())
            return {TokenType::END_OF_FILE, "", line_};

        char c = src_[pos_];

        // Identificadores ou palavras-chave
        if (std::isalpha(c) || c == '_')
            return identifierOrKeyword();

        // Números
        if (std::isdigit(c))
            return number();

        // Literais de string ou char
        if (c == '"' || c == '\'')
            return stringOrChar();

        // Símbolos de um caractere ou duplo (<=, >=, := etc.)
        return symbol();
    }

    void putBackToken(const Token& tok)
    {
        pos_ -= tok.lexeme.size();
    }

private:
    const std::string src_;
    size_t pos_;
    int line_;

    void skipWhitespaceAndComments()
    {
        while (pos_ < src_.size())
        {
            if (src_[pos_] == ' ' || src_[pos_] == '\t' || src_[pos_] == '\r')
            {
                ++pos_;
            }
            else if (src_[pos_] == '\n')
            {
                ++pos_;
                ++line_;
            }
            else if (src_[pos_] == '/' && pos_ + 1 < src_.size())
            {
                if (src_[pos_ + 1] == '/')
                { // comentário de linha
                    pos_ += 2;
                    while (pos_ < src_.size() && src_[pos_] != '\n')
                        ++pos_;
                }
                else if (src_[pos_ + 1] == '*')
                { // comentário de bloco
                    pos_ += 2;
                    while (pos_ + 1 < src_.size() && !(src_[pos_] == '*' && src_[pos_ + 1] == '/'))
                    {

                        if (src_[pos_] == '\n')
                            ++line_;

                        ++pos_;
                    }
                    pos_ += 2;
                }
                else
                    break;
            }
            else
                break;
        }
    }

    Token identifierOrKeyword()
    {
        size_t start = pos_;

        while (pos_ < src_.size() &&
               (std::isalnum(src_[pos_]) || src_[pos_] == '_'))
            ++pos_;

        std::string lex = src_.substr(start, pos_ - start);

        for (auto &ch : lex)
            ch = std::toupper(ch);

        static const std::map<std::string, TokenType> kw = {
            {"PROGRAM", TokenType::PROGRAM},
            {"DECLARATIONS", TokenType::DECLARATIONS},
            {"ENDDECLARATIONS", TokenType::ENDDECLARATIONS},
            {"FUNCTIONS", TokenType::FUNCTIONS},
            {"ENDFUNCTION", TokenType::ENDFUNCTION},
            {"ENDFUNCTIONS", TokenType::ENDFUNCTIONS},
            {"ENDPROGRAM", TokenType::ENDPROGRAM},
            {"VARTYPE", TokenType::VARTYPE},
            {"FUNCTYPE", TokenType::FUNCTYPE},
            {"PARAMTYPE", TokenType::PARAMTYPE},
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
            {"TRUE", TokenType::TRUE},
            {"FALSE", TokenType::FALSE}};

        auto it = kw.find(lex);

        if (it != kw.end())
            return {it->second, lex, line_};

        return {TokenType::IDENT, lex, line_};
    }

    Token number()
    {
        size_t start = pos_;

        while (pos_ < src_.size() && std::isdigit(src_[pos_]))
            ++pos_;

        if (pos_ < src_.size() && src_[pos_] == '.')
        {
            ++pos_;

            while (pos_ < src_.size() && std::isdigit(src_[pos_]))
                ++pos_;

            return {TokenType::REALCONST, src_.substr(start, pos_ - start), line_};
        }

        return {TokenType::INTCONST, src_.substr(start, pos_ - start), line_};
    }

    Token stringOrChar()
    {
        char quote = src_[pos_++];

        size_t start = pos_;

        while (pos_ < src_.size() && src_[pos_] != quote)
        {
            if (src_[pos_] == '\n')
                ++line_;
            ++pos_;
        }

        if (pos_ >= src_.size())
        {
            throw std::runtime_error("Erro na linha " + std::to_string(line_) +
                                     ": String nao fechada. Esperava '" + quote + "'");
        }

        std::string lit = src_.substr(start, pos_ - start);

        pos_++;

        return {quote == '"'
                    ? TokenType::STRINGCONST
                    : TokenType::CHARCONST,
                lit, line_};
    }

    Token symbol()
    {
        auto match2 = [&](char a, char b, TokenType two, TokenType one)
        {
            if (src_[pos_] == a && pos_ + 1 < src_.size() && src_[pos_ + 1] == b)
            {
                pos_ += 2;
                return Token{two, std::string(1, a) + b, line_};
            }

            pos_++;

            return Token{one, std::string(1, a), line_};
        };

        char c = src_[pos_];

        switch (c)
        {
        case ';':
            ++pos_;
            return {TokenType::SEMI, ";", line_};
        case ':':
            return match2(':', '=', TokenType::ASSIGN, TokenType::COLON);
        case ',':
            ++pos_;
            return {TokenType::COMMA, ",", line_};
        case '[':
            ++pos_;
            return {TokenType::LBRACK, "[", line_};
        case ']':
            ++pos_;
            return {TokenType::RBRACK, "]", line_};
        case '(':
            ++pos_;
            return {TokenType::LPAREN, "(", line_};
        case ')':
            ++pos_;
            return {TokenType::RPAREN, ")", line_};
        case '{':
            ++pos_;
            return {TokenType::LBRACE, "{", line_};
        case '}':
            ++pos_;
            return {TokenType::RBRACE, "}", line_};
        case '?':
            ++pos_;
            return {TokenType::QUESTION, "?", line_};
        case '<':
            return match2('<', '=', TokenType::LE, TokenType::LT);
        case '>':
            return match2('>', '=', TokenType::GE, TokenType::GT);
        case '=':
            return match2('=', '=', TokenType::EQ, TokenType::ASSIGN);
        case '!':
            return match2('!', '=', TokenType::NE, TokenType::HASH);
        case '+':
            ++pos_;
            return {TokenType::PLUS, "+", line_};
        case '-':
            ++pos_;
            return {TokenType::MINUS, "-", line_};
        case '*':
            ++pos_;
            return {TokenType::MUL, "*", line_};
        case '/':
            ++pos_;
            return {TokenType::DIV, "/", line_};
        case '%':
            ++pos_;
            return {TokenType::MOD, "%", line_};
        default:
            throw std::runtime_error("Erro na linha " + std::to_string(line_) +
                                     ": Caractere invalido '" + c + "'");
        }
    }
};