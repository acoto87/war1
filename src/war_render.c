#define NVG_WHITE nvgRGBA(255, 255, 255, 255)
#define NVG_BLACK nvgRGBA(0, 0, 0, 255)
#define NVG_FOG nvgRGBA(0, 0, 0, 128)
#define NVG_GRAY_TRANSPARENT nvgRGBA(128, 128, 128, 128)
#define NVG_RED_TRANSPARENT nvgRGBA(128, 0, 0, 128)
#define NVG_GREEN_SELECTION nvgRGBA(0, 199, 0, 255)
#define NVG_RED_SELECTION nvgRGBA(199, 0, 0, 255)
#define NVG_WHITE_SELECTION nvgRGBA(199, 199, 199, 255)
#define NVG_BLUE_INVULNERABLE nvgRGBA(0, 0, 199, 255)

typedef struct
{
    s32 image;              // the id of the source image
    s32 cimages;            // how many images are going to be rendered
    s32 iw;                 // width of the source image in pixels
    s32 ih;                 // height of the source image in pixels
    s32 nvertices;          // actual count of vertices
    s32 cvertices;          // total count of vertices
    NVGvertex* vertices;    // the vertices
} NVGimageBatch;

NVGimageBatch* nvgBeginImageBatch(NVGcontext* gfx, s32 image, s32 cimages)
{
    NVGimageBatch* batch = (NVGimageBatch*)xmalloc(sizeof(NVGimageBatch));
    batch->image = image;
    batch->cimages = cimages;
    batch->nvertices = 0;
    batch->cvertices = cimages * 6;
    batch->vertices = (NVGvertex *)xcalloc(batch->cvertices, sizeof(NVGvertex));

    nvgImageSize(gfx, batch->image, &batch->iw, &batch->ih);

    return batch;
}

void nvgRenderBatchImage(NVGcontext* gfx, NVGimageBatch* batch, rect rs, rect rd, vec2 scale)
{
    f32 x, y;
    NVGvertex* vertex;
    
    if (batch->nvertices + 6 > batch->cvertices)
    {
        logError("Can't print more images in this batch: (nvertices: %d, cvertices: %d)\n", batch->nvertices, batch->cvertices);
        return;
    }

    NVGstate* state = nvg__getState(gfx);

    // if the scale is (1.0, 1.0) then these transformations aren't necessary
    if (!vec2IsOne(scale))
    {
        nvgTranslate(gfx, halff(rd.width), halff(rd.height));
        nvgScale(gfx, scale.x, scale.y);
        nvgTranslate(gfx, -halff(rd.width), -halff(rd.height));
    }

    // positions of the four vertices
    f32 px = rd.x;
    f32 py = rd.y;
    f32 pw = rd.width;
    f32 ph = rd.height;

    // tex-coords of the four vertices
    f32 tx = rs.x / batch->iw;
    f32 ty = rs.y / batch->ih;
    f32 tw = rs.width / batch->iw;
    f32 th = rs.height / batch->ih;

    f32 pxs[] = { px, px,      px + pw, px + pw, px,      px + pw };
    f32 pys[] = { py, py + ph, py,      py,      py + ph, py + ph };

    f32 cxs[] = { tx, tx,      tx + tw, tx + tw, tx,      tx + tw };
    f32 cys[] = { ty, ty + th, ty,      ty,      ty + th, ty + th };

    for (s32 i = 0; i < 6; i++)
    {
        vertex = &batch->vertices[batch->nvertices];
        nvgTransformPoint(&x, &y, state->xform, pxs[i], pys[i]);
        nvg__vset(vertex, x, y, cxs[i], cys[i]);
        batch->nvertices++;    
    }
}

