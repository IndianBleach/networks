#include "scanner.h"

#include <ctype.h>

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
    vector_init(scnr->tokens, sizeof(token *), 0);
}

void scanner_destroy(scanner *scnr) {
    for (size_t i = 0; i < scnr->tokens->len; i++) {
        token **tkn = vector_get(scnr->tokens, i);
        assert(tkn);
        token_destroy(*tkn);
        free(*tkn);
    }
    vector_destroy(scnr->tokens);
    free(scnr->tokens);

    scnr->cursor = scnr->len = 0;
    free(scnr->input);
}

int is_end(scanner *scnr) { return scnr->cursor == scnr->len; }

char move(scanner *scnr) {
    if (is_end(scnr))
        return '\0';
    return scnr->input[scnr->cursor++];
}

token *next_token(scanner *scnr) {
    token *tkn = malloc(sizeof(token));
    while (!is_end(scnr)) {
        scnr->start = scnr->cursor;
        char ch = move(scnr);
        if (isdigit(ch)) {
            number(scnr, tkn);
            return tkn;
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
    return tkn;
}

void scan(scanner *scnr) {
    while (!is_end(scnr)) {
        token *t = next_token(scnr);
        vector_push_back(scnr->tokens, &t);
    }
}

void number(scanner *scnr, token *tkn) {
    int is_float = 0;
    tkn->lexeme = malloc(MAX_NUMBER_LEN + 1);
    while (!is_end(scnr) && scnr->cursor < MAX_NUMBER_LEN) {
        char ch = move(scnr);
        if (ch == '.' && !is_float)
            is_float = 1;
        else if (!isdigit(ch))
            break;
    }
    size_t substr_len = scnr->cursor - scnr->start;
    assert(MAX_NUMBER_LEN >= substr_len);
    memcpy(tkn->lexeme, scnr->input + scnr->start, substr_len);
    tkn->lexeme[substr_len] = '\0';
    if (is_float) {
        tkn->type = FLOAT;
        tkn->u_value.f = atof(tkn->lexeme);
    } else {
        tkn->type = INT;
        tkn->u_value.i = atol(tkn->lexeme);
    }
}

void identifier(scanner *scnr, token *tkn) {}

int main(void) {
    scanner *scnr = malloc(sizeof(scanner));
    char *tests[] = {
        "0",
    };
    scanner_init(scnr, tests[0]);
    scan(scnr);
    scanner_destroy(scnr);
    free(scnr);
    return 0;
}
