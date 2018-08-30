#define NVG_WHITE nvgRGBA(255, 255, 255, 255)
#define NVG_BLACK nvgRGBA(0, 0, 0, 255)
#define NVG_GRAY_TRANSPARENT nvgRGBA(128, 128, 128, 128)
#define NVG_GREEN_SELECTION nvgRGBA(0, 199, 0, 255)

typedef struct
{
    s32 image;
    s32 cimages;
    s32 nvertices;
    s32 cvertices;
    NVGvertex *vertices;
} NVGimageBatch;

NVGimageBatch* nvgBeginImageBatch(NVGcontext *gfx, s32 image, s32 cimages)
{
    NVGimageBatch* batch = (NVGimageBatch*)xmalloc(sizeof(NVGimageBatch));
    batch->image = image;
    batch->cimages = cimages;
    batch->nvertices = 0;
    batch->cvertices = cimages * 6;
    batch->vertices = (NVGvertex *)xcalloc(batch->cvertices, sizeof(NVGvertex));
    return batch;
}

void nvgRenderBatchImage(NVGcontext* gfx, NVGimageBatch* batch, f32 sx, f32 sy, f32 sw, f32 sh, f32 dx, f32 dy, f32 dw, f32 dh)
{
    f32 x, y;
    s32 iw, ih;
    f32 tx, ty;
    NVGstate *state;
    NVGvertex *vertex;
    
    if (batch->nvertices >= batch->cvertices)
    {
        fprintf(stderr, "Can't print more images in this batch: (%d, %d)\n", batch->nvertices, batch->cvertices);
        return;
    }

    state = nvg__getState(gfx);
    
    nvgImageSize(gfx, batch->image, &iw, &ih);

    tx = sx / iw;
    ty = sy / ih;

    // first triangle
    vertex = &batch->vertices[batch->nvertices];
    nvgTransformPoint(&x, &y, state->xform, dx, dy);
    nvg__vset(vertex, x, y, tx, ty);
    batch->nvertices++;

    vertex = &batch->vertices[batch->nvertices];
    nvgTransformPoint(&x, &y, state->xform, dx, dy + dh);
    nvg__vset(vertex, x, y, tx, ty + sh / ih);
    batch->nvertices++;

    vertex = &batch->vertices[batch->nvertices];
    nvgTransformPoint(&x, &y, state->xform, dx + dw, dy);
    nvg__vset(vertex, x, y, tx + sw / iw, ty);
    batch->nvertices++;

    // second triangle
    vertex = &batch->vertices[batch->nvertices];
    nvgTransformPoint(&x, &y, state->xform, dx + dw, dy);
    nvg__vset(vertex, x, y, tx + sw / iw, ty);
    batch->nvertices++;

    vertex = &batch->vertices[batch->nvertices];
    nvgTransformPoint(&x, &y, state->xform, dx, dy + dh);
    nvg__vset(vertex, x, y, tx, ty + sh / ih);
    batch->nvertices++;

    vertex = &batch->vertices[batch->nvertices];
    nvgTransformPoint(&x, &y, state->xform, dx + dw, dy + dh);
    nvg__vset(vertex, x, y, tx + sw / iw, ty + sh / ih);
    batch->nvertices++;
}

void nvgEndImageBatch(NVGcontext *gfx, NVGimageBatch* batch)
{
    NVGstate* state = nvg__getState(gfx);
    NVGpaint paint = state->fill;

    paint.image = batch->image;
    paint.innerColor.a *= state->alpha;
    paint.outerColor.a *= state->alpha;

	gfx->params.renderTriangles(gfx->params.userPtr, &paint, state->compositeOperation, &state->scissor, batch->vertices, batch->nvertices);
	gfx->drawCallCount++;
	gfx->textTriCount += batch->nvertices/3;

    free(batch->vertices);
    free(batch);
}

