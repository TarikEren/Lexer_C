#include "headers/lexer.h"

int main() {
    Tokenizer tokenizer = TokenizerCreate("#include <stdio.h>\nint main() {\n\tprintf(\"Hello, World!\");\nreturn 0;\n}");
    Tokenize(&tokenizer);
    TokenizerDelete(&tokenizer);
    return 0;
}
