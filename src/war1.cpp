#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include <stdarg.h>
#include <commdlg.h>

#include <SDL2-2.0.8\include\SDL.h>
#undef main

#include <glew32\include\glew.h>
#include <SDL2-2.0.8\include\SDL_opengl.h>

#include "stretchy_buffer.h"
#include "utils.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

global GLuint program;
global GLint positionLocation;
global GLint colorLocation;
global GLint texCoordLocation;
global GLuint vbo = 0;
global GLuint ibo = 0;

internal b32 loadTexture()
{
    SDL_Surface *surface = SDL_LoadBMP(".\\build\\cat.bmp");
    if (!surface)
    {
        return FALSE;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(surface);
}

internal void printProgramLog(GLuint program)
{
    if(glIsProgram(program))
    {
        int infoLogLength = 0;
        int maxLength = infoLogLength;
        
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        
        char* infoLog = (char*)malloc(maxLength);
        
        glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
        if(infoLogLength > 0)
        {
            printf("%s\n", infoLog);
        }
        
        free(infoLog);
    }
    else
    {
        printf( "Name %d is not a program\n", program );
    }
}

internal void printShaderLog(GLuint shader)
{
    if (glIsShader(shader))
    {
        int infoLogLength = 0;
        int maxLength = infoLogLength;
        
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        
        char* infoLog = (char*)malloc(maxLength);
        
        glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
        if(infoLogLength > 0)
        {
            printf("%s\n", infoLog);
        }

        free(infoLog);
    }
    else
    {
        printf("Name %d is not a shader\n", shader);
    }
}

internal inline b32 checkShaderCompilationStatus(GLuint shader)
{
    GLint shaderCompiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
    return (shaderCompiled == GL_TRUE);
}

internal inline b32 checkProgramLinkStatus(GLuint program)
{
    GLint programSuccess = GL_TRUE;
    glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
    return (programSuccess == GL_TRUE);
}

internal b32 initOpenGL()
{
    program = glCreateProgram();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    char vertexShaderFilePath[] = ".\\src\\basic.vert";
    TextFile vertexShaderFile = readAllText(vertexShaderFilePath);
    if (vertexShaderFile.status != FILE_STATUS_OK)
    {
        printf("Unable to load vertex shader file: %s\n", vertexShaderFilePath);
        return FALSE;
    }

    char *vertexShaderSource = vertexShaderFile.contents;
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    if (!checkShaderCompilationStatus(vertexShader))
    {
        printShaderLog(vertexShader);
        printf("Unable to compile vertex shader %d!\n", vertexShader);
        return FALSE;
    }

    glAttachShader(program, vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    char fragmentShaderFilePath[] = ".\\src\\basic.frag";
    TextFile fragmentShaderFile = readAllText(fragmentShaderFilePath);
    if (fragmentShaderFile.status != FILE_STATUS_OK)
    {
        printf("Unable to load fragment shader file: %s\n", fragmentShaderFilePath);
        return FALSE;
    }

    char *fragmentShaderSource = fragmentShaderFile.contents;
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    if (!checkShaderCompilationStatus(fragmentShader))
    {   
        printShaderLog(fragmentShader);
        printf("Unable to compile fragment shader %d!\n", fragmentShader);
        return FALSE;
    }

    // Since a fragment shader is allowed to write to multiple buffers, you need to explicitly specify which output is written to which buffer. 
    // This needs to happen before linking the program. However, since this is 0 by default and there's only one output right now, 
    // the following line of code is not really necessary
    glBindFragDataLocation(program, 0, "fragColor");

    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    if (!checkProgramLinkStatus(program))
    {
        printProgramLog(program);
        printf("Error linking program %d!\n", program);
        return FALSE;   
    }

    positionLocation = glGetAttribLocation(program, "position");
    if (positionLocation == -1)
    {
        printf("position is not a valid glsl program variable!\n");
        return FALSE;
    }

    colorLocation = glGetAttribLocation(program, "color");
    if (colorLocation == -1)
    {
        printf("color is not a valid glsl program variable!\n");
        return FALSE;
    }

    texCoordLocation = glGetAttribLocation(program, "texCoord");
    if (texCoordLocation == -1)
    {
        printf("texCoord is not a valid glsl program variable!\n");
        return FALSE;   
    }

    // VBO data
    GLfloat vertexData[] =
    {
        // position     // color           // texcoords
        -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
         0.5f, -0.5f,   1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
         0.5f,  0.5f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,  1.0f, 0.0f
    };

    // IBO data
    GLuint indexData[] = { 0, 1, 2, 0, 2, 3 };

    // Create VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 7 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    // Create IBO
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glClearDepth(1.0f);

    return TRUE;
}

int main(int argc, char *argv[]) 
{
    char windowTitle[256] = "War 1";

    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	// Request opengl 2.2 context.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window *window = SDL_CreateWindow(windowTitle,
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, 
        WINDOW_HEIGHT, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (!window)
    {
        printf("SDL window could not be created! SDL Error: %s\n", SDL_GetError());
        return -1;   
    }

	s32 win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return -1;
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
        return -1;
    }

    /* VSYNC: This makes our buffer swap syncronized with the monitor's vertical refresh */
    if (SDL_GL_SetSwapInterval(1) < 0)
    {
        printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }

    if (!initOpenGL())
    {
        printf("Unable to initialize OpenGL!\n");
        return -1;
    }

	b32 running = TRUE;
	b32 quit = FALSE;

	s32 fps = 0;
	s32 fpsCounter = 0;

    u32 startTime = SDL_GetTicks();

    loadTexture();

    while (running)
    {
    	SDL_SetWindowTitle(window, debugText(windowTitle, "War 1: %d fps", fps));

        SDL_Event evt;
        while (SDL_PollEvent(&evt))
        {
            if (evt.type == SDL_QUIT)
            {
                quit = TRUE;
                break;
            }
        }

        if (quit)
        {
            break;
        }

    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        glUseProgram(program);
        
        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

        glEnableVertexAttribArray(colorLocation);
        glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

        glEnableVertexAttribArray(texCoordLocation);
        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        glDisableVertexAttribArray(positionLocation);
        glUseProgram(NULL);

    	/* Swap our back buffer to the front */
    	SDL_GL_SwapWindow(window);

        u32 currentTime = SDL_GetTicks();
        f32 secondsElapsed = (currentTime - startTime) / 1000.0f;

        if (secondsElapsed >= 1)
        {
        	fps = fpsCounter;
        	fpsCounter = 0;
        	startTime = currentTime;
        }
        else
        {
        	fpsCounter++;
        }
    }

    /* Delete our opengl context, destroy our window, and shutdown SDL */
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}