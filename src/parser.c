#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Helper: Peek at next token without consuming it
static Token *peek_next(Parser *parser)
{
    int saved_pos = parser->lexer->position;
    int saved_line = parser->lexer->line;
    int saved_column = parser->lexer->column;
    char saved_char = parser->lexer->current_char;

    Token *next = lexer_next_token(parser->lexer);

    // Restore lexer state
    parser->lexer->position = saved_pos;
    parser->lexer->line = saved_line;
    parser->lexer->column = saved_column;
    parser->lexer->current_char = saved_char;

    return next;
}

// Helper: Advance to next token
static void advance(Parser *parser)
{
    if (parser->current_token)
    {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

// Helper: Check if current token matches expected type
static int check(Parser *parser, TokenType type)
{
    return parser->current_token->type == type;
}

// Helper: Consume token if it matches, otherwise error
static void expect(Parser *parser, TokenType type, const char *message)
{
    if (!check(parser, type))
    {
        fprintf(stderr, "Parse error at line %d: %s\n",
                parser->current_token->line, message);
        fprintf(stderr, "Expected %s, got %s\n",
                token_type_to_string(type),
                token_type_to_string(parser->current_token->type));
        exit(1);
    }
    advance(parser);
}

// Forward declarations
static ASTNode *parse_statement(Parser *parser);
static ASTNode *parse_block(Parser *parser);
static ASTNode *parse_expression(Parser *parser);

// Parse a primary expression (string, number, identifier)
static ASTNode *parse_primary(Parser *parser)
{
    if (check(parser, TOKEN_STRING))
    {
        ASTNode *node = ast_create_string(parser->current_token->value);
        advance(parser);
        return node;
    }

    if (check(parser, TOKEN_NUMBER))
    {
        double value = atof(parser->current_token->value);
        ASTNode *node = ast_create_number(value);
        advance(parser);
        return node;
    }

    if (check(parser, TOKEN_IDENTIFIER))
    {
        ASTNode *node = ast_create_identifier(parser->current_token->value);
        advance(parser);
        return node;
    }

    if (check(parser, TOKEN_LPAREN))
    {
        advance(parser); // consume '('
        ASTNode *expr = parse_expression(parser);
        expect(parser, TOKEN_RPAREN, "Expected ')' after expression");
        return expr;
    }

    fprintf(stderr, "Parse error: Expected expression at line %d\n",
            parser->current_token->line);
    exit(1);
}

// Parse multiplication and division
static ASTNode *parse_multiplicative(Parser *parser)
{
    ASTNode *left = parse_primary(parser);

    while (check(parser, TOKEN_STAR) || check(parser, TOKEN_SLASH))
    {
        char *op = strdup(parser->current_token->value);
        advance(parser);
        ASTNode *right = parse_primary(parser);
        left = ast_create_binary_op(op, left, right);
        free(op);
    }

    return left;
}

// Parse addition and subtraction
static ASTNode *parse_additive(Parser *parser)
{
    ASTNode *left = parse_multiplicative(parser);

    while (check(parser, TOKEN_PLUS) || check(parser, TOKEN_MINUS))
    {
        char *op = strdup(parser->current_token->value);
        advance(parser);
        ASTNode *right = parse_multiplicative(parser);
        left = ast_create_binary_op(op, left, right);
        free(op);
    }

    return left;
}

// Parse comparison operators
static ASTNode *parse_comparison(Parser *parser)
{
    ASTNode *left = parse_additive(parser);

    while (check(parser, TOKEN_LT) || check(parser, TOKEN_GT) ||
           check(parser, TOKEN_LTE) || check(parser, TOKEN_GTE))
    {
        char *op = strdup(parser->current_token->value);
        advance(parser);
        ASTNode *right = parse_additive(parser);
        left = ast_create_binary_op(op, left, right);
        free(op);
    }

    return left;
}

// Parse equality operators
static ASTNode *parse_equality(Parser *parser)
{
    ASTNode *left = parse_comparison(parser);

    while (check(parser, TOKEN_EQ) || check(parser, TOKEN_NEQ))
    {
        char *op = strdup(parser->current_token->value);
        advance(parser);
        ASTNode *right = parse_comparison(parser);
        left = ast_create_binary_op(op, left, right);
        free(op);
    }

    return left;
}

// Parse logical AND
static ASTNode *parse_and(Parser *parser)
{
    ASTNode *left = parse_equality(parser);

    while (check(parser, TOKEN_AND))
    {
        char *op = strdup(parser->current_token->value);
        advance(parser);
        ASTNode *right = parse_equality(parser);
        left = ast_create_binary_op(op, left, right);
        free(op);
    }

    return left;
}

// Parse logical OR (lowest precedence)
static ASTNode *parse_expression(Parser *parser)
{
    ASTNode *left = parse_and(parser);

    while (check(parser, TOKEN_OR))
    {
        char *op = strdup(parser->current_token->value);
        advance(parser);
        ASTNode *right = parse_and(parser);
        left = ast_create_binary_op(op, left, right);
        free(op);
    }

    return left;
}

// Parse a block { ... }
static ASTNode *parse_block(Parser *parser)
{
    expect(parser, TOKEN_LBRACE, "Expected '{' to start block");

    ASTNode *block = ast_create_block();

    while (!check(parser, TOKEN_RBRACE) && !check(parser, TOKEN_EOF))
    {
        ASTNode *stmt = parse_statement(parser);
        ast_block_add_statement(block, stmt);
    }

    expect(parser, TOKEN_RBRACE, "Expected '}' to end block");

    return block;
}

// Parse a response statement
static ASTNode *parse_response(Parser *parser)
{
    expect(parser, TOKEN_RESPONSE, "Expected 'response'");

    int is_html = 0;
    ASTNode *value = NULL;

    // Check for 'html' keyword
    if (check(parser, TOKEN_HTML))
    {
        is_html = 1;
        advance(parser);
        value = parse_block(parser);
    }
    else
    {
        value = parse_expression(parser);
    }

    return ast_create_response(value, is_html);
}

// Parse an assignment: name = value
static ASTNode *parse_assignment(Parser *parser)
{
    char *name = strdup(parser->current_token->value);
    advance(parser); // consume identifier

    expect(parser, TOKEN_EQUALS, "Expected '=' in assignment");

    ASTNode *value = parse_expression(parser);

    return ast_create_assignment(name, value);
}

// Parse a statement (assignment, response, or bare identifier)
static ASTNode *parse_statement(Parser *parser)
{
    if (check(parser, TOKEN_RESPONSE))
    {
        return parse_response(parser);
    }

    if (check(parser, TOKEN_IDENTIFIER))
    {
        // Peek ahead to see if it's an assignment
        Token *next = peek_next(parser);
        int is_assignment = (next->type == TOKEN_EQUALS);
        token_free(next);

        if (is_assignment)
        {
            return parse_assignment(parser);
        }
        else
        {
            // Just a bare identifier
            ASTNode *node = ast_create_identifier(parser->current_token->value);
            advance(parser);
            return node;
        }
    }

    fprintf(stderr, "Parse error: Unexpected token at line %d\n",
            parser->current_token->line);
    exit(1);
}

// Parse a route definition
static ASTNode *parse_route(Parser *parser)
{
    expect(parser, TOKEN_ROUTE, "Expected 'route'");

    if (!check(parser, TOKEN_STRING))
    {
        fprintf(stderr, "Parse error: Expected route path string at line %d\n",
                parser->current_token->line);
        exit(1);
    }

    char *path = strdup(parser->current_token->value);
    advance(parser);

    ASTNode *body = parse_block(parser);

    return ast_create_route(path, body);
}

// Parse the entire program
static ASTNode *parse_program(Parser *parser)
{
    ASTNode *program = ast_create_program();

    while (!check(parser, TOKEN_EOF))
    {
        ASTNode *route = parse_route(parser);
        ast_program_add_route(program, route);
    }

    return program;
}

// Initialize parser
Parser *parser_init(Lexer *lexer)
{
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = NULL;
    advance(parser); // Get first token
    return parser;
}

// Free parser
void parser_free(Parser *parser)
{
    if (parser->current_token)
    {
        token_free(parser->current_token);
    }
    free(parser);
}

// Parse the program
ASTNode *parser_parse(Parser *parser)
{
    return parse_program(parser);
}
