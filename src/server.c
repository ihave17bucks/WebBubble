#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// Global server for signal handling
HTTPServer *global_server = NULL;

// Signal handler for graceful shutdown
void signal_handler(int signum)
{
    printf("\n\nShutting down server...\n");
    if (global_server)
    {
        http_server_stop(global_server);
        http_server_free(global_server);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int port = 8080;

    // Check for port argument
    if (argc > 1)
    {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535)
        {
            fprintf(stderr, "Invalid port number. Using default: 8080\n");
            port = 8080;
        }
    }

    // Example WebBubble code
    const char *source =
        "route \"/\" {\n"
        "    response \"Welcome to WebBubble! 🫧\"\n"
        "}\n"
        "\n"
        "route \"/hello\" {\n"
        "    greeting = \"Hello\"\n"
        "    name = \"World\"\n"
        "    message = greeting + \", \" + name + \"!\"\n"
        "    response message\n"
        "}\n"
        "\n"
        "route \"/calc\" {\n"
        "    x = 10\n"
        "    y = 5\n"
        "    sum = x + y\n"
        "    product = x * y\n"
        "    result = \"Sum: \" + sum + \", Product: \" + product\n"
        "    response result\n"
        "}\n"
        "\n"
        "route \"/user\" {\n"
        "    name = \"Alice\"\n"
        "    age = 28\n"
        "    email = \"alice@example.com\"\n"
        "    info = name + \" (\" + age + \") - \" + email\n"
        "    response html {\n"
        "        info\n"
        "    }\n"
        "}\n"
        "\n"
        "route \"/about\" {\n"
        "    title = \"About WebBubble\"\n"
        "    version = 1.0\n"
        "    info = title + \" v\" + version\n"
        "    response html {\n"
        "        info\n"
        "    }\n"
        "}\n"
        "\n"
        "route \"/api/status\" {\n"
        "    status = \"OK\"\n"
        "    uptime = 100\n"
        "    response status\n"
        "}\n";

    printf("=== WebBubble HTTP Server ===\n\n");
    printf("Parsing program...\n");

    // Create lexer and parser
    Lexer *lexer = lexer_init(source);
    Parser *parser = parser_init(lexer);

    // Parse the program
    ASTNode *ast = parser_parse(parser);
    printf("Parse successful!\n");

    // Cleanup parser and lexer (keep AST)
    parser_free(parser);
    lexer_free(lexer);

    // Create and start HTTP server
    global_server = http_server_create(port, ast);

    // Setup signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Start the server (this blocks)
    http_server_start(global_server);

    // Cleanup (only reached if server stops)
    http_server_free(global_server);
    ast_free(ast);

    return 0;
}
