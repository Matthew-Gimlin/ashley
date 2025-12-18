#include "lex.h"
#include "state.h"
#include "common.h"
#include "token.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

void lex_init(ash_state_t* a, const char* filename, const char* src) {
    a->filename = filename;
    a->src = src;
    a->begin = a->end = 0;
    a->token = TOKEN_ERROR;
}

static inline bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

static inline bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline bool is_binary(char c) {
    return c == '0' || c == '1';
}

static inline bool is_octal(char c) {
    return c >= '0' && c <= '7';
}

static inline bool is_hexadecimal(char c) {
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static inline bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static void skip_space(ash_state_t* a) {
    while (is_space(a->src[a->end])) {
        a->end++;
    }
}

static void skip_comment(ash_state_t* a) {
    while (a->src[a->end] && a->src[a->end] != '\n') {
        a->end++;
    }
}

static bool accept(ash_state_t* a, char c) {
    if (a->src[a->end] == c) {
        a->end++;
        return true;
    }
    return false;
}

static bool accept_n(ash_state_t* a, const char* s, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (a->src[a->end + i] == '\0' || s[i] == '\0') {
            return false;
        }
        if (a->src[a->end + i] != s[i]) {
            return false;
        }
    }
    a->end += n;
    return true;
}

static int skip_digits(ash_state_t* a) {
    int count = 0;
    while (is_digit(a->src[a->end])) {
        a->end++;
        count++;
    }
    return count;
}

static int lex_binary_int(ash_state_t* a) {
    while (is_binary(a->src[a->end])) {
        a->end++;
    }
    if (is_digit(a->src[a->end])) {
        a->token = TOKEN_ERROR;
        return 1;
    }
    a->token = TOKEN_INT;
    return 0;
}

static int lex_octal_int(ash_state_t* a) {
    while (is_octal(a->src[a->end]))
        a->end++;
    if (is_digit(a->src[a->end])) {
        a->token = TOKEN_ERROR;
        return 1;
    }
    a->token = TOKEN_INT;
    return 0;
}

static int lex_hexadecimal_int(ash_state_t* a) {
    while (is_hexadecimal(a->src[a->end]))
        a->end++;
    if (is_alpha(a->src[a->end])) {
        a->token = TOKEN_ERROR;
        return 1;
    }
    a->token = TOKEN_INT;
    return 0;
}

static int lex_number(ash_state_t* a) {
    a->token = TOKEN_INT;
    if (accept(a, '0')) {
        if (accept(a, 'b') || accept(a, 'B'))
            return lex_binary_int(a);
        if (accept(a, 'o') || accept(a, 'O'))
            return lex_octal_int(a);
        if (accept(a, 'x') || accept(a, 'X'))
            return lex_hexadecimal_int(a);
    }
    skip_digits(a);
    if (accept(a, '.')) {
        a->token = TOKEN_FLOAT;
        if (skip_digits(a) <= 0) {
            a->token = TOKEN_ERROR;
            return 1;
        }
    }
    if (accept(a, 'e') || accept(a, 'E')) {
        a->token = TOKEN_FLOAT;
        accept(a, '-');
        accept(a, '+');
        if (skip_digits(a) <= 0) {
            a->token = TOKEN_ERROR;
            return 1;
        }
        if (accept(a, '.')) {
            a->token = TOKEN_ERROR;
            return 1;
        }
    }
    return 0;
}

static int lex_string(ash_state_t* a) {
    a->end++;
    while (a->src[a->end]) {
        if (accept(a, '"')) {
            a->token = TOKEN_STRING;
            return 0;
        } else if (accept(a, '\\')) {
            // TODO
            // handle string escapes
        }
        a->end++;
    }
    a->token = TOKEN_ERROR;
    return 1;
}

static int lex_char(ash_state_t* a) {
    a->end++;
    if (a->src[a->end] == '\0') {
        a->token = TOKEN_ERROR;
        return 1;
    } else if (accept(a, '\\')) {
        // TODO
        // handle string escapes
        a->end++;
    } else {
        a->end++;
    }
    if (!accept(a, '\'')) {
        a->token = TOKEN_ERROR;
        return 1;
    }
    a->token = TOKEN_CHAR;
    return 0;
}

