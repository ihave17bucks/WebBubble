# Contributing to WebBubble 🫧

Thank you for your interest in contributing to WebBubble! We're excited to have you here.

## Ways to Contribute

- 🐛 Report bugs
- 💡 Suggest features
- 📝 Improve documentation
- 🔧 Fix issues
- ✨ Add new features
- 📖 Write tutorials

## Getting Started

1. **Fork the repository**
2. **Clone your fork**

   ```bash
   git clone https://github.com/yourusername/webbubble.git
   cd webbubble
   ```

3. **Build the project**

   ```bash
   make
   ```

4. **Run tests**
   ```bash
   make test
   ./build/webbubble-demo
   ```

## Development Workflow

1. **Create a branch**

   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes**
   - C files: `src/*.c`, `include/*.h`
   - C++ files: `src/*.cpp`, `include/*.hpp`
   - Follow existing code style

3. **Test your changes**

   ```bash
   make clean && make
   ./build/webbubble-server
   ./build/webbubble-demo
   ```

4. **Commit your changes**

   ```bash
   git add .
   git commit -m "Add feature: description"
   ```

5. **Push to your fork**

   ```bash
   git push origin feature/your-feature-name
   ```

6. **Create a Pull Request**
   - Go to GitHub
   - Click "New Pull Request"
   - Describe your changes

## Code Style

### C Code

- Use 4 spaces for indentation
- Follow existing naming conventions
- Comment complex logic
- Keep functions focused and small

### C++ Code

- Use modern C++17 features
- Use smart pointers
- Follow RAII principles
- Provide C interface for all modules

### Commit Messages

- Use present tense ("Add feature" not "Added feature")
- Keep first line under 50 characters
- Add detailed description if needed

```
Add route parameter support

- Implement :id syntax in parser
- Add parameter extraction in HTTP server
- Update documentation
- Add tests
```

## Architecture Guidelines

### Core (C)

Use C for:

- Performance-critical code
- Language core (lexer, parser, interpreter)
- HTTP server
- Low-level operations

### Extensions (C++)

Use C++ for:

- Complex data structures
- String manipulation
- External library wrappers
- Advanced features

### C/C++ Interface

- Always provide C interface (`extern "C"`)
- Use opaque pointers
- Document clearly
- No exceptions across boundary

## Testing

- Test on Linux, macOS, and Windows (if possible)
- Test with both gcc and clang
- Verify no memory leaks
- Check performance impact

## Documentation

- Update README.md if needed
- Add/update doc comments
- Update docs/ files
- Add examples for new features

## Feature Requests

Open an issue with:

- Clear description
- Use case / motivation
- Example code (if applicable)
- Implementation ideas (optional)

## Bug Reports

Include:

- WebBubble version
- Operating system
- Steps to reproduce
- Expected vs actual behavior
- Code sample (if applicable)

## Questions?

- Open a GitHub Discussion
- Join our Discord
- Ask in issues

## Code of Conduct

Be respectful, inclusive, and constructive. We're all here to build something great together.

---

Thank you for contributing to WebBubble! 🫧
