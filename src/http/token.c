#include "token.h"

void token_destroy(token *tkn) { free(tkn->lexeme); }

void token_print(token *tkn) {
    switch (tkn->type) {
        case INT: {
            printf("Token INT with value %s\n", tkn->lexeme);
            break;
        }
        case FLOAT: {
            printf("Token FLOAT with value %s\n", tkn->lexeme);
            break;
        }
        default: {
            printf("Token %s\n", tkn->lexeme);
        }
    }
}

void token_create(token *tkn, enum token_type type, trusted_str lexeme) {
    tkn->type = type;
    tkn->lexeme = (trusted_str) malloc(strlen(lexeme) + 1);
    strcpy(tkn->lexeme, lexeme);
}
