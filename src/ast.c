#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Create program node
ASTNode *ast_create_program()
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_PROGRAM;
    node->data.program.routes = NULL;
    node->data.program.route_count = 0;
    return node;
}

// Create route node
ASTNode *ast_create_route(char *path, ASTNode *body)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_ROUTE;
    node->data.route.path = strdup(path);
    node->data.route.body = body;
    return node;
}

// Create response node
ASTNode *ast_create_response(ASTNode *value, int is_html)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_RESPONSE;
    node->data.response.value = value;
    node->data.response.is_html = is_html;
    return node;
}

// Create assignment node
ASTNode *ast_create_assignment(char *name, ASTNode *value)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->data.assignment.name = strdup(name);
    node->data.assignment.value = value;
    return node;
}

// Create identifier node
ASTNode *ast_create_identifier(char *name)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->data.identifier.name = strdup(name);
    return node;
}

// Create string node
ASTNode *ast_create_string(char *value)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_STRING;
    node->data.string.value = strdup(value);
    return node;
}

// Create number node
ASTNode *ast_create_number(double value)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->data.number.value = value;
    return node;
}

// Create block node
ASTNode *ast_create_block()
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_BLOCK;
    node->data.block.statements = NULL;
    node->data.block.statement_count = 0;
    return node;
}

// Create binary operation node
ASTNode *ast_create_binary_op(const char *operator, ASTNode *left, ASTNode *right)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_BINARY_OP;
    node->data.binary_op.operator = strdup(operator);
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

// Add route to program
void ast_program_add_route(ASTNode *program, ASTNode *route)
{
    if (program->type != AST_PROGRAM)
        return;

    int count = program->data.program.route_count;
    program->data.program.routes = (ASTNode **)realloc(
        program->data.program.routes,
        sizeof(ASTNode *) * (count + 1));
    program->data.program.routes[count] = route;
    program->data.program.route_count++;
}

// Add statement to block
void ast_block_add_statement(ASTNode *block, ASTNode *statement)
{
    if (block->type != AST_BLOCK)
        return;

    int count = block->data.block.statement_count;
    block->data.block.statements = (ASTNode **)realloc(
        block->data.block.statements,
        sizeof(ASTNode *) * (count + 1));
    block->data.block.statements[count] = statement;
    block->data.block.statement_count++;
}

// Free AST node and all children
void ast_free(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_PROGRAM:
        for (int i = 0; i < node->data.program.route_count; i++)
        {
            ast_free(node->data.program.routes[i]);
        }
        free(node->data.program.routes);
        break;

    case AST_ROUTE:
        free(node->data.route.path);
        ast_free(node->data.route.body);
        break;

    case AST_RESPONSE:
        ast_free(node->data.response.value);
        break;

    case AST_ASSIGNMENT:
        free(node->data.assignment.name);
        ast_free(node->data.assignment.value);
        break;

    case AST_IDENTIFIER:
        free(node->data.identifier.name);
        break;

    case AST_STRING:
        free(node->data.string.value);
        break;

    case AST_BLOCK:
        for (int i = 0; i < node->data.block.statement_count; i++)
        {
            ast_free(node->data.block.statements[i]);
        }
        free(node->data.block.statements);
        break;

    case AST_BINARY_OP:
        free(node->data.binary_op.operator);
        ast_free(node->data.binary_op.left);
        ast_free(node->data.binary_op.right);
        break;

    case AST_NUMBER:
        // Nothing to free
        break;
    }

    free(node);
}

// Print AST (for debugging)
void ast_print(ASTNode *node, int indent)
{
    if (!node)
        return;

    for (int i = 0; i < indent; i++)
        printf("  ");

    switch (node->type)
    {
    case AST_PROGRAM:
        printf("Program (%d routes)\n", node->data.program.route_count);
        for (int i = 0; i < node->data.program.route_count; i++)
        {
            ast_print(node->data.program.routes[i], indent + 1);
        }
        break;

    case AST_ROUTE:
        printf("Route: %s\n", node->data.route.path);
        ast_print(node->data.route.body, indent + 1);
        break;

    case AST_RESPONSE:
        printf("Response %s\n", node->data.response.is_html ? "(HTML)" : "");
        ast_print(node->data.response.value, indent + 1);
        break;

    case AST_ASSIGNMENT:
        printf("Assignment: %s =\n", node->data.assignment.name);
        ast_print(node->data.assignment.value, indent + 1);
        break;

    case AST_IDENTIFIER:
        printf("Identifier: %s\n", node->data.identifier.name);
        break;

    case AST_STRING:
        printf("String: \"%s\"\n", node->data.string.value);
        break;

    case AST_NUMBER:
        printf("Number: %g\n", node->data.number.value);
        break;

    case AST_BLOCK:
        printf("Block (%d statements)\n", node->data.block.statement_count);
        for (int i = 0; i < node->data.block.statement_count; i++)
        {
            ast_print(node->data.block.statements[i], indent + 1);
        }
        break;

    case AST_BINARY_OP:
        printf("BinaryOp: %s\n", node->data.binary_op.operator);
        ast_print(node->data.binary_op.left, indent + 1);
        ast_print(node->data.binary_op.right, indent + 1);
        break;
    }
}