void nvgEndImageBatch(NVGcontext* gfx, NVGimageBatch* batch)
{
    NVGstate* state = nvg__getState(gfx);
    NVGpaint paint = state->fill;

    paint.image = batch->image;
    paint.innerColor.a *= state->alpha;
    paint.outerColor.a *= state->alpha;

	gfx->params.renderTriangles(gfx->params.userPtr, 
                                &paint, 
                                state->compositeOperation,
                                &state->scissor, 
                                batch->vertices, 
                                batch->nvertices);
	gfx->drawCallCount++;
	gfx->textTriCount += batch->nvertices/3;

    free(batch->vertices);
    free(batch);
}

typedef struct
{
    s32 image;				// the id of the source image
    s32 cimages;			// how many images are going to be rendered
	s32 iw;					// width of the source image in pixels
	s32 ih;					// height of the source image in pixels
	s32 gw;					// width of the grid in cells
	s32 gh;					// height of the grid in cells
    s32 nvertices;			// actual count of vertices
    s32 cvertices;			// total count of vertices
    NVGvertex* vertices;	// the vertices
} NVGimageGridBatch;

NVGimageGridBatch* nvgBeginImageGridBatch(NVGcontext* gfx, s32 image, s32 gw, s32 gh)
{
    NVGimageGridBatch* batch = (NVGimageGridBatch*)xmalloc(sizeof(NVGimageGridBatch));
    batch->image = image;
    batch->cimages = gw * gh;
    batch->gw = gw;
    batch->gh = gh;
    batch->nvertices = 0;
    batch->cvertices = batch->cimages * 6;
    batch->vertices = (NVGvertex *)xcalloc(batch->cvertices, sizeof(NVGvertex));

    nvgImageSize(gfx, batch->image, &batch->iw, &batch->ih);

    return batch;
}

void nvgRenderGridBatchImage(NVGcontext* gfx, NVGimageGridBatch* batch, rect rs, rect rd, vec2 scale, s32 gx, s32 gy)
{
    f32 x, y;
    NVGvertex *vertex;
    
    if (batch->nvertices + 6 > batch->cvertices)
    {
        fprintf(stderr, "Can't print more images in this batch: (%d, %d)\n", batch->nvertices, batch->cvertices);
        return;
    }

    NVGstate* state = nvg__getState(gfx);

    f32 tx = rs.x / batch->iw;
    f32 ty = rs.y / batch->ih;

    if (!vec2IsOne(scale))
    {
        nvgTranslate(gfx, halff(rd.width), halff(rd.height));
        nvgScale(gfx, scale.x, scale.y);
        nvgTranslate(gfx, -halff(rd.width), -halff(rd.height));
    }

    // first triangle
	{
		// first vertex
		{
			vertex = &batch->vertices[batch->nvertices];

			if (gx == 0 && gy == 0)
			{
				nvgTransformPoint(&x, &y, state->xform, rd.x, rd.y);
			}
            else if (gx == 0)
            {
                s32 index = ((gy - 1) * batch->gw + gx) * 6 + 1;
                x = batch->vertices[index].x;
                y = batch->vertices[index].y;
            }
			else
			{
                s32 index = (gy * batch->gw + (gx - 1)) * 6 + 2;
                x = batch->vertices[index].x;
                y = batch->vertices[index].y;
			}

            nvg__vset(vertex, x, y, tx, ty);

			batch->nvertices++;
		}

		// second vertex
		{
			vertex = &batch->vertices[batch->nvertices];

			if (gx == 0)
			{
                nvgTransformPoint(&x, &y, state->xform, rd.x, rd.y + rd.height);
			}
			else
			{
                s32 index = (gy * batch->gw + (gx - 1)) * 6 + 5;
                x = batch->vertices[index].x;
                y = batch->vertices[index].y;
			}

            nvg__vset(vertex, x, y, tx, ty + rs.height / batch->ih);

			batch->nvertices++;
        }

		// third vertex
		{
			vertex = &batch->vertices[batch->nvertices];

			if (gy == 0)
			{
				nvgTransformPoint(&x, &y, state->xform, rd.x + rd.width, rd.y);
			}
			else
			{
                s32 index = ((gy - 1) * batch->gw + gx) * 6 + 5;
                x = batch->vertices[index].x;
                y = batch->vertices[index].y;
			}

            nvg__vset(vertex, x, y, tx + rs.width / batch->iw, ty);

			batch->nvertices++;
		}
	}
    
    // second triangle
    {
		// first vertex
		{
			vertex = &batch->vertices[batch->nvertices];

            s32 index = batch->nvertices - 1;
            x = batch->vertices[index].x;
            y = batch->vertices[index].y;

            nvg__vset(vertex, x, y, tx + rs.width / batch->iw, ty);

			batch->nvertices++;
		}

		// second vertex
		{
			vertex = &batch->vertices[batch->nvertices];

            s32 index = batch->nvertices - 3;
            x = batch->vertices[index].x;
            y = batch->vertices[index].y;

            nvg__vset(vertex, x, y, tx, ty + rs.height / batch->ih);

			batch->nvertices++;
		}

		// third vertex
		{
			vertex = &batch->vertices[batch->nvertices];
			nvgTransformPoint(&x, &y, state->xform, rd.x + rd.width, rd.y + rd.height);
			nvg__vset(vertex, x, y, tx + rs.width / batch->iw, ty + rs.height / batch->ih);
			batch->nvertices++;
		}
	}
}

