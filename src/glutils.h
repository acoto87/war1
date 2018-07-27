GLenum glCheckError_(const char *file, int line)
{
    char *error;

    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        printf("%s in file %s (%d)\n", error, file, line);
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void printProgramLog(GLuint program)
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

void printShaderLog(GLuint shader)
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

inline bool checkShaderCompilationStatus(GLuint shader)
{
    GLint shaderCompiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
    return (shaderCompiled == GL_TRUE);
}

inline bool checkProgramLinkStatus(GLuint program)
{
    GLint programSuccess = GL_TRUE;
    glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
    return (programSuccess == GL_TRUE);
}

GLuint loadShaderFromFile(char *shaderFilePath, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader <= 0)
    {
        glCheckError();
        printf("Unable to create new shader of type: %d\n", shaderType);
        return 0;
    }

    TextFile *shaderFile = fileReadAllText(shaderFilePath);
    if (!shaderFile)
    {
        printf("Unable to load vertex shader file: %s\n", shaderFilePath);

        glDeleteShader(shader);
        return 0;
    }

    char *shaderSource = shaderFile->contents;
    
    glShaderSource(shader, 1, &shaderSource, null);
    glCheckError();

    glCompileShader(shader);
    glCheckError();

    if (!checkShaderCompilationStatus(shader))
    {
        printShaderLog(shader);
        printf("Unable to compile vertex shader %d!\n", shader);

        glDeleteShader(shader);
        return 0;
    }

    fileDispose(shaderFile);

    return shader;
}