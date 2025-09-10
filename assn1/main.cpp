// build: g++ main.cpp -o app -lglew32 -lfreeglut -lopengl32   (Windows 예시)
//       clang++/g++ 환경에 맞게 라이브러리 이름만 조정
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <string>

static GLuint gProgram = 0;
static GLuint gVAO = 0, gVBO = 0;
static int gWidth = 800, gHeight = 600;

static const char* kVertSrc = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vColor;
void main() {
    vColor = aColor;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

static const char* kFragSrc = R"(#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

static GLuint compileShader(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = GL_FALSE;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len);
        glGetShaderInfoLog(sh, len, nullptr, log.data());
        std::cerr << "Shader compile error: " << log.data() << std::endl;
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

static GLuint linkProgram(GLuint vs, GLuint fs) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len);
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        std::cerr << "Program link error: " << log.data() << std::endl;
        glDeleteProgram(prog);
        return 0;
    }
    glDetachShader(prog, vs);
    glDetachShader(prog, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

void initGLObjects() {
    // 삼각형: (x, y, r, g, b)
    float vertices[] = {
        //   pos        // color
         0.0f,  0.8f,   1.0f, 0.2f, 0.2f,
        -0.8f, -0.6f,   0.2f, 1.0f, 0.2f,
         0.8f, -0.6f,   0.2f, 0.2f, 1.0f
    };

    // VAO/VBO
    glGenVertexArrays(1, &gVAO);
    glGenBuffers(1, &gVBO);

    glBindVertexArray(gVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // layout(location=0) vec2 aPos;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0
    );

    // layout(location=1) vec3 aColor;
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float))
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void display() {
    glViewport(0, 0, gWidth, gHeight);
    glClearColor(0.08f, 0.09f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(gProgram);
    glBindVertexArray(gVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);

    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int w, int h) {
    gWidth = w; gHeight = h;
}

void keyboard(unsigned char key, int, int) {
    if (key == 27 || key == 'q') { // ESC or q
        glutLeaveMainLoop();
    }
}

int main(int argc, char** argv) {
    // --- freeglut: Core Profile 3.3 요청 ---
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);   // 중요: Core Profile
    // glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG); // 선택

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(gWidth, gHeight);
    glutCreateWindow("OpenGL 3.3 Core (freeglut + GLEW)");

    // --- GLEW 초기화 (Core에서 필수) ---
    glewExperimental = GL_TRUE;                  // Core에서 확장 포인터 로드 위해
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW init error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
    // GLEW가 일부 플랫폼에서 GL_INVALID_ENUM을 띄우는 경우가 있어 오류 플래그 비우기
    glGetError();

    // 버전 출력
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "OpenGL:   " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL:     " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

    // 셰이더 프로그램
    GLuint vs = compileShader(GL_VERTEX_SHADER,   kVertSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragSrc);
    gProgram = linkProgram(vs, fs);
    if (!gProgram) return 1;

    // 버텍스 준비
    initGLObjects();

    // 콜백
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    // 정리(일부 드라이버는 자동 정리하긴 함)
    glDeleteBuffers(1, &gVBO);
    glDeleteVertexArrays(1, &gVAO);
    glDeleteProgram(gProgram);
    return 0;
}
