#include "interpreter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ===== VALUE FUNCTIONS =====

Value value_create_string(const char *str)
{
    Value val;
    val.type = VAL_STRING;
    val.data.string = strdup(str);
    return val;
}

Value value_create_number(double num)
{
    Value val;
    val.type = VAL_NUMBER;
    val.data.number = num;
    return val;
}

Value value_create_null()
{
    Value val;
    val.type = VAL_NULL;
    return val;
}

void value_free(Value *val)
{
    if (val->type == VAL_STRING && val->data.string)
    {
        free(val->data.string);
    }
}

void value_print(Value *val, FILE *output)
{
    switch (val->type)
    {
    case VAL_STRING:
        fprintf(output, "%s", val->data.string);
        break;
    case VAL_NUMBER:
        fprintf(output, "%g", val->data.number);
        break;
    case VAL_BOOL:
        fprintf(output, "%s", val->data.boolean ? "true" : "false");
        break;
    case VAL_NULL:
        fprintf(output, "null");
        break;
    }
}

char *value_to_string(Value *val)
{
    char buffer[256];
    switch (val->type)
    {
    case VAL_STRING:
        return strdup(val->data.string);
    case VAL_NUMBER:
        snprintf(buffer, sizeof(buffer), "%g", val->data.number);
        return strdup(buffer);
    case VAL_BOOL:
        return strdup(val->data.boolean ? "true" : "false");
    case VAL_NULL:
        return strdup("null");
    }
    return strdup("");
}

// ===== VARIABLE STORAGE =====

void set_variable(Interpreter *interp, const char *name, Value value)
{
    // Check if variable already exists
    Variable *var = interp->variables;
    while (var)
    {
        if (strcmp(var->name, name) == 0)
        {
            // Update existing variable
            value_free(&var->value);
            var->value = value;
            return;
        }
        var = var->next;
    }

    // Create new variable
    Variable *new_var = (Variable *)malloc(sizeof(Variable));
    new_var->name = strdup(name);
    new_var->value = value;
    new_var->next = interp->variables;
    interp->variables = new_var;
}

static Value *get_variable(Interpreter *interp, const char *name)
{
    Variable *var = interp->variables;
    while (var)
    {
        if (strcmp(var->name, name) == 0)
        {
            return &var->value;
        }
        var = var->next;
    }
    return NULL;
}

// ===== INTERPRETER CORE =====

// Forward declarations
static Value eval_expression(Interpreter *interp, ASTNode *node);
void execute_statement(Interpreter *interp, ASTNode *node); // Not static - used by HTTP server