void nvgEndImageGridBatch(NVGcontext* gfx, NVGimageGridBatch* batch)
{
    NVGstate* state = nvg__getState(gfx);
    NVGpaint paint = state->fill;

    paint.image = batch->image;
    paint.innerColor.a *= state->alpha;
    paint.outerColor.a *= state->alpha;

	gfx->params.renderTriangles(gfx->params.userPtr, 
                                &paint, 
                                state->compositeOperation, 
                                &state->scissor, 
                                batch->vertices, 
                                batch->nvertices);
	gfx->drawCallCount++;
	gfx->textTriCount += batch->nvertices/3;

    free(batch->vertices);
    free(batch);
}

void nvgRenderSubImage(NVGcontext* gfx, s32 image, rect rs, rect rd, vec2 scale)
{
    NVGimageBatch* batch = nvgBeginImageBatch(gfx, image, 1);
    nvgRenderBatchImage(gfx, batch, rs, rd, scale);
    nvgEndImageBatch(gfx, batch);
}

void nvgRenderImage(NVGcontext* gfx, s32 image, rect rd, vec2 scale)
{
    rect rs = rectf(0, 0, rd.width, rd.height);
    nvgRenderSubImage(gfx, image, rs, rd, scale);
}

void nvgFillRect(NVGcontext* gfx, rect r, NVGcolor color)
{
    nvgSave(gfx);
    nvgBeginPath(gfx);
    nvgRect(gfx, r.x, r.y, r.width, r.height);
    nvgFillColor(gfx, color);
    nvgFill(gfx);
    nvgRestore(gfx);
}

void nvgFillRects(NVGcontext* gfx, s32 count, rect r[], NVGcolor color)
{
    nvgSave(gfx);
    nvgBeginPath(gfx);

    for (s32 i = 0; i < count; i++)
        nvgRect(gfx, r[i].x, r[i].y, r[i].width, r[i].height);

    nvgFillColor(gfx, color);
    nvgFill(gfx);
    nvgRestore(gfx);
}

void nvgStrokeRect(NVGcontext* gfx, rect r, NVGcolor color, f32 width)
{
    nvgSave(gfx);
    nvgBeginPath(gfx);
    nvgRect(gfx, r.x, r.y, r.width, r.height);
    nvgStrokeColor(gfx, color);
    nvgStrokeWidth(gfx, width);
    nvgStroke(gfx);
    nvgRestore(gfx);
}

