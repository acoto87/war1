WarSprite createSprite(WarContext *context, s32 count, u32 width, u32 height, WarVertex vertices[], s32 verticesCount, GLuint indices[], s32 indicesCount)
{
    WarSprite sprite = (WarSprite){0};
    sprite.textureIndex = createTexture(context);
    sprite.count = count;
    sprite.width = width;
    sprite.height = height;

    // if this isn't generated glEnableVertexAttribArray set GL_INVALID_OPERATION error.
    glGenVertexArrays(1, &sprite.vao);
    glBindVertexArray(sprite.vao);
    
    glGenBuffers(1, &sprite.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, sprite.vbo);
    glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(WarVertex), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &sprite.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(GLuint), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(context->positionLocation);
    glVertexAttribPointer(context->positionLocation, 2, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, position));
    glDisableVertexAttribArray(context->positionLocation);

    glEnableVertexAttribArray(context->texCoordLocation);
    glVertexAttribPointer(context->texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(WarVertex), (void*)offsetof(WarVertex, texCoords));
    glDisableVertexAttribArray(context->texCoordLocation);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return sprite;
}

void renderSprite(WarContext *context, WarSprite *sprite, u8 *textureData, mat4 transform)
{
    glUniformMatrix4fv(context->modelLocation, 1, GL_FALSE, (f32*)transform);

    glBindTexture(GL_TEXTURE_2D, context->textures[sprite->textureIndex]);

    if (textureData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sprite->width, sprite->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    }

    glBindVertexArray(sprite->vao);
    glBindBuffer(GL_ARRAY_BUFFER, sprite->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite->ibo);

    glEnableVertexAttribArray(context->positionLocation);
    glEnableVertexAttribArray(context->texCoordLocation);

    glDrawElements(GL_TRIANGLES, sprite->count * 6, GL_UNSIGNED_INT, null);

    glDisableVertexAttribArray(context->texCoordLocation);
    glDisableVertexAttribArray(context->positionLocation);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}