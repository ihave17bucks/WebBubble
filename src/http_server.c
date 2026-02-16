#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Cross-platform socket includes
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define close closesocket
typedef int socklen_t;
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#define BUFFER_SIZE 4096

// ===== HTTP REQUEST PARSING =====

HTTPRequest *http_request_parse(const char *raw_request)
{
    HTTPRequest *request = (HTTPRequest *)malloc(sizeof(HTTPRequest));

    // Make a copy we can modify
    char *request_copy = strdup(raw_request);
    char *line = strtok(request_copy, "\r\n");

    if (line)
    {
        // Parse request line: "GET /path HTTP/1.1"
        char method[16], path[256], version[16];
        if (sscanf(line, "%15s %255s %15s", method, path, version) == 3)
        {
            request->method = strdup(method);
            request->path = strdup(path);
            request->version = strdup(version);
        }
        else
        {
            request->method = strdup("GET");
            request->path = strdup("/");
            request->version = strdup("HTTP/1.1");
        }

        // Skip headers for now (simple implementation)
        request->headers = strdup("");
        request->body = strdup("");
    }

    free(request_copy);
    return request;
}

void http_request_free(HTTPRequest *request)
{
    if (!request)
        return;
    free(request->method);
    free(request->path);
    free(request->version);
    free(request->headers);
    free(request->body);
    free(request);
}

// ===== HTTP RESPONSE =====

HTTPResponse *http_response_create(int status_code, const char *content_type, const char *body)
{
    HTTPResponse *response = (HTTPResponse *)malloc(sizeof(HTTPResponse));
    response->status_code = status_code;

    switch (status_code)
    {
    case 200:
        response->status_text = strdup("OK");
        break;
    case 404:
        response->status_text = strdup("Not Found");
        break;
    case 500:
        response->status_text = strdup("Internal Server Error");
        break;
    default:
        response->status_text = strdup("Unknown");
        break;
    }

    response->content_type = strdup(content_type);
    response->body = strdup(body);
    return response;
}

char *http_response_to_string(HTTPResponse *response)
{
    size_t size = 512 + strlen(response->body);
    char *result = (char *)malloc(size);

    snprintf(result, size,
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             response->status_code,
             response->status_text,
             response->content_type,
             strlen(response->body),
             response->body);

    return result;
}

void http_response_free(HTTPResponse *response)
{
    if (!response)
        return;
    free(response->status_text);
    free(response->content_type);
    free(response->body);
    free(response);
}

// ===== ROUTE MATCHING =====

// Check if a route path matches with parameters (e.g., /user/:id matches /user/123)
static int route_matches(const char *route_pattern, const char *request_path,
                         char params[][256], char values[][256], int *param_count)
{
    *param_count = 0;

    // Split both paths by '/'
    char pattern_copy[256], path_copy[256];
    strncpy(pattern_copy, route_pattern, 255);
    strncpy(path_copy, request_path, 255);

    char *pattern_parts[32];
    char *path_parts[32];
    int pattern_count = 0, path_count = 0;

    // Tokenize pattern
    char *p = strtok(pattern_copy, "/");
    while (p && pattern_count < 32)
    {
        pattern_parts[pattern_count++] = p;
        p = strtok(NULL, "/");
    }

    // Tokenize path
    p = strtok(path_copy, "/");
    while (p && path_count < 32)
    {
        path_parts[path_count++] = p;
        p = strtok(NULL, "/");
    }

    // Must have same number of parts
    if (pattern_count != path_count)
        return 0;

    // Check each part
    for (int i = 0; i < pattern_count; i++)
    {
        if (pattern_parts[i][0] == ':')
        {
            // This is a parameter
            strncpy(params[*param_count], pattern_parts[i] + 1, 255);
            strncpy(values[*param_count], path_parts[i], 255);
            (*param_count)++;
        }
        else
        {
            // Must match exactly
            if (strcmp(pattern_parts[i], path_parts[i]) != 0)
            {
                return 0;
            }
        }
    }

    return 1;
}

ASTNode *find_matching_route(ASTNode *program, const char *path, Interpreter *interp)
{
    if (program->type != AST_PROGRAM)
        return NULL;

    for (int i = 0; i < program->data.program.route_count; i++)
    {
        ASTNode *route = program->data.program.routes[i];

        // Check for exact match first
        if (strcmp(route->data.route.path, path) == 0)
        {
            return route;
        }

        // Check for parameter match
        char params[8][256], values[8][256];
        int param_count = 0;
        if (route_matches(route->data.route.path, path, params, values, &param_count))
        {
            // Store parameters as variables in interpreter
            for (int j = 0; j < param_count; j++)
            {
                Value val = value_create_string(values[j]);
                set_variable(interp, params[j], val);
            }
            return route;
        }
    }

    return NULL;
}

// ===== HTTP SERVER =====

