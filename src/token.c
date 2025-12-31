#include "../headers/token.h"
#include <stdlib.h>

/*
 * \brief       Finds the nearest suitable capacity
 * \note        The nearest suitable capacity in this context
 *              is the nearest multiple of 8 to the provided
 *              value
 * \param[in]   n: Value to use
 * \returns     Nearest divisor of 8 to the provided value
 */
#define NEAREST_SUITABLE_CAPACITY(n) (n + 7) & (-8)

/*
 * \brief       Creates a new token struct
 * \param[in]   col: Current column of the tokenizer
 * \param[in]   line: Current line of the tokenizer
 * \param[in]   lexeme: The lexeme of the token
 * \param[in]   type: The type of the token
 * \returns     The newly created token struct
*/
Token* TokenCreate(int col, int line, char* lexeme, TokenType type) {
    Token* newToken = (Token*)malloc(sizeof(Token));
    newToken->col = col;
    newToken->line = line;
    newToken->lexeme = lexeme;
    newToken->type = type;
    return newToken;
}

/*
 * \brief       Frees the heap allocated token and its lexeme
 * \param[in]   token: Token to use
 */
void TokenDelete(Token* token) {
    free(token->lexeme);
    token->col = 0;
    token->line = 0;
    free(token);
    token = NULL;
}