void nvgRenderSubImage(NVGcontext *gfx, s32 image, f32 sx, f32 sy, f32 sw, f32 sh, f32 dx, f32 dy, f32 dw, f32 dh)
{
    NVGimageBatch* batch = nvgBeginImageBatch(gfx, image, 1);
    nvgRenderBatchImage(gfx, batch, sx, sy, sw, sh, dx, dy, dw, dh);
    nvgEndImageBatch(gfx, batch);
}

void nvgRenderImage(NVGcontext *gfx, s32 image, f32 x, f32 y, f32 w, f32 h)
{
    nvgRenderSubImage(gfx, image, 0, 0, w, h, x, y, w, h);
}

WarSprite createSprite(WarContext *context, s32 width, s32 height, u8 data[])
{
    WarSprite sprite = (WarSprite){0};
    sprite.width = width;
    sprite.height = height;
    sprite.image = nvgCreateImageRGBA(context->gfx, width, height, NVG_IMAGE_NEAREST, data);
    return sprite;
}

void updateSprite(WarContext *context, WarSprite *sprite, u8 data[])
{
    nvgUpdateImage(context->gfx, sprite->image, data);
}

void renderSubSprite(WarContext *context, WarSprite *sprite, f32 sx, f32 sy, f32 sw, f32 sh, f32 dx, f32 dy, f32 dw, f32 dh)
{
    nvgRenderSubImage(context->gfx, sprite->image, sx, sy, sw, sh, dx, dy, dw, dh);
}

void renderSprite(WarContext *context, WarSprite *sprite, f32 x, f32 y)
{
    nvgRenderSubImage(context->gfx, sprite->image, 0, 0, sprite->width, sprite->height, x, y, sprite->width, sprite->height);
}