void nvgStrokeRects(NVGcontext* gfx, s32 count, rect r[], NVGcolor color, f32 width)
{
    nvgSave(gfx);
    nvgBeginPath(gfx);

    for (s32 i = 0; i < count; i++)
        nvgRect(gfx, r[i].x, r[i].y, r[i].width, r[i].height);
    
    nvgStrokeColor(gfx, color);
    nvgStrokeWidth(gfx, width);
    nvgStroke(gfx);
    nvgRestore(gfx);
}

void nvgStrokeLine(NVGcontext* gfx, vec2 p1, vec2 p2, NVGcolor color, f32 width)
{
    nvgSave(gfx);
    nvgBeginPath(gfx);
    nvgMoveTo(gfx, p1.x, p1.y);
    nvgLineTo(gfx, p2.x, p2.y);
    nvgStrokeColor(gfx, color);
    nvgStrokeWidth(gfx, width);
    nvgStroke(gfx);
    nvgRestore(gfx);
}

void nvgStrokePolyline(NVGcontext* gfx, s32 count, vec2 points[], NVGcolor color, f32 width)
{
    if (count <= 1)
        return;

    nvgSave(gfx);
    nvgBeginPath(gfx);

    nvgMoveTo(gfx, points[0].x, points[0].y);
    for(s32 i = 1; i < count; i++)
        nvgLineTo(gfx, points[i].x, points[i].y);
    
    nvgStrokeColor(gfx, color);
    nvgStrokeWidth(gfx, width);
    nvgStroke(gfx);
    nvgRestore(gfx);
}

typedef struct 
{
    char* fontFace;
    s32 textAlign;
    enum NVGalign horizontalAlign;
    enum NVGalign verticalAlign;
    f32 width;
    f32 height;
    f32 blur;

    s32 fontIndex;
    f32 fontSize;
    NVGcolor fontColor;
    WarSprite fontSprite;
    s32 highlightIndex;
    WarFontData fontData;
} NVGfontParams;

void nvgSingleText(NVGcontext* gfx, const char* text, f32 x, f32 y, NVGfontParams params)
{
    if (params.fontSize <= 0)
        params.fontSize = 8.0f;

    if (!params.fontFace)
        params.fontFace = "defaultFont";

    if (params.textAlign <= 0)
        params.textAlign = NVG_ALIGN_LEFT | NVG_ALIGN_TOP;

    if (params.blur < 0)
        params.blur = 0;

	nvgSave(gfx);
    nvgFontSize(gfx, params.fontSize);
    nvgFontFace(gfx, params.fontFace);
    nvgFillColor(gfx, params.fontColor);
    nvgTextAlign(gfx, params.textAlign);
    nvgFontBlur(gfx, params.blur);
    nvgText(gfx, x, y, text, NULL);
    nvgRestore(gfx);
}

void nvgMultilineText(NVGcontext* gfx, const char* text, f32 x, f32 y, f32 width, f32 height, NVGfontParams params)
{
    if (params.fontSize <= 0)
        params.fontSize = 8.0f;

    if (!params.fontFace)
        params.fontFace = "defaultFont";

    if (params.textAlign <= 0)
        params.textAlign = NVG_ALIGN_LEFT | NVG_ALIGN_TOP;

    if (params.blur < 0)
        params.blur = 0;

	nvgSave(gfx);
    nvgFontSize(gfx, params.fontSize);
    nvgFontFace(gfx, params.fontFace);
    nvgFillColor(gfx, params.fontColor);
    nvgTextAlign(gfx, params.textAlign);
    nvgFontBlur(gfx, params.blur);

    float lineh;
    nvgTextMetrics(gfx, NULL, NULL, &lineh);

    NVGtextRow rows[10];
    const char* start = text;
    const char* end = text + strlen(text);
    s32 nrows;

    // The text break API can be used to fill a large buffer of rows,
	// or to iterate over the text just few lines (or just one) at a time.
	// The "next" variable of the last returned item tells where to continue.
    while ((nrows = nvgTextBreakLines(gfx, start, end, width, rows, 10))) {
        for (s32 i = 0; i < nrows; i++) {
            NVGtextRow* row = &rows[i];

            nvgText(gfx, x, y, row->start, row->end);
            y += lineh;
        }

        start = rows[nrows - 1].next;
    }

    nvgRestore(gfx);
}

