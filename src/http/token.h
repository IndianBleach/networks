#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum token_type {

    /*
     * KEYWORDS
     * */

    // ------METHODS START------
    K_M_START,
    K_GET,
    K_POST,
    K_PUT,
    /*
     * ...
     * */
    K_M_FIN,
    // ------METHODS END--------

    K_HTTP,
    K_END,

    IDENTIFIER,
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

typedef char* trusted_str;

typedef struct {
    enum token_type type;
    trusted_str lexeme;
} token;

void token_create(token *tkn, enum token_type type, trusted_str lexeme);
void token_destroy(token *tkn);
void token_print(token *tkn);
