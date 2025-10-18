#include <SDL.h>
#include <GLES2/gl2.h>
#include <iostream>
#include <cmath>

// Шейдеры для OpenGL ES 2.0
const char* vertexShaderSource = R"(
attribute vec2 position;
attribute vec3 color;
varying vec3 vColor;
void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    vColor = color;
}
)";

const char* fragmentShaderSource = R"(
precision mediump float;
varying vec3 vColor;
void main()
{
    gl_FragColor = vec4(vColor, 1.0);
}
)";

// Функция для компиляции шейдера
GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Проверка компиляции
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Ошибка компиляции шейдера:\n" << infoLog << std::endl;
    }
    return shader;
}

// Функция для создания шейдерной программы
GLuint createShaderProgram()
{
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Проверка линковки
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Ошибка линковки шейдерной программы:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main(int argc, char* argv[])
{
    // Инициализация SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Ошибка инициализации SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Настройка атрибутов OpenGL ES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Создание окна (полноэкранный режим)
    SDL_Window* window = SDL_CreateWindow("SDL2 + OpenGL ES 2.0 Triangle",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED,
                                         0, 0,  // Размеры не важны в полноэкранном режиме
                                         SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    if (!window)
    {
        std::cerr << "Ошибка создания окна: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Создание контекста OpenGL ES
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << "Ошибка создания OpenGL ES контекста: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Данные вершин треугольника (позиция и цвет)
    float vertices[] = {
        // position    // color
        -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // левая нижняя - красный
         0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // правая нижняя - зеленый
         0.0f,  0.5f,  0.0f, 0.0f, 1.0f   // верхняя - синий
    };

    // Создание шейдерной программы
    GLuint shaderProgram = createShaderProgram();

    // Создание VBO (Vertex Buffer Object)
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Настройка атрибутов вершин
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    // Основной цикл
    bool running = true;
    SDL_Event event;

    while (running)
    {
        // Обработка событий
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
            }
        }

        // Очистка экрана
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Рисование треугольника
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Обновление экрана
        SDL_GL_SwapWindow(window);

        // Небольшая задержка
        SDL_Delay(16);
    }

    // Очистка ресурсов
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
