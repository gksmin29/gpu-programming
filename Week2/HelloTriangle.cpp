#include <glad/glad.h>      // OpenGL 함수 포인터 로딩 라이브러리
#include <GLFW/glfw3.h>     // 윈도우 생성 및 입력 처리를 위한 라이브러리

#include <iostream>         // 콘솔 출력
#include <cmath>            // sin 함수 사용

#include <learnopengl/shader_s.h> // Shader 클래스를 사용하기 위한 헤더
#define USE_SHADER_CLASS          // Shader 클래스를 사용할지 여부 결정

// 윈도우 크기가 변경될 때 호출되는 콜백 함수
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// 키 입력 처리 함수
void processInput(GLFWwindow* window);

// 윈도우 크기 설정
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

#ifndef USE_SHADER_CLASS
// vertex shader 코드 문자열
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"    // 위치 입력 attribute
"layout (location = 1) in vec3 aColor;\n"  // 색상 입력 attribute
"out vec3 ourColor;\n"                     // fragment shader로 전달할 색상
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"      // 정점 위치를 clip space 좌표로 변환
"   ourColor = aColor;\n"                  // 색상을 fragment shader로 전달
"}\0";

// fragment shader 코드 문자열
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"        // 최종 출력 색상
"in vec3 ourColor;\n"          // vertex shader에서 전달된 색상
"uniform float ourBrightness;\n"  // CPU에서 전달받는 밝기 값

"void main()\n"
"{\n"
"   FragColor = ourBrightness*vec4(ourColor, 1.0f);\n" // 색상에 밝기 값을 곱해 출력
"}\n\0";
#endif


int main()
{
    // GLFW 라이브러리 초기화
    glfwInit();

    // 사용할 OpenGL 버전 설정 (3.3 Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    // macOS에서는 forward compatibility 필요
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 윈도우 생성
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

    // 윈도우 생성 실패 시 프로그램 종료
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 생성된 윈도우를 현재 OpenGL 컨텍스트로 설정
    glfwMakeContextCurrent(window);

    // 윈도우 크기 변경 시 호출할 콜백 함수 등록
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // GLAD를 통해 OpenGL 함수 포인터 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

#ifndef USE_SHADER_CLASS
    // Vertex Shader 객체 생성
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // shader 코드 전달
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // shader 컴파일
    glCompileShader(vertexShader);

    // 컴파일 성공 여부 확인
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    // 실패 시 로그 출력
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::VERTEX_SHADER\n" << infoLog << std::endl;
    }

    // Fragment Shader 생성
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // shader 코드 전달
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // shader 컴파일
    glCompileShader(fragmentShader);

    // 컴파일 성공 여부 확인
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    // 실패 시 로그 출력
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT_SHADER\n" << infoLog << std::endl;
    }

    // Shader Program 생성
    unsigned int shaderProgram = glCreateProgram();

    // Vertex Shader 연결
    glAttachShader(shaderProgram, vertexShader);

    // Fragment Shader 연결
    glAttachShader(shaderProgram, fragmentShader);

    // Shader Program 링크
    glLinkProgram(shaderProgram);

    // 링크 성공 여부 확인
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    // 실패 시 로그 출력
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINK\n" << infoLog << std::endl;
    }

    // shader 객체 삭제 (program에 이미 포함됨)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

#else
    // Shader 클래스 이용하여 shader 로드 및 컴파일
    Shader ourShader("shader/3.3.shader.vs", "shader/3.3.shader.fs");
#endif


    // 정점 데이터 (위치 + 색상)
    float vertices[] = {
        // position        // color
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    unsigned int VBO, VAO;

    // VAO 생성
    glGenVertexArrays(1, &VAO);

    // VBO 생성
    glGenBuffers(1, &VBO);

    // VAO 바인딩 (vertex attribute 설정 저장용)
    glBindVertexArray(VAO);

    // VBO 바인딩
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 정점 데이터를 GPU 메모리에 업로드
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute 설정
    glVertexAttribPointer(
        0,                    // location index
        3,                    // vec3
        GL_FLOAT,             // 데이터 타입
        GL_FALSE,             // 정규화 여부
        6 * sizeof(float),    // stride
        (void*)0              // 시작 offset
    );

    // attribute 활성화
    glEnableVertexAttribArray(0);

    // color attribute 설정
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    // attribute 활성화
    glEnableVertexAttribArray(1);


#ifdef USE_SHADER_CLASS
    // Shader 클래스 사용 시 shader 활성화
    ourShader.use();
#else
    // shader program 사용
    glUseProgram(shaderProgram);
#endif


    // 메인 렌더링 루프
    while (!glfwWindowShouldClose(window))
    {
        // 키 입력 처리
        processInput(window);

        // 화면 색 초기화
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // color buffer 초기화
        glClear(GL_COLOR_BUFFER_BIT);

        // 현재 시간 기반 밝기 계산
        float timeValue = glfwGetTime();

        float brightnessValue =
            static_cast<float>(sin(timeValue) / 2.0 + 0.5);

#ifdef USE_SHADER_CLASS
        // Shader 클래스 방식 uniform 전달
        ourShader.setFloat("ourBrightness", brightnessValue);
#else
        // uniform 변수 위치 얻기
        int brightnessLocation =
            glGetUniformLocation(shaderProgram, "ourBrightness");

        // uniform 값 전달
        glUniform1f(brightnessLocation, brightnessValue);
#endif

        // VAO 바인딩
        glBindVertexArray(VAO);

        // 삼각형 그리기
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // 프레임 버퍼 교체
        glfwSwapBuffers(window);

        // 이벤트 처리
        glfwPollEvents();
    }

    // VAO 삭제
    glDeleteVertexArrays(1, &VAO);

    // VBO 삭제
    glDeleteBuffers(1, &VBO);

#if 0
    glDeleteProgram(shaderProgram);
#endif

    // GLFW 종료
    glfwTerminate();

    return 0;
}


// ESC 키 입력 시 창 종료
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// 윈도우 크기 변경 시 viewport 재설정
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}