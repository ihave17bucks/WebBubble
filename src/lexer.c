#include "lexer.h"

// Helper function to advance the lexer position
static void advance(Lexer *lexer)
{
    if (lexer->current_char == '\n')
    {
        lexer->line++;
        lexer->column = 0;
    }
    else
    {
        lexer->column++;
    }

    lexer->position++;
    if (lexer->position < strlen(lexer->source))
    {
        lexer->current_char = lexer->source[lexer->position];
    }
    else
    {
        lexer->current_char = '\0';
    }
}

// Skip whitespace
static void skip_whitespace(Lexer *lexer)
{
    while (lexer->current_char != '\0' && isspace(lexer->current_char))
    {
        advance(lexer);
    }
}

// Skip single-line comments (// ...)
static void skip_comment(Lexer *lexer)
{
    if (lexer->current_char == '/' && lexer->position + 1 < strlen(lexer->source) && lexer->source[lexer->position + 1] == '/')
    {
        // Skip until end of line
        while (lexer->current_char != '\0' && lexer->current_char != '\n')
        {
            advance(lexer);
        }
    }
}

// Create a new token
static Token *token_create(TokenType type, const char *value, int line, int column)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value = value ? strdup(value) : NULL;
    token->line = line;
    token->column = column;
    return token;
}

// Read a string literal
static Token *read_string(Lexer *lexer)
{
    int line = lexer->line;
    int column = lexer->column;
    char buffer[1024];
    int i = 0;

    advance(lexer); // Skip opening quote

    while (lexer->current_char != '\0' && lexer->current_char != '"')
    {
        if (i >= 1023)
        {
            fprintf(stderr, "String too long at line %d\n", line);
            exit(1);
        }
        buffer[i++] = lexer->current_char;
        advance(lexer);
    }

    if (lexer->current_char == '"')
    {
        advance(lexer); // Skip closing quote
    }
    else
    {
        fprintf(stderr, "Unterminated string at line %d\n", line);
        exit(1);
    }

    buffer[i] = '\0';
    return token_create(TOKEN_STRING, buffer, line, column);
}

// Read a number
static Token *read_number(Lexer *lexer)
{
    int line = lexer->line;
    int column = lexer->column;
    char buffer[256];
    int i = 0;

    while (lexer->current_char != '\0' && (isdigit(lexer->current_char) || lexer->current_char == '.'))
    {
        if (i >= 255)
        {
            fprintf(stderr, "Number too long at line %d\n", line);
            exit(1);
        }
        buffer[i++] = lexer->current_char;
        advance(lexer);
    }

    buffer[i] = '\0';
    return token_create(TOKEN_NUMBER, buffer, line, column);
}

// Read an identifier or keyword
static Token *read_identifier(Lexer *lexer)
{
    int line = lexer->line;
    int column = lexer->column;
    char buffer[256];
    int i = 0;

    while (lexer->current_char != '\0' && (isalnum(lexer->current_char) || lexer->current_char == '_'))
    {
        if (i >= 255)
        {
            fprintf(stderr, "Identifier too long at line %d\n", line);
            exit(1);
        }
        buffer[i++] = lexer->current_char;
        advance(lexer);
    }

    buffer[i] = '\0';

    // Check for keywords
    if (strcmp(buffer, "route") == 0)
    {
        return token_create(TOKEN_ROUTE, buffer, line, column);
    }
    else if (strcmp(buffer, "response") == 0)
    {
        return token_create(TOKEN_RESPONSE, buffer, line, column);
    }
    else if (strcmp(buffer, "html") == 0)
    {
        return token_create(TOKEN_HTML, buffer, line, column);
    }
    else if (strcmp(buffer, "if") == 0)
    {
        return token_create(TOKEN_IF, buffer, line, column);
    }
    else if (strcmp(buffer, "else") == 0)
    {
        return token_create(TOKEN_ELSE, buffer, line, column);
    }
    else if (strcmp(buffer, "while") == 0)
    {
        return token_create(TOKEN_WHILE, buffer, line, column);
    }
    else if (strcmp(buffer, "for") == 0)
    {
        return token_create(TOKEN_FOR, buffer, line, column);
    }
    else if (strcmp(buffer, "function") == 0)
    {
        return token_create(TOKEN_FUNCTION, buffer, line, column);
    }
    else if (strcmp(buffer, "return") == 0)
    {
        return token_create(TOKEN_RETURN, buffer, line, column);
    }

    return token_create(TOKEN_IDENTIFIER, buffer, line, column);
}

// Initialize lexer
Lexer *lexer_init(const char *source)
{
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 0;
    lexer->current_char = source[0];
    return lexer;
}

// Free lexer
void lexer_free(Lexer *lexer)
{
    free(lexer);
}

