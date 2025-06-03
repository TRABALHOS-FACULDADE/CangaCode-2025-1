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

        if (pos_ >= src_.size())
            return {TokenType::END_OF_FILE, "", line_};

        char c = src_[pos_];

        // Números
        if (std::isdigit(c))
            return number();

        // Literais de string ou char
        if (c == '"' || c == '\'')
            return stringOrChar();

        // Símbolos de um caractere ou combinação (<=, >=, := etc.)
        return symbol();
    }

private:
    const std::string src_;
    size_t pos_;
    int line_;

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
            return match2('=', '=', TokenType::EQ, TokenType::HASH);
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
            throw std::runtime_error("Símbolo inesperado na linha " + std::to_string(line_));
        }
    }
};