#ifndef LEXER_H
#define LEXER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "token.h"

typedef struct Tokenizer {
    const char* text;       // Text to tokenize
    char current;           // The current character
    int line;               // Line of the current character
    int col;                // Column of the current character
    int idx;                // Index of the current character
    Token** tokens;         // Token array (Non-NULL terminated)
    int tokensSize;         // Size of the token array
    int tokensCapacity;     // Capacity of the token array
    char* buffer;           // The lexeme buffer
    int bufferSize;         // The buffer's size
    int bufferCapacity;     // The buffer's capacity
    int bufferStart;        // The buffer's start col
}Tokenizer;

Tokenizer TokenizerCreate(const char* text);
void TokenizerDelete(Tokenizer* tokenizer);
void Tokenize(Tokenizer* tokenizer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LEXER_H