/* Intent of implementation of a graphics API

#define MAX_RENDERING_CONTEXT_STATES 256
#define MAX_GRADIENT_STOPS_COUNT 256
#define MAX_PROGRAM_LOCATIONS 12
#define MAX_TEXTURES_COUNT 512

typedef struct
{
    union
    {
        f32 rgba[4];
        struct
        {
            f32 r, g, b, a;
        };
    };
} WarColor;

#define WAR_COLOR_RGBA(r, g, b, a) (WarColor){(r)/255.0f, (g)/255.0f, (b)/255.0f, (a)/255.0f}
#define WAR_COLOR_RGBA_F(r, g, b, a) (WarColor){a, g, b, a}

#define TRANSPARENT_BLACK WAR_COLOR_RGBA_F(0.0f, 0.0f, 0.0f, 0.0f)
#define SOLID_BLACK WAR_COLOR_RGBA_F(0.0f, 0.0f, 0.0f, 1.0f)

typedef enum
{
    COMPOSITION_SOURCE_OVER,
    COMPOSITION_SOURCE_ATOP,
    COMPOSITION_SOURCE_IN,
    COMPOSITION_SOURCE_OUT,
    COMPOSITION_DESTINATION_ATOP,
    COMPOSITION_DESTINATION_IN,
    COMPOSITION_DESTINATION_OUT,
    COMPOSITION_DESTINATION_OVER,
    COMPOSITION_LIGHTER,
    COMPOSITION_COPY,
    COMPOSITION_XOR
} WarCompositionOperation;

typedef struct
{
    WarColor color;
} WarStyle;

typedef struct
{
    s32 imageIndex;
    u32 width;
    u32 height;
    u8* data;
} WarImageData;

typedef WarImageData WarImageSource;

typedef struct
{
    // compositing
    f32 globalAlpha; // (default: 1.0)
    WarCompositionOperation globalCompositeOperation; // (default: COMPOSITION_SOURCE_OVER)

    // colors and styles
    WarStyle strokeStyle; // (default: "black")
    WarStyle fillStyle; // (default: "black")

    f32 lineWidth; // (default: 1.0f);

    mat3 transform; // (default: identity)
} WarRenderingContextState;

typedef enum
{
    WAR_PRG_LOC_POSITION,
    WAR_PRG_LOC_TEXCOORD,
    WAR_PRG_LOC_COLOR,
    WAR_PRG_LOC_WORLD,
    WAR_PRG_LOC_VIEWSIZE,
    WAR_PRG_LOC_TEX,
    WAR_PGR_LOC_USETEX
} WarProgramLocations;

typedef struct
{
    vec2 position;
    vec2 texCoords;
    vec4 color;
} WarVertex;

typedef struct
{
    WarRenderingContextState state;
    vec2 frameBufferSize;

    GLuint shaderProgram;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint locations[MAX_PROGRAM_LOCATIONS];

    s32 images[MAX_TEXTURES_COUNT];
    GLint textures[MAX_TEXTURES_COUNT];

    u32 _stateIndex;
    WarRenderingContextState _states[MAX_RENDERING_CONTEXT_STATES];
} WarRenderingContext2D;

internal bool initOpenGL1(WarRenderingContext2D *ctx)
{
    ctx->vertexShader = loadShaderFromFile(".\\src\\basic.vert", GL_VERTEX_SHADER);
    if (!ctx->vertexShader)
    {
        printf("Unable to load the vertex shader!\n");
        return false;
    }

    ctx->fragmentShader = loadShaderFromFile(".\\src\\basic.frag", GL_FRAGMENT_SHADER);
    if (!ctx->fragmentShader)
    {
        printf("Unable to load the fragment shader!\n");
        return false;
    }

    ctx->shaderProgram = glCreateProgram();

    // Since a fragment shader is allowed to write to multiple buffers, you need to explicitly specify which output is written to which buffer. 
    // This needs to happen before linking the program. However, since this is 0 by default and there's only one output right now, 
    // the following line of code is not really necessary
    glBindFragDataLocation(ctx->shaderProgram, 0, "fragColor");

    glAttachShader(ctx->shaderProgram, ctx->vertexShader);
    glAttachShader(ctx->shaderProgram, ctx->fragmentShader);
    glLinkProgram(ctx->shaderProgram);
    if (!checkProgramLinkStatus(ctx->shaderProgram))
    {
        printProgramLog(ctx->shaderProgram);
        printf("Error linking program %d!\n", ctx->shaderProgram);
        return false;   
    }

    ctx->locations[WAR_PRG_LOC_POSITION] = glGetAttribLocation(ctx->shaderProgram, "position");
    ctx->locations[WAR_PRG_LOC_TEXCOORD] = glGetAttribLocation(ctx->shaderProgram, "texCoord");
    ctx->locations[WAR_PRG_LOC_COLOR] = glGetAttribLocation(ctx->shaderProgram, "color");
    ctx->locations[WAR_PRG_LOC_WORLD] = glGetUniformLocation(ctx->shaderProgram, "world");
    ctx->locations[WAR_PRG_LOC_VIEWSIZE] = glGetUniformLocation(ctx->shaderProgram, "viewSize");
    ctx->locations[WAR_PRG_LOC_TEX] = glGetUniformLocation(ctx->shaderProgram, "tex");
    ctx->locations[WAR_PGR_LOC_USETEX] = glGetUniformLocation(ctx->shaderProgram, "useTex");

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);

    return true;
}

internal s32 createTexture1(WarRenderingContext2D *ctx, u32 imageIndex)
{
    s32 textureIndex = -1;
    for(s32 i = 0; i < MAX_TEXTURES_COUNT; i++)
    {
        if (ctx->textures[i] < 0)
        {
            textureIndex = i;
            break;
        }
    }
    
    if (textureIndex >= 0) 
    {
        GLuint tex;
        glGenTextures(1, &tex);

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        ctx->textures[textureIndex] = tex;
        ctx->images[imageIndex] = textureIndex;
    }

    return textureIndex;
}

WarRenderingContext2D* create(u32 width, u32 height)
{
    WarRenderingContext2D* ctx = (WarRenderingContext2D*)calloc(1, sizeof(WarRenderingContext2D));

    ctx->frameBufferSize[0] = (f32)width;
    ctx->frameBufferSize[1] = (f32)height;

    if (!initOpenGL1(ctx))
    {
        free(ctx);
        return NULL;
    }
    
    for(s32 i = 0; i < MAX_TEXTURES_COUNT; i++)
    {
        ctx->textures[i] = -1;
        ctx->images[i] = -1;
    }
    
    // compositing
    ctx->state.globalAlpha = 1.0f;
    ctx->state.globalCompositeOperation = COMPOSITION_SOURCE_OVER;

    // colors and styles
    ctx->state.strokeStyle.color = SOLID_BLACK;
    ctx->state.fillStyle.color = SOLID_BLACK;

    ctx->state.lineWidth = 1.0f;

    glm_mat3_identity(ctx->state.transform);

    return ctx;
}

void fillStyle(WarRenderingContext2D *ctx, WarColor color)
{
    ctx->state.fillStyle.color = color;
}

void strokeStyle(WarRenderingContext2D *ctx, WarColor color)
{
    ctx->state.strokeStyle.color = color;
}

void lineWidth(WarRenderingContext2D *ctx, f32 lineWidth)
{
    if (lineWidth > 0)
    {
        ctx->state.lineWidth = lineWidth;
    }
}

void globalAlpha(WarRenderingContext2D *ctx, f32 alpha)
{
    if (alpha >= 0.0f && alpha <= 1.0f)
    {
        ctx->state.globalAlpha = alpha;
    }
}

void globalCompositeOperation(WarRenderingContext2D *ctx, WarCompositionOperation operation)
{
    ctx->state.globalCompositeOperation = operation;
}

// states
void save(WarRenderingContext2D* ctx)
{
    if (ctx->_stateIndex >= MAX_RENDERING_CONTEXT_STATES)
    {
        fprintf(stderr, "Can't push the state to the stack. MAX_RENDERING_CONTEXT_STATES reached.");
        return;
    }

    memcpy(&ctx->_states[ctx->_stateIndex], &ctx->state, sizeof(WarRenderingContextState));
    ctx->_stateIndex++;
}

void restore(WarRenderingContext2D* ctx)
{
    if (ctx->_stateIndex > 0)
    {
        memcpy(&ctx->state, &ctx->_states[ctx->_stateIndex - 1], sizeof(WarRenderingContextState));
        ctx->_stateIndex--;
    }
}

// frame
void beginFrame(WarRenderingContext2D *ctx)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(ctx->shaderProgram);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    save(ctx);
}

void endFrame(WarRenderingContext2D *ctx)
{
    restore(ctx);

    glUseProgram(0);
}

// transforms
void transform(WarRenderingContext2D* ctx, f32 a, f32 b, f32 c, f32 d, f32 e, f32 f)
{
    mat3 m = {{a, b, 0},  // column 0
              {c, d, 0},  // column 1
              {e, f, 1}}; // column 2

    glm_mat3_mul(ctx->state.transform, m, ctx->state.transform);
}

void setTransform(WarRenderingContext2D* ctx, f32 a, f32 b, f32 c, f32 d, f32 e, f32 f)
{
    glm_mat3_identity(ctx->state.transform);
    transform(ctx, a, b, c, d, e, f);
}

void scale(WarRenderingContext2D* ctx, f32 x, f32 y)
{
    transform(ctx, x, 0, 0, y, 0, 0);
}

void rotate(WarRenderingContext2D* ctx, f32 angle)
{
    transform(ctx, cosf(angle), sinf(angle), -sinf(angle), cosf(angle), 0, 0);
}

void translate(WarRenderingContext2D* ctx, f32 x, f32 y)
{
    transform(ctx, 1, 0, 0, 1, x, y);
}

void fillRect(WarRenderingContext2D* ctx, s32 x, s32 y, u32 w, u32 h)
{
    f32 fx = (f32)x;
    f32 fy = (f32)y;
    f32 fw = (f32)w;
    f32 fh = (f32)h;

    save(ctx);
    translate(ctx, fx, fy);

    WarColor c = ctx->state.fillStyle.color;
    WarVertex vertices[] = 
    {
        {{ 0,  0}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},
        {{fw,  0}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},
        {{fw, fh}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},
        {{ 0, fh}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}}
    };

    // create IBO
    GLuint indices[] = { 0, 1, 2, 0, 2, 3 };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(WarVertex), vertices, GL_STATIC_DRAW);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    GLint positionLoc = ctx->locations[WAR_PRG_LOC_POSITION];
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, position));

    GLint texCoordLoc = ctx->locations[WAR_PRG_LOC_TEXCOORD];
    glEnableVertexAttribArray(texCoordLoc);
    glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, texCoords));

    GLint colorLoc = ctx->locations[WAR_PRG_LOC_COLOR];
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, color));

    GLint worldLoc = ctx->locations[WAR_PRG_LOC_WORLD];
    glUniformMatrix3fv(worldLoc, 1, GL_FALSE, (f32*)ctx->state.transform);

    GLint viewSizeLoc = ctx->locations[WAR_PRG_LOC_VIEWSIZE];
    glUniform2fv(viewSizeLoc, 1, ctx->frameBufferSize);

    GLint useTexLoc = ctx->locations[WAR_PGR_LOC_USETEX];
    glUniform1i(useTexLoc, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glDrawElements(GL_TRIANGLES, 2 * 6, GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(colorLoc);
    glDisableVertexAttribArray(texCoordLoc);
    glDisableVertexAttribArray(positionLoc);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    restore(ctx);
}

void clearRect(WarRenderingContext2D* ctx, s32 x, s32 y, u32 w, u32 h)
{
    fillStyle(ctx, WAR_COLOR_RGBA(0, 0, 0, 0));
    fillRect(ctx, x, y, w, h);
}

void strokeRect(WarRenderingContext2D* ctx, s32 x, s32 y, u32 w, u32 h)
{
    f32 fx = (f32)x;
    f32 fy = (f32)y;
    f32 fw = (f32)w;
    f32 fh = (f32)h;

    save(ctx);
    translate(ctx, fx, fy);

    WarColor c = ctx->state.strokeStyle.color;
    f32 lw = ctx->state.lineWidth;
    
    WarVertex vertices[] = 
    {
        // outside points
        {{-lw,  -lw}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},
        {{fw+lw,  -lw}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},
        {{fw+lw, fh+lw}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},
        {{-lw, fh+lw}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},

        // inside points
        {{lw,  lw}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},
        {{fw-lw,  lw}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},
        {{fw-lw, fh-lw}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}},
        {{lw, fh-lw}, {0.0f, 0.0f}, {c.r, c.g, c.b, c.a}}
    };

    // create IBO
    GLuint indices[] = { 0, 5, 1, 0, 4, 5, 1, 5, 6, 1, 6, 2, 2, 6, 7, 2, 7, 3, 3, 7, 4, 3, 4, 0 };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(WarVertex), vertices, GL_STATIC_DRAW);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    GLint positionLoc = ctx->locations[WAR_PRG_LOC_POSITION];
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, position));

    GLint texCoordLoc = ctx->locations[WAR_PRG_LOC_TEXCOORD];
    glEnableVertexAttribArray(texCoordLoc);
    glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, texCoords));

    GLint colorLoc = ctx->locations[WAR_PRG_LOC_COLOR];
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, color));

    GLint worldLoc = ctx->locations[WAR_PRG_LOC_WORLD];
    glUniformMatrix3fv(worldLoc, 1, GL_FALSE, (f32*)ctx->state.transform);

    GLint viewSizeLoc = ctx->locations[WAR_PRG_LOC_VIEWSIZE];
    glUniform2fv(viewSizeLoc, 1, ctx->frameBufferSize);

    GLint useTexLoc = ctx->locations[WAR_PGR_LOC_USETEX];
    glUniform1i(useTexLoc, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glDrawElements(GL_TRIANGLES, 8 * 6, GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(colorLoc);
    glDisableVertexAttribArray(texCoordLoc);
    glDisableVertexAttribArray(positionLoc);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    restore(ctx);
}

void drawSubImage(WarRenderingContext2D* ctx, WarImageSource image, s32 sx, s32 sy, u32 sw, u32 sh, s32 dx, s32 dy, u32 dw, u32 dh)
{
    f32 fsx = (f32)sx;
    f32 fsy = (f32)sy;
    f32 fsw = (f32)sw;
    f32 fsh = (f32)sh;

    f32 fdx = (f32)dx;
    f32 fdy = (f32)dy;
    f32 fdw = (f32)dw;
    f32 fdh = (f32)dh;

    f32 imgw = (f32)image.width;
    f32 imgh = (f32)image.height;

    save(ctx);
    translate(ctx, fdx, fdy);

    WarColor c = ctx->state.fillStyle.color;
    WarVertex vertices[] = 
    {
        {{  0,   0}, {      fsx/imgw,       fsy/imgh}, {c.r, c.g, c.b, c.a}},
        {{fdw,   0}, {(fsx+fsw)/imgw,       fsy/imgh}, {c.r, c.g, c.b, c.a}},
        {{fdw, fdh}, {(fsx+fsw)/imgw, (fsy+fsh)/imgh}, {c.r, c.g, c.b, c.a}},
        {{  0, fdh}, {      fsx/imgw, (fsy+fsh)/imgh}, {c.r, c.g, c.b, c.a}}
    };

    // create IBO
    GLuint indices[] = { 0, 1, 2, 0, 2, 3 };

    s32 textureIndex = -1;
    if (image.imageIndex >= 0)
    {
        textureIndex = ctx->images[image.imageIndex];
    }

    if (textureIndex < 0)
    {
        textureIndex = createTexture1(ctx, image.imageIndex);
        if (textureIndex >= 0)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
        }
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(WarVertex), vertices, GL_STATIC_DRAW);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    GLint positionLoc = ctx->locations[WAR_PRG_LOC_POSITION];
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, position));

    GLint texCoordLoc = ctx->locations[WAR_PRG_LOC_TEXCOORD];
    glEnableVertexAttribArray(texCoordLoc);
    glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, texCoords));

    GLint colorLoc = ctx->locations[WAR_PRG_LOC_COLOR];
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, color));

    GLint worldLoc = ctx->locations[WAR_PRG_LOC_WORLD];
    glUniformMatrix3fv(worldLoc, 1, GL_FALSE, (f32*)ctx->state.transform);

    GLint viewSizeLoc = ctx->locations[WAR_PRG_LOC_VIEWSIZE];
    glUniform2fv(viewSizeLoc, 1, ctx->frameBufferSize);

    GLint useTexLoc = ctx->locations[WAR_PGR_LOC_USETEX];
    glUniform1i(useTexLoc, 1);

    glBindTexture(GL_TEXTURE_2D, ctx->textures[textureIndex]);

    glDrawElements(GL_TRIANGLES, 2 * 6, GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(colorLoc);
    glDisableVertexAttribArray(texCoordLoc);
    glDisableVertexAttribArray(positionLoc);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    restore(ctx);
}

void drawImageRect(WarRenderingContext2D* ctx, WarImageSource image, s32 dx, s32 dy, u32 dw, u32 dh)
{
    drawSubImage(ctx, image, 0, 0, image.width, image.height, dx, dy, dw, dh);
}

void drawImage(WarRenderingContext2D* ctx, WarImageSource image, s32 dx, s32 dy)
{
    drawSubImage(ctx, image, 0, 0, image.width, image.height, dx, dy, image.width, image.height);
}

WarImageData createImageData(WarRenderingContext2D* ctx, u32 sw, u32 sh)
{
    s32 imageIndex = -1;
    for(s32 i = 0; i < MAX_TEXTURES_COUNT; i++)
    {
        if (ctx->images[i] < 0)
        {
            imageIndex = i;
            break;
        }
    }
    
    WarImageData imageData = (WarImageData){0};
    imageData.imageIndex = imageIndex;

    if (imageIndex >= 0)
    {
        imageData.width = sw;
        imageData.height = sh;
        imageData.data = calloc(imageData.width * imageData.height * 4, sizeof(u8));
    }

    return imageData;
}

*/