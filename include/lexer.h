#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types for our web language
typedef enum {
    TOKEN_EOF,
    TOKEN_ROUTE,
    TOKEN_RESPONSE,
    TOKEN_HTML,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_FUNCTION,
    TOKEN_RETURN,
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_LBRACE,      // {
    TOKEN_RBRACE,      // }
    TOKEN_LPAREN,      // (
    TOKEN_RPAREN,      // )
    TOKEN_EQUALS,      // =
    TOKEN_PLUS,        // +
    TOKEN_MINUS,       // -
    TOKEN_STAR,        // *
    TOKEN_SLASH,       // /
    TOKEN_DOT,         // .
    TOKEN_COMMA,       // ,
    TOKEN_COLON,       // :
    TOKEN_SEMICOLON,   // ;
    TOKEN_LT,          // <
    TOKEN_GT,          // >
    TOKEN_LTE,         // <=
    TOKEN_GTE,         // >=
    TOKEN_EQ,          // ==
    TOKEN_NEQ,         // !=
    TOKEN_AND,         // &&
    TOKEN_OR,          // ||
    TOKEN_UNKNOWN
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char *value;
    int line;
    int column;
} Token;

// Lexer structure
typedef struct {
    const char *source;
    int position;
    int line;
    int column;
    char current_char;
} Lexer;

// Function prototypes
Lexer* lexer_init(const char *source);
void lexer_free(Lexer *lexer);
Token* lexer_next_token(Lexer *lexer);
void token_free(Token *token);
const char* token_type_to_string(TokenType type);
void token_print(Token *token);

#endif
