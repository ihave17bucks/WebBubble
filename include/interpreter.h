#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include <stdio.h>

// Value types that can be stored at runtime
typedef enum {
    VAL_NULL,
    VAL_STRING,
    VAL_NUMBER,
    VAL_BOOL
} ValueType;

// Runtime value
typedef struct {
    ValueType type;
    union {
        char *string;
        double number;
        int boolean;
    } data;
} Value;

// Variable storage (simple symbol table)
typedef struct Variable {
    char *name;
    Value value;
    struct Variable *next;
} Variable;

// Interpreter context
typedef struct {
    Variable *variables;  // Linked list of variables
    FILE *output;         // Where to write output (stdout or file)
} Interpreter;

// Interpreter functions
Interpreter* interpreter_init();
void interpreter_free(Interpreter *interp);
void interpreter_execute(Interpreter *interp, ASTNode *ast);
void execute_statement(Interpreter *interp, ASTNode *node);  // Exposed for HTTP server
void set_variable(Interpreter *interp, const char *name, Value value);  // Set variable value

// Value functions
Value value_create_string(const char *str);
Value value_create_number(double num);
Value value_create_null();
void value_free(Value *val);
void value_print(Value *val, FILE *output);
char* value_to_string(Value *val);

#endif
