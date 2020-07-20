#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "matrix.h"
#include "gol.h"

#define GRID_WIDTH  168
#define GRID_HEIGHT 76
#define CELL_WIDTH  10
#define CELL_HEIGHT 10

uint32_t matrix[GRID_WIDTH * GRID_HEIGHT];

SDL_Window *window;
GLuint vbos[GRID_WIDTH * GRID_HEIGHT];
GLuint program;
GLuint attribute_coord2d;

char* read_file(const char* filename) {
    FILE *file   = NULL;
    char *buffer = NULL;
    size_t size  = 0;

    file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "I couldn't open the file...\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    buffer = (char*)malloc(size * sizeof(char) + sizeof(char));

    fread(buffer, sizeof(char), size, file);
    fclose(file);
    
    buffer[size] = '\0';

    return buffer;
}

void free_resources() {
    glDeleteProgram(program);

    glDeleteBuffers(GRID_WIDTH * GRID_HEIGHT, vbos);

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void init_sdl_gl() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("GOL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GRID_WIDTH * CELL_WIDTH, GRID_HEIGHT * CELL_HEIGHT, SDL_WINDOW_OPENGL);

    if (window == NULL) {
        fprintf(stderr, "Couldn't create SDL Window: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GL_CreateContext(window);

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        fprintf(stderr, "Error initializing glew: %s\n", glewGetErrorString(glew_status));
        exit(1);
    }
}

void init_vbos() {
    glGenBuffers(GRID_WIDTH * GRID_HEIGHT, vbos);

    float x_ratio = 2.0f / (GRID_WIDTH  * CELL_WIDTH);
    float y_ratio = 2.0f / (GRID_HEIGHT * CELL_HEIGHT);

    for (size_t y = 0; y < GRID_HEIGHT; y++) {
        for (size_t x = 0; x < GRID_WIDTH; x++) {
            float _x = x * CELL_WIDTH;
            float _y = y * CELL_HEIGHT;

            GLfloat vertices[] = {
                _x                * x_ratio - 1.0f, _y                 * y_ratio - 1.0f, // top-left
                (_x + CELL_WIDTH) * x_ratio - 1.0f, _y                 * y_ratio - 1.0f, // top-right
                (_x + CELL_WIDTH) * x_ratio - 1.0f, (_y + CELL_HEIGHT) * y_ratio - 1.0f, // bottom-right

                _x                * x_ratio - 1.0f, _y                 * y_ratio - 1.0f, // top-left
                _x                * x_ratio - 1.0f, (_y + CELL_HEIGHT) * y_ratio - 1.0f, // bottom-left
                (_x + CELL_WIDTH) * x_ratio - 1.0f, (_y + CELL_HEIGHT) * y_ratio - 1.0f  // bottom-right
            };

            printf("%f - %f\n", (x + CELL_WIDTH) * x_ratio - 1.0f, y * y_ratio - 1.0f);

            // GLfloat vertices[] = {
            //     x               , y, // top-left
            //     (x + CELL_WIDTH), y, // top-right
            //     (x + CELL_WIDTH), (y + CELL_HEIGHT), // bottom-right

            //     x               , y                , // top-left
            //     x               , (y + CELL_HEIGHT), // bottom-left
            //     (x + CELL_WIDTH), (y + CELL_HEIGHT)  // bottom-right
            // };

            glBindBuffer(GL_ARRAY_BUFFER, vbos[y * GRID_WIDTH + x]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        }
    }
}

void init_shader_program() {
    program = glCreateProgram();

    GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint f_shader = glCreateShader(GL_FRAGMENT_SHADER);

    GLint v_compile_ok = GL_FALSE;
    GLint f_compile_ok = GL_FALSE;
    GLint link_ok      = GL_FALSE;

    const char *v_src = read_file("./shaders/cell.v.glsl");
    const char *f_src = read_file("./shaders/cell.f.glsl");

    if (v_src == NULL) {
        fprintf(stderr, "Could not read vertex shader file\n");
        exit(1);
    }

    if (f_src == NULL) {
        fprintf(stderr, "Could not read fragment shader file\n");
        exit(1);
    }

    glShaderSource(v_shader, 1, &v_src, NULL);
    glShaderSource(f_shader, 1, &f_src, NULL);

    glCompileShader(v_shader);
    glCompileShader(f_shader);

    glGetShaderiv(v_shader, GL_COMPILE_STATUS, &v_compile_ok);
    glGetShaderiv(f_shader, GL_COMPILE_STATUS, &f_compile_ok);

    if (!v_compile_ok) {
        fprintf(stderr, "Could not compile vertex shader\n");
        exit(1);
    }

    if (!f_compile_ok) {
        fprintf(stderr, "Could not compile fragment shader\n");
        exit(1);
    }

    glAttachShader(program, v_shader);
    glAttachShader(program, f_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);

    if (!link_ok) {
        fprintf(stderr, "Could not link program\n");
        exit(1);
    }

    attribute_coord2d = glGetAttribLocation(program, "coord2d");

    if (attribute_coord2d == -1) {
        fprintf(stderr, "Could not bind attribute coord2d");
        exit(1);
    }

    free((void*)v_src);
    free((void*)f_src);

    v_src = NULL;
    f_src = NULL;
}

void init_matrix() {
    fill_white_noise(matrix, GRID_WIDTH, GRID_HEIGHT);
}

void init() {
    srand(time(NULL));

    init_sdl_gl();
    init_vbos();
    init_shader_program();

    init_matrix();
}

void render() {
    glClearColor(0.8, 0.6, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    size_t len = GRID_WIDTH * GRID_HEIGHT;
    for (size_t i = 0; i < len; i++) {
        if (!matrix[i])
            continue;

        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);

        glEnableVertexAttribArray(attribute_coord2d);
        glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    SDL_GL_SwapWindow(window);
}

void main_loop() {
    uint32_t last_tick = SDL_GetTicks();
    SDL_Event e;
    uint8_t running = 1;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;
        }

        render();

        if (SDL_GetTicks() - last_tick >= 0) {
            gol_iterate(matrix, GRID_WIDTH, GRID_HEIGHT);

            last_tick = SDL_GetTicks();
        }
    }
}

int main(int argc, char **argv) {
    printf("Game of Life!\n");

    init();
    main_loop();

    free_resources();

    return 0;
}