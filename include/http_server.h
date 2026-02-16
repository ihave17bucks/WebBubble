#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "ast.h"
#include "interpreter.h"

// HTTP request structure
typedef struct {
    char *method;      // GET, POST, etc.
    char *path;        // /hello, /user, etc.
    char *version;     // HTTP/1.1
    char *headers;     // Raw headers
    char *body;        // Request body
} HTTPRequest;

// HTTP response structure
typedef struct {
    int status_code;
    char *status_text;
    char *content_type;
    char *body;
} HTTPResponse;

// HTTP server
typedef struct {
    int port;
    int socket_fd;
    ASTNode *program;
    Interpreter *interpreter;
} HTTPServer;

// Server functions
HTTPServer* http_server_create(int port, ASTNode *program);
void http_server_start(HTTPServer *server);
void http_server_stop(HTTPServer *server);
void http_server_free(HTTPServer *server);

// Request/Response functions
HTTPRequest* http_request_parse(const char *raw_request);
void http_request_free(HTTPRequest *request);
HTTPResponse* http_response_create(int status_code, const char *content_type, const char *body);
char* http_response_to_string(HTTPResponse *response);
void http_response_free(HTTPResponse *response);

// Route matching
ASTNode* find_matching_route(ASTNode *program, const char *path, Interpreter *interp);

#endif
