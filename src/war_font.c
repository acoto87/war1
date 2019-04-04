

WarSprite loadFontSprite(WarContext* context)
{
    const char* fontPath = context->fontPath;

    WarSprite sprite = {0};

    s32 width, height, bitsPerPixel;
    u8* data = stbi_load(fontPath, &width, &height, &bitsPerPixel, 0);
    if (data)
    {
        sprite = createSprite(context, width, height, data);
        stbi_image_free(data);
    }

    return sprite;
}
