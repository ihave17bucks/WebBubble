# WebBubble Examples 🫧

Example `.bub` files showing what you can build with WebBubble!

## Files

### `features.bub`
Demonstrates basic WebBubble features:
- String concatenation
- Math operations
- Variables
- Route definitions
- HTML responses

**Run it:**
```bash
webbubble features.bub
```

### `complete-api.bub`
A production-ready API example featuring:
- 🎯 **Route parameters** - `/users/:id`
- 🔀 **Control flow** - if/else, while loops
- 🔧 **Functions** - Define and call functions
- 🗄️ **SQLite database** - CRUD operations
- 🔐 **Authentication** - User registration & login
- 📝 **Blog API** - Posts with pagination
- 🔍 **Search** - Filtering and queries

**Run it:**
```bash
webbubble complete-api.bub
```

## What's a .bub file? 🫧

`.bub` is the file extension for WebBubble programs!

- **Cute** - Easy to remember
- **Unique** - Stands out
- **On-brand** - Matches the bubble theme 🫧

Just like:
- Python uses `.py`
- JavaScript uses `.js`
- Go uses `.go`
- **WebBubble uses `.bub`!** 🫧

## Creating Your Own

1. **Create a file:**
   ```bash
   touch myapp.bub
   ```

2. **Write some code:**
   ```bubble
   route "/hello" {
       response "Hello from .bub! 🫧"
   }
   ```

3. **Run it:**
   ```bash
   webbubble myapp.bub
   ```

4. **Visit:**
   ```
   http://localhost:8080/hello
   ```

## Example Snippets

### Simple API
```bubble
// api.bub
route "/api/ping" {
    response "pong"
}
```

### With Database
```bubble
// users.bub
route "/api/users/:id" {
    db = database.connect("app.db")
    user = db.queryOne("SELECT * FROM users WHERE id = ?", id)
    response json { user }
}
```

### With Functions
```bubble
// greet.bub
function greet(name) {
    return "Hello, " + name + "! 🫧"
}

route "/greet/:name" {
    message = greet(name)
    response message
}
```

### Complete App
```bubble
// blog.bub
function getPost(id) {
    db = database.connect("blog.db")
    return db.queryOne("SELECT * FROM posts WHERE id = ?", id)
}

route "/posts/:id" {
    post = getPost(id)
    
    if post {
        response html {
            post.title
            post.content
        }
    } else {
        response status=404 "Post not found"
    }
}
```

## More Examples

Check out the full examples:
- `features.bub` - Basic features
- `complete-api.bub` - Production API

## Tips

- Use `.bub` for all WebBubble files
- Keep file names descriptive: `auth.bub`, `api.bub`, `blog.bub`
- One app per file, or split into modules
- Run with: `webbubble yourfile.bub`

---

**Happy bubbling!** 🫧✨
