# WebBubble Quick Start Guide 🫧

## What is WebBubble?

WebBubble is a friendly, fast programming language for building modern web systems. It combines:
- The simplicity of Python/Ruby
- The performance of C/Go  
- Built-in database and HTTP server
- Route parameters, control flow, and functions

Perfect for APIs, microservices, and web apps!

## Installation

1. Extract the project archive
2. Navigate to the directory
3. Build the project:
```bash
make
```

## Your First API

Create a file `api.bub`:
```bubble
route "/api/users/:id" {
    // Route parameters work automatically!
    db = database.connect("users.db")
    user = db.queryOne("SELECT * FROM users WHERE id = ?", id)
    
    if user {
        response json { user }
    } else {
        response status=404 "User not found"
    }
}
```

Run it:
```bash
./build/webbubble-server api.bub
```

Test it:
```bash
curl http://localhost:8080/api/users/123
```

## Language Basics

### Variables
```
name = "Alice"
age = 25
price = 99.99
```

### Routes with Parameters
```bubble
route "/users/:id" {
    response "User ID: " + id
}

route "/posts/:postId/comments/:commentId" {
    response "Post " + postId + ", Comment " + commentId
}
```

### Control Flow
```bubble
route "/check/:age" {
    ageNum = age + 0
    
    if ageNum >= 18 {
        status = "adult"
    } else {
        status = "minor"
    }
    
    response status
}
```

### Functions
```bubble
function greet(name) {
    return "Hello, " + name + "!"
}

route "/greet/:name" {
    message = greet(name)
    response message
}
```

### Database
```bubble
route "/api/users" {
    db = database.connect("app.db")
    users = db.query("SELECT * FROM users")
    response json { users }
}
```

### Plain Text Response
```
route "/hello" {
    message = "Hello!"
    response message
}
```

### HTML Response
```
route "/profile" {
    username = "Bob"
    response html {
        username
    }
}
```

## Example Programs

### Simple Welcome Page
```
route "/welcome" {
    response "Welcome to WebLang!"
}
```

### User Profile
```
route "/user" {
    name = "Alice"
    age = 28
    email = "alice@example.com"
    response html {
        name
    }
}
```

### Multiple Routes
```
route "/home" {
    response "Home Page"
}

route "/about" {
    company = "WebLang Inc"
    year = 2026
    response html {
        company
    }
}

route "/contact" {
    response "Contact Us"
}
```

## Output Format

When you run a program, you'll see:
1. **Source Code** - Your program
2. **Parsing** - Confirms successful parsing
3. **AST** - Tree structure of your program
4. **Execution** - Output from each route

Example output:
```
=== Executing Route: /hello ===
Content-Type: text/plain

Hello, World!
```

## Common Commands

Build the project:
```bash
make
```

Clean build files:
```bash
make clean
```

Build and run:
```bash
make run
```

See all commands:
```bash
make help
```

## Next Steps

- Read the full language spec: `docs/LANGUAGE_SPEC.md`
- Look at examples: `examples/` directory
- Modify the interpreter to add new features!
- Add control flow (if/else)
- Add loops
- Add functions
- Add HTTP server integration

## Project Structure

```
my-web-language/
├── src/           # C source files
├── include/       # Header files
├── build/         # Compiled executable
├── examples/      # Example .web programs
└── docs/          # Documentation
```

## Getting Help

- Check `README.md` for project overview
- Read `docs/LANGUAGE_SPEC.md` for detailed syntax
- Look at `examples/*.web` for working programs

Happy coding! 🚀
