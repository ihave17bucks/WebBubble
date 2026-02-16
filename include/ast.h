#ifndef AST_H
#define AST_H

// Types of AST nodes
typedef enum {
    AST_PROGRAM,
    AST_ROUTE,
    AST_RESPONSE,
    AST_ASSIGNMENT,
    AST_IDENTIFIER,
    AST_STRING,
    AST_NUMBER,
    AST_BLOCK,
    AST_BINARY_OP,
    AST_IF,
    AST_WHILE,
    AST_FUNCTION,
    AST_FUNCTION_CALL,
    AST_RETURN
} ASTNodeType;

// Forward declaration
typedef struct ASTNode ASTNode;

// AST Node structure
struct ASTNode {
    ASTNodeType type;
    
    // Different data depending on node type
    union {
        // For AST_PROGRAM: list of routes
        struct {
            ASTNode **routes;
            int route_count;
        } program;
        
        // For AST_ROUTE: path and body
        struct {
            char *path;
            ASTNode *body;  // Block node
        } route;
        
        // For AST_RESPONSE: value
        struct {
            ASTNode *value;
            int is_html;
        } response;
        
        // For AST_ASSIGNMENT: name and value
        struct {
            char *name;
            ASTNode *value;
        } assignment;
        
        // For AST_IDENTIFIER
        struct {
            char *name;
        } identifier;
        
        // For AST_STRING
        struct {
            char *value;
        } string;
        
        // For AST_NUMBER
        struct {
            double value;
        } number;
        
        // For AST_BLOCK: list of statements
        struct {
            ASTNode **statements;
            int statement_count;
        } block;
        
        // For AST_BINARY_OP: left op right
        struct {
            char *operator;
            ASTNode *left;
            ASTNode *right;
        } binary_op;
        
        // For AST_IF: condition, then_branch, else_branch
        struct {
            ASTNode *condition;
            ASTNode *then_branch;
            ASTNode *else_branch;  // Can be NULL
        } if_stmt;
        
        // For AST_WHILE: condition, body
        struct {
            ASTNode *condition;
            ASTNode *body;
        } while_stmt;
        
        // For AST_FUNCTION: name, params, body
        struct {
            char *name;
            char **params;
            int param_count;
            ASTNode *body;
        } function;
        
        // For AST_FUNCTION_CALL: name, arguments
        struct {
            char *name;
            ASTNode **args;
            int arg_count;
        } function_call;
        
        // For AST_RETURN: value
        struct {
            ASTNode *value;
        } return_stmt;
    } data;
};

// AST node creation functions
ASTNode* ast_create_program();
ASTNode* ast_create_route(char *path, ASTNode *body);
ASTNode* ast_create_response(ASTNode *value, int is_html);
ASTNode* ast_create_assignment(char *name, ASTNode *value);
ASTNode* ast_create_identifier(char *name);
ASTNode* ast_create_string(char *value);
ASTNode* ast_create_number(double value);
ASTNode* ast_create_block();
ASTNode* ast_create_binary_op(const char *operator, ASTNode *left, ASTNode *right);
ASTNode* ast_create_if(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);
ASTNode* ast_create_while(ASTNode *condition, ASTNode *body);
ASTNode* ast_create_function(const char *name, char **params, int param_count, ASTNode *body);
ASTNode* ast_create_function_call(const char *name, ASTNode **args, int arg_count);
ASTNode* ast_create_return(ASTNode *value);

// Helper functions
void ast_program_add_route(ASTNode *program, ASTNode *route);
void ast_block_add_statement(ASTNode *block, ASTNode *statement);
void ast_free(ASTNode *node);
void ast_print(ASTNode *node, int indent);

#endif
