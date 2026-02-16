#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "interpreter.h"

int main()
{
    // Example web language code
    const char *source =
        "// Simple web language example\n"
        "route \"/hello\" {\n"
        "    response \"Hello, World!\"\n"
        "}\n"
        "\n"
        "route \"/user\" {\n"
        "    name = \"John\"\n"
        "    age = 25\n"
        "    response html {\n"
        "        name\n"
        "    }\n"
        "}\n"
        "\n"
        "route \"/calc\" {\n"
        "    x = 10\n"
        "    y = 32\n"
        "    response \"Results calculated\"\n"
        "}\n";

    printf("=== Source Code ===\n%s\n", source);

    // Create lexer
    Lexer *lexer = lexer_init(source);

    // Create parser
    Parser *parser = parser_init(lexer);

    // Parse the program
    printf("\n=== Parsing ===\n");
    ASTNode *ast = parser_parse(parser);
    printf("Parse successful!\n");

    // Print the AST
    printf("\n=== Abstract Syntax Tree ===\n");
    ast_print(ast, 0);

    // Create interpreter and execute
    printf("\n=== Execution ===\n");
    Interpreter *interp = interpreter_init();
    interpreter_execute(interp, ast);

    // Cleanup
    interpreter_free(interp);
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);

    printf("\n=== Complete! ===\n");
    return 0;
}