// Evaluate an expression node and return its value
static Value eval_expression(Interpreter *interp, ASTNode *node)
{
    switch (node->type)
    {
    case AST_STRING:
        return value_create_string(node->data.string.value);

    case AST_NUMBER:
        return value_create_number(node->data.number.value);

    case AST_IDENTIFIER:
    {
        Value *var = get_variable(interp, node->data.identifier.name);
        if (var)
        {
            // Return a copy
            if (var->type == VAL_STRING)
            {
                return value_create_string(var->data.string);
            }
            else if (var->type == VAL_NUMBER)
            {
                return value_create_number(var->data.number);
            }
        }
        fprintf(stderr, "Runtime error: Undefined variable '%s'\n",
                node->data.identifier.name);
        return value_create_null();
    }

    case AST_BLOCK:
    {
        // For blocks in HTML context, concatenate identifiers
        Value result = value_create_string("");
        for (int i = 0; i < node->data.block.statement_count; i++)
        {
            ASTNode *stmt = node->data.block.statements[i];
            if (stmt->type == AST_IDENTIFIER)
            {
                // Look up the variable and append it
                Value *var = get_variable(interp, stmt->data.identifier.name);
                if (var)
                {
                    char *old = result.data.string;
                    char *var_str = value_to_string(var);

                    size_t new_len = strlen(old) + strlen(var_str) + 2;
                    char *new_str = malloc(new_len);
                    snprintf(new_str, new_len, "%s%s", old, var_str);

                    free(old);
                    free(var_str);
                    result.data.string = new_str;
                }
            }
        }
        return result;
    }

    case AST_BINARY_OP:
    {
        Value left = eval_expression(interp, node->data.binary_op.left);
        Value right = eval_expression(interp, node->data.binary_op.right);
        Value result;

        const char *op = node->data.binary_op.operator;

        // Arithmetic operations
        if (strcmp(op, "+") == 0)
        {
            if (left.type == VAL_STRING || right.type == VAL_STRING)
            {
                // String concatenation
                char *left_str = value_to_string(&left);
                char *right_str = value_to_string(&right);
                size_t len = strlen(left_str) + strlen(right_str) + 1;
                char *concat = malloc(len);
                snprintf(concat, len, "%s%s", left_str, right_str);
                result = value_create_string(concat);
                free(concat);
                free(left_str);
                free(right_str);
            }
            else if (left.type == VAL_NUMBER && right.type == VAL_NUMBER)
            {
                result = value_create_number(left.data.number + right.data.number);
            }
            else
            {
                result = value_create_null();
            }
        }
        else if (strcmp(op, "-") == 0 && left.type == VAL_NUMBER && right.type == VAL_NUMBER)
        {
            result = value_create_number(left.data.number - right.data.number);
        }
        else if (strcmp(op, "*") == 0 && left.type == VAL_NUMBER && right.type == VAL_NUMBER)
        {
            result = value_create_number(left.data.number * right.data.number);
        }
        else if (strcmp(op, "/") == 0 && left.type == VAL_NUMBER && right.type == VAL_NUMBER)
        {
            if (right.data.number != 0)
            {
                result = value_create_number(left.data.number / right.data.number);
            }
            else
            {
                fprintf(stderr, "Runtime error: Division by zero\n");
                result = value_create_null();
            }
        }
        // Comparison operations
        else if (strcmp(op, "<") == 0 && left.type == VAL_NUMBER && right.type == VAL_NUMBER)
        {
            result.type = VAL_BOOL;
            result.data.boolean = left.data.number < right.data.number;
        }
        else if (strcmp(op, ">") == 0 && left.type == VAL_NUMBER && right.type == VAL_NUMBER)
        {
            result.type = VAL_BOOL;
            result.data.boolean = left.data.number > right.data.number;
        }
        else if (strcmp(op, "<=") == 0 && left.type == VAL_NUMBER && right.type == VAL_NUMBER)
        {
            result.type = VAL_BOOL;
            result.data.boolean = left.data.number <= right.data.number;
        }
        else if (strcmp(op, ">=") == 0 && left.type == VAL_NUMBER && right.type == VAL_NUMBER)
        {
            result.type = VAL_BOOL;
            result.data.boolean = left.data.number >= right.data.number;
        }
        else if (strcmp(op, "==") == 0)
        {
            if (left.type == VAL_NUMBER && right.type == VAL_NUMBER)
            {
                result.type = VAL_BOOL;
                result.data.boolean = left.data.number == right.data.number;
            }
            else if (left.type == VAL_STRING && right.type == VAL_STRING)
            {
                result.type = VAL_BOOL;
                result.data.boolean = strcmp(left.data.string, right.data.string) == 0;
            }
            else
            {
                result.type = VAL_BOOL;
                result.data.boolean = 0;
            }
        }
        else if (strcmp(op, "!=") == 0)
        {
            if (left.type == VAL_NUMBER && right.type == VAL_NUMBER)
            {
                result.type = VAL_BOOL;
                result.data.boolean = left.data.number != right.data.number;
            }
            else if (left.type == VAL_STRING && right.type == VAL_STRING)
            {
                result.type = VAL_BOOL;
                result.data.boolean = strcmp(left.data.string, right.data.string) != 0;
            }
            else
            {
                result.type = VAL_BOOL;
                result.data.boolean = 1;
            }
        }
        else
        {
            result = value_create_null();
        }

        value_free(&left);
        value_free(&right);
        return result;
    }

    default:
        return value_create_null();
    }
}

// Execute a statement node
void execute_statement(Interpreter *interp, ASTNode *node)
{
    switch (node->type)
    {
    case AST_ASSIGNMENT:
    {
        Value value = eval_expression(interp, node->data.assignment.value);
        set_variable(interp, node->data.assignment.name, value);
        break;
    }

    case AST_RESPONSE:
    {
        Value value = eval_expression(interp, node->data.response.value);

        if (node->data.response.is_html)
        {
            fprintf(interp->output, "Content-Type: text/html\n\n");
            fprintf(interp->output, "<html><body>");
            value_print(&value, interp->output);
            fprintf(interp->output, "</body></html>\n");
        }
        else
        {
            fprintf(interp->output, "Content-Type: text/plain\n\n");
            value_print(&value, interp->output);
            fprintf(interp->output, "\n");
        }

        value_free(&value);
        break;
    }

    case AST_BLOCK:
    {
        for (int i = 0; i < node->data.block.statement_count; i++)
        {
            execute_statement(interp, node->data.block.statements[i]);
        }
        break;
    }

    default:
        break;
    }
}

// Execute a route
static void execute_route(Interpreter *interp, ASTNode *route)
{
    fprintf(interp->output, "\n=== Executing Route: %s ===\n",
            route->data.route.path);
    execute_statement(interp, route->data.route.body);
}

// Execute the entire program
static void execute_program(Interpreter *interp, ASTNode *program)
{
    for (int i = 0; i < program->data.program.route_count; i++)
    {
        execute_route(interp, program->data.program.routes[i]);
    }
}

// ===== PUBLIC API =====

Interpreter *interpreter_init()
{
    Interpreter *interp = (Interpreter *)malloc(sizeof(Interpreter));
    interp->variables = NULL;
    interp->output = stdout;
    return interp;
}

void interpreter_free(Interpreter *interp)
{
    // Free all variables
    Variable *var = interp->variables;
    while (var)
    {
        Variable *next = var->next;
        free(var->name);
        value_free(&var->value);
        free(var);
        var = next;
    }

    free(interp);
}

void interpreter_execute(Interpreter *interp, ASTNode *ast)
{
    if (ast->type == AST_PROGRAM)
    {
        execute_program(interp, ast);
    }
    else
    {
        fprintf(stderr, "Interpreter error: Expected program node\n");
    }
}
