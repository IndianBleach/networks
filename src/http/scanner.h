#pragma once
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

#define MAX_NUMBER_LEN 10

// trusted_str means that string is null-terminated
typedef char *trusted_str;

enum token_type {
    IDENTIFIER,
    INT,
    FLOAT,
    STRING,

    // keywords
    GET,
    POST,
    HTTP,

    DOT,
    DASH,
    SLASH,
    COLON,
    SEMICOLON,
    STAR,

    LF, // '\n'
    CR, // '\r'
    SPACE,

    UNKWN,
    END,
};

extern const char *keywords_lookup[END];

typedef struct {
    enum token_type type;
    trusted_str lexeme;

    union {
        int i;
        float f;
    } u_value;
} token;

void token_create(token *tkn, enum token_type type, trusted_str lexeme);
void token_destroy(token *tkn);

typedef struct {
    trusted_str input;
    vector *tokens;
    size_t start, len, cursor;
} scanner;

void scanner_init(scanner *scnr, const trusted_str input);
void scanner_destroy(scanner *scnr);

int is_end(scanner *scnr);
char move(scanner *scnr);

token *next_token(scanner *scnr);
void scan(scanner *scnr);

void number(scanner *scnr, token *);
void identifier(scanner *scnr, token *);
