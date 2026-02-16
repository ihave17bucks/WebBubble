# WebBubble Language Specification v1.1

## Overview
WebBubble is a friendly, fast programming language designed for building modern web systems. It combines the simplicity of scripting languages with the performance of compiled languages.

Version: 1.1  
Status: Production Ready

## Key Features
- Route parameters (`:id`)
- Control flow (if/else, while)
- Functions with recursion
- Built-in SQLite database
- Cross-platform (Linux, macOS, Windows)

## Syntax

### Comments
```
// Single-line comment
```

### Data Types
- **String**: `"Hello, World!"`
- **Number**: `42`, `3.14`
- **Identifier**: Variable names like `name`, `age`, `user_id`

### Variables
Assignment syntax:
```
variable_name = value
```

Examples:
```
name = "Alice"
age = 25
price = 99.99
```

### Routes
Define web routes with the `route` keyword:
```
route "/path" {
    // statements
}
```

Routes represent HTTP endpoints. Each route has a path and a body containing statements.

**Route Parameters:**
```
route "/users/:id" {
    // :id is automatically available as a variable
    response "User ID: " + id
}

route "/posts/:postId/comments/:commentId" {
    // Multiple parameters supported
    response "Post " + postId + ", Comment " + commentId
}
```

Parameters in the route path (prefixed with `:`) are automatically extracted and made available as variables.

### Responses
Send responses using the `response` keyword:

**Plain text response:**
```
response "Hello, World!"
```

**HTML response:**
```
response html {
    variable_name
}
```

Variables in HTML blocks are automatically interpolated.

### Complete Example
```
route "/welcome" {
    message = "Welcome to WebLang!"
    response message
}

route "/user" {
    name = "Bob"
    age = 30
    response html {
        name
    }
}
```

## Execution Model

1. **Parse**: Source code is tokenized and parsed into an AST
2. **Execute**: Each route is executed in order
3. **Output**: Responses include HTTP-like headers and content

Example output:
```
=== Executing Route: /welcome ===
Content-Type: text/plain

Welcome to WebLang!
```

## Control Flow

### If/Else Statements
```
if condition {
    // statements
} else if another_condition {
    // statements  
} else {
    // statements
}
```

Example:
```
route "/check/:age" {
    ageNum = age + 0
    
    if ageNum >= 18 {
        status = "adult"
    } else if ageNum >= 13 {
        status = "teen"
    } else {
        status = "child"
    }
    
    response status
}
```

**Comparison Operators:**
- `==` Equal
- `!=` Not equal
- `<` Less than
- `>` Greater than
- `<=` Less than or equal
- `>=` Greater than or equal

**Logical Operators:**
- `&&` AND
- `||` OR

### While Loops
```
while condition {
    // statements
}
```

Example:
```
count = 0
while count < 10 {
    print(count)
    count = count + 1
}
```

## Functions

### Definition
```
function name(param1, param2) {
    // statements
    return value
}
```

### Calling Functions
```
result = functionName(arg1, arg2)
```

### Examples
```
function greet(name) {
    return "Hello, " + name + "!"
}

function fibonacci(n) {
    if n <= 1 {
        return n
    }
    return fibonacci(n - 1) + fibonacci(n - 2)
}

route "/api/greet/:name" {
    message = greet(name)
    response message
}
```

## Database Operations

### Connecting
```
db = database.connect("filename.db")
```

### Querying
```
// Multiple rows
users = db.query("SELECT * FROM users")

// Single row
user = db.queryOne("SELECT * FROM users WHERE id = ?", id)

// With parameters (prevents SQL injection)
user = db.queryOne("SELECT * FROM users WHERE email = ?", email)
```

### Executing
```
// Insert
result = db.execute("INSERT INTO users (name, email) VALUES (?, ?)", name, email)
id = result.lastInsertId

// Update
result = db.execute("UPDATE users SET name = ? WHERE id = ?", newName, id)

// Delete
result = db.execute("DELETE FROM users WHERE id = ?", id)
```

### Transactions
```
db.execute("BEGIN TRANSACTION")
// Multiple operations
db.execute("COMMIT")
```

## Future Features

### Planned Extensions

**For loops:**
```
for item in list {
    // statements
}
```

**Request body parsing:**
```
data = request.body
name = request.body.name
```

**Middleware:**
```
middleware auth() {
    // Check authentication
}
```

**Functions:**
```
function greet(name) {
    response "Hello, " + name
}
```

**HTTP methods:**
```
route GET "/users" {
    // handle GET request
}

route POST "/users" {
    // handle POST request
}
```

**Database queries:**
```
route "/users/:id" {
    user = database.query("SELECT * FROM users WHERE id = :id")
    response html {
        user.name
    }
}
```

## Language Grammar (EBNF-style)

```
program     = route*

route       = "route" STRING block

block       = "{" statement* "}"

statement   = assignment
            | response
            | identifier

assignment  = IDENTIFIER "=" expression

response    = "response" expression
            | "response" "html" block

expression  = STRING
            | NUMBER
            | IDENTIFIER

STRING      = '"' [^"]* '"'
NUMBER      = [0-9]+ ("." [0-9]+)?
IDENTIFIER  = [a-zA-Z_][a-zA-Z0-9_]*
```

## Implementation Details

### Components
1. **Lexer**: Tokenizes source code
2. **Parser**: Builds Abstract Syntax Tree
3. **Interpreter**: Executes the AST

### File Extensions
- `.web` - WebLang source files

### Compilation
WebLang is currently interpreted. Future versions may include:
- Bytecode compilation
- JIT compilation
- Native code generation via LLVM
