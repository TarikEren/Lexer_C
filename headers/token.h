#ifndef TOKEN_H
#define TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>

typedef enum TokenType {
    TOKEN_INT = 0,      // int
    TOKEN_CHAR,         // char
    TOKEN_FOR,          // for
    TOKEN_WHILE,        // while
    TOKEN_IF,           // if
    TOKEN_ELSE,         // else
    TOKEN_RET,          // return
    TOKEN_INCL,         // include
    TOKEN_LIT  = 50,    // generic literal (variable names)
    TOKEN_L_INT,        // integer literal
    TOKEN_LPAR = 100,   // (
    TOKEN_RPAR,         // )
    TOKEN_LBRA,         // [
    TOKEN_RBRA,         // ]
    TOKEN_LANG,         // <
    TOKEN_RANG,         // >
    TOKEN_LCUR,         // {
    TOKEN_RCUR,         // }
    TOKEN_SEMI,         // ;
    TOKEN_COLO,         // :
    TOKEN_DOT,          // .
    TOKEN_COMM,         // ,
    TOKEN_BANG,         // !
    TOKEN_QUOT,         // "
    TOKEN_HASH,         // #
    TOKEN_DOLL,         // $
    TOKEN_MOD,          // %
    TOKEN_AMP,          // &
    TOKEN_SQUO,         // '
    TOKEN_STAR,         // *
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_SLASH,        // /
    TOKEN_QUEST,        // ?
    TOKEN_AT,           // @
    TOKEN_PIPE,         // |
    TOKEN_TILDE,        // ~
    TOKEN_EQ
}TokenType;

typedef struct Token {
    char* lexeme;       // Lexeme of the token
    int line;           // The line which the token is on
    int col;            // The column which the token is in
    TokenType type;     // The type of the token
} Token;

Token* TokenCreate(int col, int line, char* lexeme, TokenType type);
void TokenDelete(Token* token);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TOKEN_H

