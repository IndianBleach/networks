#include "scanner.h"

#include <cctype>

const char *keywords_lookup[END] = {
    [GET] = "GET",
    [POST] = "POST",
    [HTTP] = "HTTP",
};

void token_create(token *tkn, enum token_type type, trusted_str lexeme) {
    tkn->type = type;
    tkn->lexeme = (trusted_str) malloc(strlen(lexeme) + 1);
    strcpy(tkn->lexeme, lexeme);
}

void token_destroy(token *tkn) { free(tkn->lexeme); }

void scanner_init(scanner *scnr, const trusted_str input) {
    scnr->len = strlen(input);
    scnr->cursor = scnr->start = 0;

    scnr->input = malloc(sizeof(char) * scnr->len + 1);
    // malloc error-checking
    strcpy(scnr->input, input);

    scnr->tokens = malloc(sizeof(vector));
    // malloc error-checking
    vector_init(scnr->tokens, sizeof(token), 0);
}

void scanner_destroy(scanner *scnr) {
    scnr->cursor = scnr->len = 0;

    for (size_t i = 0; scnr->tokens->len; i++) {
        token *tkn = vector_get(scnr->tokens, i);
        assert(tkn);
        token_destroy(tkn);
    }
    vector_destroy(scnr->tokens);

    free(scnr->input);
    free(scnr->tokens);
}

int is_end(scanner *scnr) { return scnr->cursor == scnr->len; }

char move(scanner *scnr) { return char_at(scnr, scnr->cursor++); }

char char_at(scanner *scnr, size_t i) {
    if (is_end(scnr))
        return '\0';
    return scnr->input[i];
}

token *next_token(scanner *scnr) {
    token *tkn = malloc(sizeof(token));
    while (!is_end(scnr)) {
        scnr->start = scnr->cursor;
        char ch = move(scnr);
        if (isdigit(ch)) {
        }
        switch (ch) {
            // symbols
            case '.': {
                token_create(tkn, DOT, ".");
                break;
            }
            case '-': {
                token_create(tkn, DASH, "-");
                break;
            }
            case '/': {
                token_create(tkn, SLASH, "/");
                break;
            }
            case ':': {
                token_create(tkn, COLON, ":");
                break;
            }
            case ';': {
                token_create(tkn, SEMICOLON, ":");
                break;
            }
            case '*': {
                token_create(tkn, STAR, "*");
                break;
            }

            // whitespaces
            case ' ': {
                token_create(tkn, SPACE, " ");
                break;
            }
            case 10: {
                token_create(tkn, LF, "\n");
                break;
            }
            case 13: {
                token_create(tkn, CR, "\r");
                break;
            }

            default:
                printf("Unknown token\n");
        }
    }
}

void scan(scanner *scnr) {
    while (!is_end(scnr)) {
        token *t = next_token(scnr);
    }
}

int main(void) { return 0; }
