#include "../headers/lexer.h"
#include "../headers/errorCodes.h"
#include "../headers/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SECTION: HELPERS
// Utility functions / definitions that are not available outside this file

#define NEAREST_SUITABLE_CAPACITY(n) (n + 7) & (-8)

/*
 * \brief       Prints out the token data in a readable format
 * \param[in]   tokenizer: The tokenizer to use
*/
void DumpTokens(Tokenizer* tokenizer) {
    printf("--- All Tokens ---\n");
    printf("Lexeme\tType\t(Line, Col)\n");
    for (int i = 0; i < tokenizer->tokensSize; i++) {
        Token* tok = tokenizer->tokens[i];
        printf("%s\t%d\t(%d, %d)\n", tok->lexeme, tok->type, tok->line, tok->col);
    }
    printf("--- End Tokens ---\n");
}

/*
 * \brief       Iterates forward one character in the provided tokenizer text
 * \param[in]   tokenizer: The tokenizer to use
*/
void StepForward(Tokenizer* tokenizer) {
    tokenizer->idx++;
    tokenizer->current = tokenizer->text[tokenizer->idx];
}

/*
 * \brief       Appends the current character to the tokenizer string buffer
 * \param[in]   tokenizer: The tokenizer to use
*/
void BufferAppend(Tokenizer* tokenizer) {
    if (tokenizer->bufferSize + 2 == tokenizer->bufferCapacity) {
        int newCapacity = NEAREST_SUITABLE_CAPACITY(tokenizer->bufferCapacity + 2);
        tokenizer->buffer = (char*)realloc(tokenizer->buffer, newCapacity * sizeof(char));
        printf("%s[Tokenizer] Reallocated buffer at %p with capacity %d%s\n", COLOR_INFO_SEQ, tokenizer->buffer, tokenizer->bufferCapacity, COLOR_RESET_SEQ);
        if (tokenizer->buffer == NULL) {
            TokenizerDelete(tokenizer);
            Log("[Tokenizer] Failed re-allocating memory for the buffer", LOG_ERROR);
            exit(INVALID_BUFFER_ALLOCATION);
        }
        tokenizer->bufferCapacity = newCapacity;
    }
    tokenizer->buffer[tokenizer->bufferSize] = tokenizer->current;
    tokenizer->bufferSize++;
    tokenizer->buffer[tokenizer->bufferSize] = '\0';
}

/*
 * \brief       Clears the tokenizer string buffer
 * \param[in]   tokenizer: The tokenizer to use
*/
void BufferClear(Tokenizer* tokenizer) {
    free(tokenizer->buffer);
    tokenizer->buffer = (char*)calloc(8, sizeof(char));
    tokenizer->bufferSize = 0;
    tokenizer->bufferCapacity = 8;
}

/*
 * \brief       Compares the tokenizer string buffer to the provided string
 * \note        The function is used solely to ease the usage of strcmp. Gets rid of
 *              the necessity of comparing the strcmp to 0
 * \param[in]   tokenizer: The tokenizer to use
 * \return      The comparison result
*/
int BufferCompare(Tokenizer* tokenizer, const char* str) {
    return !strcmp(tokenizer->buffer, str); // Because C uses 0 for a correct comparison
}

/*
 * \brief       Checks the tokenizer's token array's size and capacity values,
 *              reallocates memory if necessary.
 * \param[in]   tokenizer: The tokenizer to use
*/
void TokenizerCheckTokenArray(Tokenizer* tokenizer) {
    if (tokenizer->tokensSize+1 == tokenizer->tokensCapacity) {
        int newCapacity = NEAREST_SUITABLE_CAPACITY(tokenizer->tokensCapacity + 1);
        tokenizer->tokens  = (Token**)realloc(tokenizer->tokens, newCapacity * sizeof(Token*));
        if (tokenizer->tokens == NULL) {
            TokenizerDelete(tokenizer);
            exit(INVALID_ARRAY_ALLOCATION);
        }
        tokenizer->tokensCapacity = newCapacity;
    }
}

/*
 * \brief       Appends a new token to the tokenizer's token array
 * \param[in]   tokenizer: The tokenizer to use
*/
void TokenizerAppendToken(Tokenizer* tokenizer, TokenType type) {
    TokenizerCheckTokenArray(tokenizer);
    char* lexeme = (char*)calloc(strlen(tokenizer->buffer)+1, sizeof(char));
    for (size_t i = 0; i < strlen(tokenizer->buffer); ++i) {
        lexeme[i] = tokenizer->buffer[i];
    }
    Token* newTok = TokenCreate(tokenizer->bufferStart,
                                tokenizer->line,
                                lexeme,
                                type);
    tokenizer->tokens[tokenizer->tokensSize] = newTok;
    tokenizer->tokensSize++;
}