HTTPServer *http_server_create(int port, ASTNode *program)
{
    HTTPServer *server = (HTTPServer *)malloc(sizeof(HTTPServer));
    server->port = port;
    server->socket_fd = -1;
    server->program = program;
    server->interpreter = interpreter_init();
    return server;
}

void http_server_free(HTTPServer *server)
{
    if (!server)
        return;
    if (server->socket_fd >= 0)
    {
        close(server->socket_fd);
    }
    interpreter_free(server->interpreter);
    free(server);
}

// Handle a single client request
static void handle_client(HTTPServer *server, int client_fd)
{
    char buffer[BUFFER_SIZE];
#ifdef _WIN32
    int bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
#else
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
#endif

    if (bytes_read <= 0)
    {
        close(client_fd);
        return;
    }

    buffer[bytes_read] = '\0';

    // Parse request
    HTTPRequest *request = http_request_parse(buffer);
    printf("Request: %s %s\n", request->method, request->path);

    // Find matching route (will inject params into interpreter)
    ASTNode *route = find_matching_route(server->program, request->path, server->interpreter);

    HTTPResponse *response;

    if (route)
    {
        // Create a memory buffer to capture output
        char output_buffer[4096] = {0};

#ifdef _WIN32
        // Windows: use temporary file
        FILE *mem_stream = tmpfile();
#else
        // Unix: use fmemopen
        FILE *mem_stream = fmemopen(output_buffer, sizeof(output_buffer), "w");
#endif

        if (mem_stream)
        {
            // Redirect interpreter output to memory
            server->interpreter->output = mem_stream;

            // Execute the route
            execute_statement(server->interpreter, route->data.route.body);

            fflush(mem_stream);

#ifdef _WIN32
            // Windows: read back from temp file
            rewind(mem_stream);
            size_t read_size = fread(output_buffer, 1, sizeof(output_buffer) - 1, mem_stream);
            output_buffer[read_size] = '\0';
#endif

            fclose(mem_stream);

            // Reset output to stdout
            server->interpreter->output = stdout;

            // Parse the output to extract content type and body
            char *content_type = "text/plain";
            char *body_start = output_buffer;

            if (strstr(output_buffer, "Content-Type: text/html") != NULL)
            {
                content_type = "text/html";
                // Find the actual body (after blank line)
                body_start = strstr(output_buffer, "\n\n");
                if (body_start)
                    body_start += 2;
            }
            else if (strstr(output_buffer, "Content-Type: text/plain") != NULL)
            {
                content_type = "text/plain";
                body_start = strstr(output_buffer, "\n\n");
                if (body_start)
                    body_start += 2;
            }

            response = http_response_create(200, content_type, body_start ? body_start : output_buffer);
        }
        else
        {
            response = http_response_create(500, "text/plain", "Internal Server Error");
        }

        // Clear variables for next request
        interpreter_free(server->interpreter);
        server->interpreter = interpreter_init();
    }
    else
    {
        // 404 Not Found
        char not_found[256];
        snprintf(not_found, sizeof(not_found),
                 "404 Not Found - Route '%s' not defined", request->path);
        response = http_response_create(404, "text/plain", not_found);
    }

    // Send response
    char *response_str = http_response_to_string(response);
#ifdef _WIN32
    send(client_fd, response_str, strlen(response_str), 0);
#else
    write(client_fd, response_str, strlen(response_str));
#endif

    // Cleanup
    free(response_str);
    http_response_free(response);
    http_request_free(request);
    close(client_fd);
}

void http_server_start(HTTPServer *server)
{
    struct sockaddr_in address;
    int opt = 1;

#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        fprintf(stderr, "WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }
#endif

    // Create socket
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (server->socket_fd == INVALID_SOCKET)
    {
#else
    if (server->socket_fd == -1)
    {
#endif
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

// Set socket options
#ifdef _WIN32
    if (setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)))
    {
#else
    if (setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
#endif
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server->port);

    if (bind(server->socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server->socket_fd, 10) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("\n");
    printf("================================\n");
    printf("  WebBubble HTTP Server 🫧\n");
    printf("================================\n");
    printf("Listening on http://localhost:%d\n", server->port);
    printf("Press Ctrl+C to stop\n\n");

    // Print available routes
    printf("Available routes:\n");
    for (int i = 0; i < server->program->data.program.route_count; i++)
    {
        ASTNode *route = server->program->data.program.routes[i];
        printf("  - http://localhost:%d%s\n", server->port, route->data.route.path);
    }
    printf("\n");

    // Accept connections loop
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server->socket_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        handle_client(server, client_fd);
    }
}

void http_server_stop(HTTPServer *server)
{
    if (server->socket_fd >= 0)
    {
        close(server->socket_fd);
        server->socket_fd = -1;
    }
#ifdef _WIN32
    WSACleanup();
#endif
}

// Expose execute_statement for HTTP server use
void execute_statement(Interpreter *interp, ASTNode *node);