NVGcolor getColorFromList(s32 index)
{
    const u32 colors[] = 
    {
        0xE52B50, // Amaranth
        0xFFBF00, // Amber
        0x9966CC, // Amethyst
        0xFBCEB1, // Apricot
        0x7FFFD4, // Aquamarine
        0x007FFF, // Azure
        0x89CFF0, // Baby blue
        0xF5F5DC, // Beige
        0x000000, // Black
        0x0000FF, // Blue
        0x0095B6, // Blue-green
        0x8A2BE2, // Blue-violet
        0xDE5D83, // Blush
        0xCD7F32, // Bronze
        0x964B00, // Brown
        0x800020, // Burgundy
        0x702963, // Byzantium
        0x960018, // Carmine
        0xDE3163, // Cerise
        0x007BA7, // Cerulean
        0xF7E7CE, // Champagne
        0x7FFF00, // Chartreuse green
        0x7B3F00, // Chocolate
        0x0047AB, // Cobalt blue
        0x6F4E37, // Coffee
        0xB87333, // Copper
        0xF88379, // Coral
        0xDC143C, // Crimson
        0x00FFFF, // Cyan
        0xEDC9AF, // Desert sand
        0x7DF9FF, // Electric blue
        0x50C878, // Emerald
        0x00FF3F, // Erin
        0xFFD700, // Gold
        0x808080, // Gray
        0x00FF00, // Green
        0x3FFF00, // Harlequin
        0x4B0082, // Indigo
        0xFFFFF0, // Ivory
        0x00A86B, // Jade
        0x29AB87, // Jungle green
        0xB57EDC, // Lavender
        0xFFF700, // Lemon
        0xC8A2C8, // Lilac
        0xBFFF00, // Lime
        0xFF00FF, // Magenta
        0xFF00AF, // Magenta rose
        0x800000, // Maroon
        0xE0B0FF, // Mauve
        0x000080, // Navy blue
        0xCC7722, // Ocher
        0x808000, // Olive
        0xFFA500, // Orange
        0xFF4500, // Orange-red
        0xDA70D6, // Orchid
        0xFFE5B4, // Peach
        0xD1E231, // Pear
        0xCCCCFF, // Periwinkle
        0x1C39BB, // Persian blue
        0xFFC0CB, // Pink
        0x8E4585, // Plum
        0x003153, // Prussian blue
        0xCC8899, // Puce
        0x800080, // Purple
        0xE30B5C, // Raspberry
        0xFF0000, // Red
        0xC71585, // Red-violet
        0xFF007F, // Rose
        0xE0115F, // Ruby
        0xFA8072, // Salmon
        0x92000A, // Sangria
        0x0F52BA, // Sapphire
        0xFF2400, // Scarlet
        0xC0C0C0, // Silver
        0x708090, // Slate gray
        0xA7FC00, // Spring bud
        0x00FF7F, // Spring green
        0xD2B48C, // Tan
        0x483C32, // Taupe
        0x008080, // Teal
        0x40E0D0, // Turquoise
        0xEE82EE, // Violet
        0x40826D, // Viridian
        0xFFFFFF, // White
        0xFFFF00, // Yellow
    };

    u32 rgb = colors[index % sizeof(colors)];

    NVGcolor color;
	color.r = ((rgb >> 16) & 0xFF) / 255.0f;
	color.g = ((rgb >>  8) & 0xFF) / 255.0f;
	color.b = ((rgb >>  0) & 0xFF) / 255.0f;
	color.a = 1.0f;
	return color;
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