/*
 * \brief       Checks whether the provided character is a whitespace character
 * \param[in]   tokenizer: The tokenizer to use
 * \returns     Comparison result
*/
int IsWhitespace(char c) {
    return (c == ' ' ||
    c == '\n' ||
    c == '\t' ||
    c == '\r' ||
    c == '\0');
}

/*
 * \brief       Checks whether the provided character is a digit
 * \param[in]   tokenizer: The tokenizer to use
 * \returns     Comparison result
*/
int IsDigit(char c) {
    return (c >= 48 && c <= 57);
}

/*
 * \brief       Checks whether the provided character is an alphabetical character
 * \param[in]   tokenizer: The tokenizer to use
 * \returns     Comparison result
*/
int IsAlphabetical(char c) {
    return ((c >= 97 && c <= 122) || (c >= 65 && c <= 90));
}

/*
 * \brief       Checks whether the provided character is an acceptable character
 * \note        The acceptable in this context refers to the characters like equal sign, comma etc.
 * \param[in]   tokenizer: The tokenizer to use
 * \returns     Comparison result
*/
int IsAcceptableChar(char c) {
return ((c >= 33 && c <= 47) ||
        (c >= 58 && c <= 64) ||
        (c >= 91 && c <= 93) ||
        (c >= 123 && c <= 126));
}

/*
 * \brief       Performs whitespace related operations
 * \param[in]   tokenizer: The tokenizer to use
*/
void OnWhitespace(Tokenizer* tokenizer) {
    BufferClear(tokenizer);
    if (tokenizer->current == '\n') {
        tokenizer->col = 0;
        tokenizer->line++;
        tokenizer->bufferStart = 0;
    } else if (tokenizer->current == '\t') {
        tokenizer->col += 4;
    }
    StepForward(tokenizer);
    tokenizer->col++;
}

/*
 * \brief       Performs digit related operations
 * \param[in]   tokenizer: The tokenizer to use
*/
void OnDigit(Tokenizer* tokenizer) {
    tokenizer->bufferStart = tokenizer->col;
    while (IsDigit(tokenizer->current)) {
        BufferAppend(tokenizer);
        StepForward(tokenizer);
        tokenizer->col++;
    }
    TokenizerAppendToken(tokenizer, TOKEN_L_INT);
    BufferClear(tokenizer);
}

/*
 * \brief       Performs alphabetical character related operations
 * \param[in]   tokenizer: The tokenizer to use
*/
void OnAlpha(Tokenizer* tokenizer) {
    tokenizer->bufferStart = tokenizer->col;
    while (IsAlphabetical(tokenizer->current) || IsDigit(tokenizer->current)) {
        BufferAppend(tokenizer);
        StepForward(tokenizer);
        tokenizer->col++;
    }
    if (BufferCompare(tokenizer, "int")) {
        TokenizerAppendToken(tokenizer, TOKEN_INT);
    }
    else if (BufferCompare(tokenizer, "char")) {
        TokenizerAppendToken(tokenizer, TOKEN_CHAR);
    }
    else if (BufferCompare(tokenizer, "for")) {
        TokenizerAppendToken(tokenizer, TOKEN_FOR);
    }
    else if (BufferCompare(tokenizer, "while")) {
        TokenizerAppendToken(tokenizer, TOKEN_WHILE);
    }
    else if (BufferCompare(tokenizer, "if")) {
        TokenizerAppendToken(tokenizer, TOKEN_IF);
    }
    else if (BufferCompare(tokenizer, "else")) {
        TokenizerAppendToken(tokenizer, TOKEN_ELSE);
    }
    else if (BufferCompare(tokenizer, "return")) {
        TokenizerAppendToken(tokenizer, TOKEN_RET);
    }
    else if (BufferCompare(tokenizer, "include")) {
        TokenizerAppendToken(tokenizer, TOKEN_INCL);
    }
    else {
        TokenizerAppendToken(tokenizer, TOKEN_LIT);
    }
    BufferClear(tokenizer);
}

