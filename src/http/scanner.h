#pragma once
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

#define MAX_NUMBER_LEN 15
#define MAX_ID_LEN 15

#define ID_CHARSET "-._"

// this is pizdec

#define peek() (_peek(scnr))
#define peek_next() (_peek_next(scnr))
#define move() (_move(scnr))
#define is_end() (_is_end(scnr))

// trusted_str means that string is null-terminated
typedef char *trusted_str;

enum token_type {
    IDENTIFIER,
    KEYWORD,

    // maybe unneccesary
    INT,
    FLOAT,

    NUMBER,
    STRING,


    DOT,
    DASH,
    SLASH,
    COLON,
    SEMICOLON,
    STAR,

    LF, // '\n'
    CR, // '\r'
    SPACE,

    UNKNWN,
    TKN_END,
};

enum keywords { GET, POST, HTTP, KEYWRD_END };

extern const char *keywords_lookup[KEYWRD_END];

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
void token_print(token *tkn);

typedef struct {
    trusted_str input;
    vector *tokens;
    size_t start, len, cursor;
} scanner;

void scanner_init(scanner *scnr, const trusted_str input);
void scanner_destroy(scanner *scnr);
void scanner_print_tokens(scanner *scnr);

int _is_end(scanner *scnr);
char _move(scanner *scnr);
char _peek(scanner *scnr);
char _peek_next(scanner *scnr);
int _match(scanner *scnr, char ch);

// copying substring from start to cursor
void substrcpy(scanner *scnr, char *dst);

token *next_token(scanner *scnr);
void scan(scanner *scnr);

void number(scanner *scnr, token *);
void identifier(scanner *scnr, token *);
