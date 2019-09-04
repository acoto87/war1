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

enum NVGwrap {
	NVG_WRAP_NONE = 1<<0,
	NVG_WRAP_WORD = 1<<1,
};

typedef struct
{
    s32 fontIndex;
    f32 fontSize;
    f32 lineHeight;
    NVGcolor fontColor;
    NVGcolor highlightColor;
    s32 highlightIndex;
    s32 highlightCount;
    vec2 boundings;
    enum NVGalign horizontalAlign;
    enum NVGalign verticalAlign;
    enum NVGalign lineAlign;
    enum NVGwrap wrapping;

    WarSprite fontSprite;
    WarFontData fontData;
} NVGfontParams;

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