/*
 * \brief       Performs acceptable character related operations
 * \note        Refer to function "IsAcceptable" for the definition of the acceptable characters
 * \param[in]   tokenizer: The tokenizer to use
*/
void OnAcceptableChar(Tokenizer* tokenizer) {
    tokenizer->bufferStart = tokenizer->col;
    switch (tokenizer->current) {
    case '(':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_LPAR);
        break;
    case ')':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_RPAR);
        break;
    case '[':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_LBRA);
        break;
    case ']':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_RBRA);
        break;
    case '<':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_LANG);
        break;
    case '>':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_RANG);
        break;
    case '{':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_LCUR);
        break;
    case '}':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_RCUR);
        break;
    case ';':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_SEMI);
        break;
    case ':':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_COLO);
        break;
    case '.':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_DOT);
        break;
    case ',':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_COMM);
        break;
    case '!':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_BANG);
        break;
    case '"':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_QUOT);
        break;
    case '#':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_HASH);
        break;
    case '$':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_DOLL);
        break;
    case '%':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_MOD);
        break;
    case '&':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_AMP);
        break;
    case '\'':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_SQUO);
        break;
    case '*':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_STAR);
        break;
    case '+':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_PLUS);
        break;
    case '-':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_MINUS);
        break;
    case '/':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_SLASH);
        break;
    case '?':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_QUEST);
        break;
    case '@':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_AT);
        break;
    case '|':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_PIPE);
        break;
    case '~':
        BufferAppend(tokenizer);
        TokenizerAppendToken(tokenizer, TOKEN_TILDE);
        break;
    }
    BufferClear(tokenizer);
    StepForward(tokenizer);
    tokenizer->col++;
}

// SECTION END

// SECTION: MAIN FUNCTIONS
// Functions that are available to whichever file imports this one

/*
 * \brief       Creates the tokenizer
 * \param[in]   text: The text to tokenize
 * \returns     Newly created tokenizer struct
*/
Tokenizer TokenizerCreate(const char* text) {
    Tokenizer tokenizer;
    tokenizer.tokensCapacity = 8;
    tokenizer.tokensSize = 0;
    tokenizer.tokens = (Token**)calloc(tokenizer.tokensCapacity, sizeof(Token*));

    tokenizer.col = 1;
    tokenizer.line = 1;
    tokenizer.text = text;
    tokenizer.idx = 0;
    tokenizer.current = tokenizer.text[tokenizer.idx];
    tokenizer.bufferCapacity = 8;
    tokenizer.buffer = (char*)calloc(tokenizer.bufferCapacity, sizeof(char));
    tokenizer.bufferSize = 0;
    tokenizer.bufferStart = 0;
    return tokenizer;
}

/*
 * \brief       Frees the given tokenizer and its heap allocated components
 * \param[in]   tokenizer: The tokenizer to use
*/
void TokenizerDelete(Tokenizer* tokenizer) {
    Log("[Tokenizer] Deleting tokens", LOG_INFO);
    tokenizer->line = 0;
    tokenizer->col = 0;
    tokenizer->idx = 0;
    tokenizer->text = "";
    for (int i = 0; i < tokenizer->tokensSize; i++) {
        TokenDelete(tokenizer->tokens[i]);
    }
    free(tokenizer->buffer);
    free(tokenizer->tokens);
    tokenizer->tokensCapacity = 0;
    tokenizer->tokensSize = 0;
    tokenizer->bufferCapacity = 0;
    tokenizer->bufferSize = 0;
}

/*
 * \brief       Performs tokenization on the given tokenizer's string
 * \param[in]   tokenizer: The tokenizer to use
*/
void Tokenize(Tokenizer* tokenizer) {
    Log("[Tokenizer] Tokenizing", LOG_INFO);
    while (tokenizer->current != '\0') {
        if (IsAlphabetical(tokenizer->current))
            OnAlpha(tokenizer);
        else if (IsDigit(tokenizer->current))
            OnDigit(tokenizer);
        else if (IsWhitespace(tokenizer->current))
            OnWhitespace(tokenizer);
        else if (IsAcceptableChar(tokenizer->current))
            OnAcceptableChar(tokenizer);
        else {
            printf("%s[Tokenizer] Invalid character %c at (%d, %d)\n", COLOR_ERROR_SEQ, tokenizer->current, tokenizer->line, tokenizer->col);
            Log("Exiting program with error", LOG_ERROR);
            TokenizerDelete(tokenizer);
            exit(INVALID_CHARACTER);
        }
    }
    DumpTokens(tokenizer);
}

// SECTION END
