// WebBubble HTTP Client Implementation (C++)
// Uses libcurl for HTTP requests

#include "http_client.hpp"
#include <curl/curl.h>
#include <map>
#include <string>
#include <memory>
#include <cstring>

namespace {
    // Internal HTTP client class
    class HttpClientImpl {
    public:
        std::map<std::string, std::string> headers;
        long timeout = 30;
    };

    // Internal HTTP response class
    class HttpResponseImpl {
    public:
        int status_code = 0;
        std::string body;
        std::map<std::string, std::string> headers;
    };

    // Callback for libcurl to write response data
    size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t total_size = size * nmemb;
        std::string* str = static_cast<std::string*>(userp);
        str->append(static_cast<char*>(contents), total_size);
        return total_size;
    }

    // Perform HTTP request
    HttpResponseImpl* perform_request(HttpClientImpl* client, const char* url, 
                                      const char* method, const char* data = nullptr) {
        CURL* curl = curl_easy_init();
        if (!curl) return nullptr;

        auto response = new HttpResponseImpl();
        
        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL, url);
        
        // Set method
        if (strcmp(method, "POST") == 0) {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            if (data) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
            }
        } else if (strcmp(method, "PUT") == 0) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            if (data) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
            }
        } else if (strcmp(method, "DELETE") == 0) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        }
        
        // Set write callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response->body);
        
        // Set headers
        struct curl_slist* header_list = nullptr;
        for (const auto& [key, value] : client->headers) {
            std::string header = key + ": " + value;
            header_list = curl_slist_append(header_list, header.c_str());
        }
        if (header_list) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        }
        
        // Set timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, client->timeout);
        
        // Perform request
        CURLcode res = curl_easy_perform(curl);
        
        // Get status code
        if (res == CURLE_OK) {
            long status_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
            response->status_code = static_cast<int>(status_code);
        }
        
        // Cleanup
        if (header_list) {
            curl_slist_free_all(header_list);
        }
        curl_easy_cleanup(curl);
        
        return response;
    }
}

extern "C" {

HttpClient http_client_create() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    return new HttpClientImpl();
}

void http_client_free(HttpClient client) {
    if (client) {
        delete static_cast<HttpClientImpl*>(client);
        curl_global_cleanup();
    }
}

HttpResponse http_get(HttpClient client, const char* url) {
    auto impl = static_cast<HttpClientImpl*>(client);
    return perform_request(impl, url, "GET");
}

HttpResponse http_post(HttpClient client, const char* url, const char* data) {
    auto impl = static_cast<HttpClientImpl*>(client);
    return perform_request(impl, url, "POST", data);
}

HttpResponse http_put(HttpClient client, const char* url, const char* data) {
    auto impl = static_cast<HttpClientImpl*>(client);
    return perform_request(impl, url, "PUT", data);
}

HttpResponse http_delete(HttpClient client, const char* url) {
    auto impl = static_cast<HttpClientImpl*>(client);
    return perform_request(impl, url, "DELETE");
}

void http_set_header(HttpClient client, const char* key, const char* value) {
    auto impl = static_cast<HttpClientImpl*>(client);
    impl->headers[key] = value;
}

int http_response_status(HttpResponse response) {
    if (!response) return 0;
    auto impl = static_cast<HttpResponseImpl*>(response);
    return impl->status_code;
}

const char* http_response_body(HttpResponse response) {
    if (!response) return nullptr;
    auto impl = static_cast<HttpResponseImpl*>(response);
    return impl->body.c_str();
}

const char* http_response_header(HttpResponse response, const char* key) {
    if (!response) return nullptr;
    auto impl = static_cast<HttpResponseImpl*>(response);
    auto it = impl->headers.find(key);
    if (it != impl->headers.end()) {
        return it->second.c_str();
    }
    return nullptr;
}

void http_response_free(HttpResponse response) {
    if (response) {
        delete static_cast<HttpResponseImpl*>(response);
    }
}

}
