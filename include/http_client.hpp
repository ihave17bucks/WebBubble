// WebBubble HTTP Client Module (C++)
// Makes HTTP requests to external APIs

#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <string>
#include <map>
#include <memory>

extern "C" {

// Opaque handle for HTTP client
typedef void* HttpClient;
typedef void* HttpResponse;

// Create HTTP client
HttpClient http_client_create();

// Free HTTP client
void http_client_free(HttpClient client);

// Make GET request
HttpResponse http_get(HttpClient client, const char* url);

// Make POST request with data
HttpResponse http_post(HttpClient client, const char* url, const char* data);

// Make PUT request
HttpResponse http_put(HttpClient client, const char* url, const char* data);

// Make DELETE request
HttpResponse http_delete(HttpClient client, const char* url);

// Set request headers
void http_set_header(HttpClient client, const char* key, const char* value);

// Get response status code
int http_response_status(HttpResponse response);

// Get response body as string
const char* http_response_body(HttpResponse response);

// Get response header
const char* http_response_header(HttpResponse response, const char* key);

// Free response
void http_response_free(HttpResponse response);

}

#endif // HTTP_CLIENT_HPP
