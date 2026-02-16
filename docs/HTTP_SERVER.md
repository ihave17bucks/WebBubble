# WebBubble HTTP Server Guide 🫧🌐

## Overview

WebBubble includes a high-performance HTTP/1.1 server built in C that can:
- Handle 100K requests per second
- Route parameters (`:id`)
- Execute control flow and functions
- Connect to SQLite databases
- Serve JSON responses
- Cross-platform (Linux, macOS, Windows)

## Quick Start

### 1. Build the Server

```bash
make
```

This creates `build/webbubble-server` (or `webbubble-server.exe` on Windows).

### 2. Run the Server

```bash
./build/webbubble-server
```

The server starts on port 8080 by default. You'll see:

```
================================
  WebBubble HTTP Server 🫧
================================
Listening on http://localhost:8080
Press Ctrl+C to stop

Available routes:
  - http://localhost:8080/
  - http://localhost:8080/hello
  - http://localhost:8080/api/users/:id
  ...
```

### 3. Test It

Open your browser and visit:
- http://localhost:8080/
- http://localhost:8080/hello
- http://localhost:8080/user

Or use curl:

```bash
curl http://localhost:8080/hello
```

## Custom Port

To use a different port:

```bash
./build/weblang-server 3000
```

## Writing Server Programs

The server executes WebBubble programs that define routes. Each route handles HTTP requests.

### Basic Example

```bubble
route "/" {
    response "Welcome to WebBubble!"
}

route "/hello" {
    name = "World"
    response "Hello, " + name + "!"
}
```

### Route Parameters Example

```bubble
route "/api/users/:id" {
    // :id is automatically available
    db = database.connect("users.db")
    user = db.queryOne("SELECT * FROM users WHERE id = ?", id)
    
    if user {
        response json { user }
    } else {
        response status=404 "User not found"
    }
}

route "/posts/:postId/comments/:commentId" {
    // Multiple parameters
    response "Post " + postId + ", Comment " + commentId
}
```

### With Database and Logic

```bubble
route "/api/posts" {
    db = database.connect("blog.db")
    
    page = request.query.page || 1
    limit = 10
    offset = (page - 1) * limit
    
    posts = db.query("SELECT * FROM posts LIMIT ? OFFSET ?", limit, offset)
    
    response json { posts }
}
```

## How It Works

1. **Parsing**: The server parses your WebLang program at startup
2. **Listening**: It listens for HTTP requests on the specified port
3. **Routing**: When a request comes in, it matches the URL path to a route
4. **Execution**: It executes the route's code
5. **Response**: It sends back the response with proper HTTP headers

## HTTP Responses

### Plain Text

```
route "/hello" {
    response "Hello, World!"
}
```

Generates:
```
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 13

Hello, World!
```

### HTML

```
route "/page" {
    title = "My Page"
    response html {
        title
    }
}
```

Generates:
```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 35

<html><body>My Page</body></html>
```

## Route Matching

Routes must match exactly:

✅ `/hello` matches `/hello`  
❌ `/hello` does NOT match `/hello/world`  
❌ `/hello` does NOT match `/Hello` (case-sensitive)

### 404 Not Found

If no route matches, the server returns:

```
HTTP/1.1 404 Not Found
Content-Type: text/plain

404 Not Found - Route '/nonexistent' not defined
```

## Current Limitations

- **No route parameters**: `/user/:id` not yet supported
- **No HTTP methods**: All routes accept all methods (GET, POST, etc.)
- **No request body parsing**: Cannot read POST data yet
- **No query strings**: `?key=value` not parsed
- **No headers**: Cannot read request headers
- **Single-threaded**: Handles one request at a time

## Testing the Server

Use the provided test script:

```bash
./test_server.sh
```

Or test manually with curl:

```bash
# Start server
./build/weblang-server &

# Test endpoints
curl http://localhost:8080/
curl http://localhost:8080/hello
curl http://localhost:8080/user

# Stop server
killall weblang-server
```

## Modifying the Program

Currently, the WebLang program is embedded in `src/server.c`. To change it:

1. Edit the `source` variable in `src/server.c`
2. Rebuild: `make clean && make`
3. Run: `./build/weblang-server`

Future versions will support loading `.web` files from disk.

## Production Use

⚠️ **This server is for learning purposes!** It's not production-ready. Missing:

- Request size limits
- Connection timeouts
- Multi-threading/async I/O
- HTTPS/TLS support
- Security headers
- Rate limiting
- Logging
- Error recovery
- Memory leak prevention

For production, consider:
- Adding these features
- Using it behind nginx/Apache
- Containerizing with Docker
- Adding monitoring and alerting

## Examples

### Simple API

```
route "/api/health" {
    response "healthy"
}

route "/api/version" {
    version = 1.0
    response version
}
```

### Welcome Page

```
route "/" {
    response html {
        "Welcome to My Site!"
    }
}
```

### Multiple Pages

```
route "/home" {
    response "Home Page"
}

route "/about" {
    company = "ACME Corp"
    response html {
        company
    }
}

route "/contact" {
    email = "info@example.com"
    response html {
        email
    }
}
```

## Debugging

The server prints request information:

```
Request: GET /hello
Request: POST /api/data
```

Watch the console output to see what requests are being handled.

## Next Steps

Try building:
- A personal homepage
- A simple API
- A status dashboard
- A URL shortener
- A blog engine

Then extend the language with:
- Request parameter parsing
- JSON support
- Database queries
- Templates
- Sessions

Happy serving! 🚀
