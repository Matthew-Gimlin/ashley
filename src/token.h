#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_ERROR = 0,
    TOKEN_EOF,
    TOKEN_NEWLINE,
    TOKEN_LEFT_PARENTHESIS,         // (
    TOKEN_RIGHT_PARENTHESIS,        // )
    TOKEN_AS,                       // as
    TOKEN_PLUS,                     // +
    TOKEN_MINUS,                    // -
    TOKEN_STAR,                     // *
    TOKEN_SLASH,                    // /
    TOKEN_PERCENT,                  // %
    TOKEN_LESS_LESS,                // <<
    TOKEN_GREATER_GREATER,          // >>
    TOKEN_AMPERSAND,                // &
    TOKEN_BAR,                      // |
    TOKEN_CARROT,                   // ^
    TOKEN_LESS,                     // <
    TOKEN_LESS_EQUAL,               // <=
    TOKEN_GREATER,                  // >
    TOKEN_GREATER_EQUAL,            // >=
    TOKEN_EQUAL_EQUAL,              // ==
    TOKEN_BANG_EQUAL,               // !=
    TOKEN_AND,                      // and
    TOKEN_OR,                       // or
    TOKEN_XOR,                      // xor
    TOKEN_EQUAL,                    // =
    TOKEN_PLUS_EQUAL,               // +=
    TOKEN_MINUS_EQUAL,              // -=
    TOKEN_STAR_EQUAL,               // *=
    TOKEN_SLASH_EQUAL,              // /=
    TOKEN_PERCENT_EQUAL,            // %=
    TOKEN_AMPERSAND_EQUAL,          // &=
    TOKEN_CARROT_EQUAL,             // ^=
    TOKEN_BAR_EQUAL,                // |=
    TOKEN_LESS_LESS_EQUAL,          // <<=
    TOKEN_GREATER_GREATER_EQUAL,    // >>=
    TOKEN_DOT,                      // .
    TOKEN_DOT_DOT_DOT,              // ...
    TOKEN_COMMA,                    // ,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_LET,
} token_t;

#endif