static bool is_keyword(ash_state_t* a, unsigned i, const char* keyword,
        size_t keyword_size) {
    return a->end - i == keyword_size &&
        memcmp(&a->src[i], keyword, keyword_size);
}

static token_t get_identifier_type(ash_state_t* a) {
    unsigned i = a->begin;
    switch (a->src[i++]) {
        case 'a':
            if (is_keyword(a, i, "nd", 2)) {
                return TOKEN_AND;
            }
            break;

        case 'o':
            if (is_keyword(a, i, "r", 1)) {
                return TOKEN_OR;
            }
            break;

        case 'x':
            if (is_keyword(a, i, "or", 2)) {
                return TOKEN_XOR;
            }
            break;

        case 'l':
            if (is_keyword(a, i, "et", 2)) {
                return TOKEN_LET;
            }
            break;

        default:
            break;
    }
    return TOKEN_IDENTIFIER;
}

static int lex_identifier(ash_state_t* a) {
    while (is_alpha(a->src[a->end]) || is_digit(a->src[a->end]) ||
            a->src[a->end] == '_') {
        a->end++;
    }
    a->token = get_identifier_type(a);
    return 0;
}

int lex(ash_state_t* a) {
    skip_space(a);
    a->begin = a->end;
    switch (a->src[a->end]) {
        case '\0':
            a->token = TOKEN_EOF;
            return 0;

        case '#':
            skip_comment(a);
            FALLTHROUGH();
        case '\n':
            a->token = TOKEN_NEWLINE;
            a->end++;
            return 0;

        case '(':
            a->end++;
            a->token = TOKEN_LEFT_PARENTHESIS;
            return 0;

        case ')':
            a->end++;
            a->token = TOKEN_RIGHT_PARENTHESIS;
            return 0;

        case '+':
            a->end++;
            a->token = accept(a, '=') ? TOKEN_PLUS_EQUAL : TOKEN_PLUS;
            return 0;

        case '-':
            a->end++;
            a->token = accept(a, '=') ? TOKEN_MINUS_EQUAL : TOKEN_MINUS;
            return 0;

        case '*':
            a->end++;
            a->token = accept(a, '=') ? TOKEN_STAR_EQUAL : TOKEN_STAR;
            return 0;

        case '/':
            a->end++;
            a->token = accept(a, '=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH;
            return 0;

        case '%':
            a->end++;
            a->token = accept(a, '=') ? TOKEN_PERCENT_EQUAL : TOKEN_PERCENT;
            return 0;

        case '&':
            a->end++;
            a->token = accept(a, '=') ? TOKEN_AMPERSAND_EQUAL : TOKEN_AMPERSAND;
            return 0;

        case '^':
            a->end++;
            a->token = accept(a, '=') ? TOKEN_CARROT_EQUAL : TOKEN_CARROT;
            return 0;

        case '|':
            a->end++;
            a->token = accept(a, '=') ? TOKEN_BAR_EQUAL : TOKEN_BAR;
            return 0;

        case '<':
            a->end++;
            a->token = accept(a, '=') ? accept_n(a, "<=", 2) ?
                TOKEN_LESS_LESS_EQUAL : TOKEN_LESS_EQUAL : TOKEN_LESS;
            return 0;

        case '>':
            a->end++;
            a->token = accept(a, '=') ? accept_n(a, ">=", 2) ?
                TOKEN_GREATER_GREATER_EQUAL : TOKEN_GREATER_EQUAL :
                TOKEN_GREATER;
            return 0;

        case '=':
            a->end++;
            a->token = accept(a, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL;
            return 0;

        case '.':
            a->end++;
            a->token = accept_n(a, "..", 2) ? TOKEN_DOT_DOT_DOT : TOKEN_DOT;
            return 0;

        case '"':
            return lex_string(a);

        case '\'':
            return lex_char(a);

        default:
            break;
    }
    if (is_digit(a->src[a->end])) {
        return lex_number(a);
    }
    if (is_alpha(a->src[a->end]) || a->src[a->end] == '_') {
        return lex_identifier(a);
    }
    a->token = TOKEN_ERROR;
    return 1;
}
