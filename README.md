# cs451

![C++](https://img.shields.io/badge/c++-00599C?style=for-the-badge&logo=cplusplus&labelColor=00599C)
![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?style=for-the-badge&logo=opengl&labelColor=white)

**csed-451 directory by *hyunseong*, POSTECH 24**

## Development Environment
```
Complier/Interpreter:
MSVC

IDE: Code::Blocks 25.03
OS: Windows 11
```

### Environment Settings
1. Install **Code::Blocks**.

2. Replace the `default.conf` file.
   * This will update your compiler settings.

3. Create a new C++ console project.
   * Select the compiler named **Microsoft Visual C++ 2022**.
   * Ensure that the project contains only the **Release** configuration.

4. Go to **Project > Build Options**.
   * Under **Linker settings**, add `freeglut.lib` and `glew32.lib`.
   * Under **Search directories > Compiler** and **Search directories > Resource compiler**, add `opengl\include`.
   * Under **Search directories > Linker**, add `opengl\lib`.
