s32 createEntity(WarContext *context, WarEntityType type)
{
    WarScene *scene = context->currentScene;
    for (s32 i = 0; i < MAX_ENTITIES_COUNT; ++i)
    {
        if (scene->entities[i].id == 0)
        {
            context->staticEntityId++;
            scene->entities[i].id = context->staticEntityId;
            scene->entities[i].enabled = true;
            scene->entities[i].type = type;

            // transform component
            scene->entities[i].transform = (WarTransformComponent){0};
            scene->entities[i].transform.enabled = true;

            // clear all other components
            scene->entities[i].sprite = (WarSpriteComponent){0};
            scene->entities[i].anim = (WarAnimationComponent){0};
            return i;
        }
    }

    printf("Can't create new entities!!");
    return -1;
}

void destroyEntityByIndex(WarContext *context, s32 index)
{
    WarScene *scene = context->currentScene;
    scene->entities[index].id = 0;
}

void destroyEntityById(WarContext *context, WarEntityId id)
{
    WarScene *scene = context->currentScene;
    for (s32 i = 0; i < MAX_ENTITIES_COUNT; ++i)
    {
        if (scene->entities[i].id == id)
        {
            destroyEntityById(context, i);
            break;
        }
    }
}

void addTransformComponent(WarContext *context, s32 entityIndex, vec2 position)
{
    WarScene *scene = context->currentScene;
    scene->entities[entityIndex].transform = (WarTransformComponent){0};
    scene->entities[entityIndex].transform.enabled = true;
    scene->entities[entityIndex].transform.position[0] = position[0];
    scene->entities[entityIndex].transform.position[1] = position[1];
}

void addSpriteComponent(WarContext *context, s32 entityIndex, s32 resourceIndex)
{
    WarScene *scene = context->currentScene;
    WarResource *resource = getOrCreateResource(context, resourceIndex);
    if (resource->type != WAR_RESOURCE_TYPE_IMAGE && 
        resource->type != WAR_RESOURCE_TYPE_SPRITE)
    {
        return;
    }

    u32 w, h;

    switch(resource->type)
    {
        case WAR_RESOURCE_TYPE_IMAGE:
        {
            w = resource->imageData.width;
            h = resource->imageData.height;
            break;
        }

        case WAR_RESOURCE_TYPE_SPRITE:
        {
            w = resource->spriteData.frameWidth;
            h = resource->spriteData.frameHeight;
            break;
        }
    }

    // create texture for the image
    s32 textureIndex = createTexture(context);
    
    // create VBO
    GLfloat vertexData[] =
    {
        // position  // texcoords
        0,      0,   0.0f, 1.0f,
        w*2,    0,   1.0f, 1.0f,
        w*2,  h*2,   1.0f, 0.0f,
        0,    h*2,   0.0f, 0.0f
    };

    // create IBO
    GLuint indexData[] = { 0, 1, 2, 0, 2, 3 };

    // create VAO, VBO and IBO
    GLuint vao, vbo, ibo;

    // if this isn't generated glEnableVertexAttribArray set GL_INVALID_OPERATION error.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 7 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(context->positionLocation);
    glVertexAttribPointer(context->positionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glDisableVertexAttribArray(context->positionLocation);

    glEnableVertexAttribArray(context->texCoordLocation);
    glVertexAttribPointer(context->texCoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glDisableVertexAttribArray(context->texCoordLocation);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // sprite component
    scene->entities[entityIndex].sprite = (WarSpriteComponent){0};
    scene->entities[entityIndex].sprite.enabled = true;
    scene->entities[entityIndex].sprite.resourceIndex = resourceIndex;
    scene->entities[entityIndex].sprite.textureIndex = textureIndex;
    scene->entities[entityIndex].sprite.frameIndex = 0;
    scene->entities[entityIndex].sprite.vao = vao;
    scene->entities[entityIndex].sprite.vbo = vbo;
    scene->entities[entityIndex].sprite.ibo = ibo;
}

void addAnimationComponent(WarContext *context, s32 entityIndex, f32 duration, bool loop)
{
    WarScene *scene = context->currentScene;
    scene->entities[entityIndex].anim = (WarAnimationComponent){0};
    scene->entities[entityIndex].anim.enabled = true;
    scene->entities[entityIndex].anim.loop = loop;
    scene->entities[entityIndex].anim.duration = duration;
    scene->entities[entityIndex].anim.offset = 0;
}