// Get next token
Token *lexer_next_token(Lexer *lexer)
{
    while (lexer->current_char != '\0')
    {
        // Skip whitespace
        if (isspace(lexer->current_char))
        {
            skip_whitespace(lexer);
            continue;
        }

        // Skip comments
        if (lexer->current_char == '/' && lexer->position + 1 < strlen(lexer->source) && lexer->source[lexer->position + 1] == '/')
        {
            skip_comment(lexer);
            continue;
        }

        int line = lexer->line;
        int column = lexer->column;

        // String literals
        if (lexer->current_char == '"')
        {
            return read_string(lexer);
        }

        // Numbers
        if (isdigit(lexer->current_char))
        {
            return read_number(lexer);
        }

        // Identifiers and keywords
        if (isalpha(lexer->current_char) || lexer->current_char == '_')
        {
            return read_identifier(lexer);
        }

        // Single character tokens
        char current = lexer->current_char;
        advance(lexer);

        // Check for two-character operators
        if (current == '=' && lexer->current_char == '=')
        {
            advance(lexer);
            return token_create(TOKEN_EQ, "==", line, column);
        }
        if (current == '!' && lexer->current_char == '=')
        {
            advance(lexer);
            return token_create(TOKEN_NEQ, "!=", line, column);
        }
        if (current == '<' && lexer->current_char == '=')
        {
            advance(lexer);
            return token_create(TOKEN_LTE, "<=", line, column);
        }
        if (current == '>' && lexer->current_char == '=')
        {
            advance(lexer);
            return token_create(TOKEN_GTE, ">=", line, column);
        }
        if (current == '&' && lexer->current_char == '&')
        {
            advance(lexer);
            return token_create(TOKEN_AND, "&&", line, column);
        }
        if (current == '|' && lexer->current_char == '|')
        {
            advance(lexer);
            return token_create(TOKEN_OR, "||", line, column);
        }

        switch (current)
        {
        case '{':
            return token_create(TOKEN_LBRACE, "{", line, column);
        case '}':
            return token_create(TOKEN_RBRACE, "}", line, column);
        case '(':
            return token_create(TOKEN_LPAREN, "(", line, column);
        case ')':
            return token_create(TOKEN_RPAREN, ")", line, column);
        case '=':
            return token_create(TOKEN_EQUALS, "=", line, column);
        case '+':
            return token_create(TOKEN_PLUS, "+", line, column);
        case '-':
            return token_create(TOKEN_MINUS, "-", line, column);
        case '*':
            return token_create(TOKEN_STAR, "*", line, column);
        case '/':
            return token_create(TOKEN_SLASH, "/", line, column);
        case '.':
            return token_create(TOKEN_DOT, ".", line, column);
        case ',':
            return token_create(TOKEN_COMMA, ",", line, column);
        case ':':
            return token_create(TOKEN_COLON, ":", line, column);
        case ';':
            return token_create(TOKEN_SEMICOLON, ";", line, column);
        case '<':
            return token_create(TOKEN_LT, "<", line, column);
        case '>':
            return token_create(TOKEN_GT, ">", line, column);
        default:
            fprintf(stderr, "Unknown character '%c' at line %d, column %d\n",
                    current, line, column);
            return token_create(TOKEN_UNKNOWN, NULL, line, column);
        }
    }

    return token_create(TOKEN_EOF, NULL, lexer->line, lexer->column);
}

// Free token
void token_free(Token *token)
{
    if (token->value)
    {
        free(token->value);
    }
    free(token);
}

// Convert token type to string
const char *token_type_to_string(TokenType type)
{
    switch (type)
    {
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_ROUTE:
        return "ROUTE";
    case TOKEN_RESPONSE:
        return "RESPONSE";
    case TOKEN_HTML:
        return "HTML";
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_STRING:
        return "STRING";
    case TOKEN_NUMBER:
        return "NUMBER";
    case TOKEN_LBRACE:
        return "LBRACE";
    case TOKEN_RBRACE:
        return "RBRACE";
    case TOKEN_LPAREN:
        return "LPAREN";
    case TOKEN_RPAREN:
        return "RPAREN";
    case TOKEN_EQUALS:
        return "EQUALS";
    case TOKEN_PLUS:
        return "PLUS";
    case TOKEN_MINUS:
        return "MINUS";
    case TOKEN_STAR:
        return "STAR";
    case TOKEN_SLASH:
        return "SLASH";
    case TOKEN_DOT:
        return "DOT";
    case TOKEN_COMMA:
        return "COMMA";
    case TOKEN_COLON:
        return "COLON";
    case TOKEN_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_LT:
        return "LT";
    case TOKEN_GT:
        return "GT";
    case TOKEN_LTE:
        return "LTE";
    case TOKEN_GTE:
        return "GTE";
    case TOKEN_EQ:
        return "EQ";
    case TOKEN_NEQ:
        return "NEQ";
    case TOKEN_AND:
        return "AND";
    case TOKEN_OR:
        return "OR";
    case TOKEN_UNKNOWN:
        return "UNKNOWN";
    default:
        return "INVALID";
    }
}

// Print token
void token_print(Token *token)
{
    printf("Token(%s", token_type_to_string(token->type));
    if (token->value)
    {
        printf(", '%s'", token->value);
    }
    printf(", line=%d, col=%d)\n", token->line, token->column);
}
