#include "scanner.h"

#include <ctype.h>

const char *keywords_lookup[K_END] = {
    [K_GET] = "GET",
    [K_POST] = "POST",
    [K_PUT] = "PUT",

    [K_HTTP] = "HTTP",
};

int find_keyword(const char *str) {
    for (int i = 0; i < K_END; i++) {
        if (strcmp(keywords_lookup[i], str) == 0)
            return i;
    }
    return UNKNWN;
}

void token_create(token *tkn, enum token_type type, trusted_str lexeme) {
    tkn->type = type;
    tkn->lexeme = (trusted_str) malloc(strlen(lexeme) + 1);
    strcpy(tkn->lexeme, lexeme);
}

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

void scanner_print_tokens(scanner *scnr) {
    for (size_t i = 0; i < scnr->tokens->len; i++) {
        token **tkn = vector_get(scnr->tokens, i);
        assert(tkn);
        token_print(*tkn);
    }
}

char _peek(scanner *scnr) {
    if (is_end())
        return '\0';
    return scnr->input[scnr->cursor];
}

char _peek_next(scanner *scnr) {
    if (scnr->cursor >= scnr->len)
        return '\0';
    return scnr->input[scnr->cursor + 1];
}

int _is_end(scanner *scnr) { return scnr->cursor == scnr->len; }

char _move(scanner *scnr) {
    if (is_end())
        return '\0';
    return scnr->input[scnr->cursor++];
}

void _putback(scanner *scnr) {
    if (scnr->cursor > 0)
        scnr->cursor--;
}

token *next_token(scanner *scnr) {
    token *tkn = malloc(sizeof(token));
    scnr->start = scnr->cursor;
    char ch = move();
    if (isdigit(ch)) {
        number(scnr, tkn);
    } else if (isalpha(ch)) {
        identifier(scnr, tkn);
    } else {
        switch (ch) {
            // symbols
            case '.': {
                token_create(tkn, DOT, ".");
                return tkn;
            }
            case '-': {
                if (isdigit(peek())) {
                    move();
                    number(scnr, tkn);
                } else 
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
                token_create(tkn, SEMICOLON, ";");
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
    while (!is_end()) {
        token *t = next_token(scnr);
        vector_push_back(scnr->tokens, &t);
    }
    token *tkn = malloc(sizeof(token));
    token_create(tkn, TKN_END, "END");
    vector_push_back(scnr->tokens, &tkn);
}

void substrncpy(scanner *scnr, char *dst, size_t n) {
    if (scnr->cursor - scnr->start < n)
        n = scnr->cursor - scnr->start;
    memcpy(dst, scnr->input + scnr->start, n);
    dst[n] = '\0';
}

void number(scanner *scnr, token *tkn) {
    tkn->lexeme = malloc(MAX_NUMBER_LEN + 1);
    tkn->type = INT;

    while (isdigit(peek()))
        move();
    if (peek() == '.' && isdigit(peek_next())) {
        tkn->type = FLOAT;
        move();
    }
    while (isdigit(peek()))
        move();

    substrncpy(scnr, tkn->lexeme, MAX_NUMBER_LEN);
}

void identifier(scanner *scnr, token *tkn) {
    tkn->lexeme = malloc(MAX_ID_LEN + 1);
    const char *charset = ID_CHARSET;
    while (!is_end() && (isalpha(peek()) || isdigit(peek()) || strchr(charset, peek())))
        move();

    substrncpy(scnr, tkn->lexeme, MAX_ID_LEN);
    int keywrd = find_keyword(tkn->lexeme);
    if (keywrd != UNKNWN) {
        tkn->type = keywrd;
    } else {
        tkn->type = IDENTIFIER;
    }
}

int main(void) {
    // TODO: cover scanner with tests
    scanner *scnr = malloc(sizeof(scanner));
    char *test = "GET HHTP HTTP PUT -abc- -GET- HTTP123";
    scanner_init(scnr, test );
    scan(scnr);
    scanner_print_tokens(scnr);
    scanner_destroy(scnr);
    free(scnr);
    return 0;
}
