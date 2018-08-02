typedef struct
{
    u32 length;
    uchar *contents;
} TextFile;

inline u32 flength(FILE *file)
{
    u32 pos = ftell(file);
    fseek(file, 0, SEEK_END);
    u32 length = ftell(file);
    fseek(file, pos, SEEK_SET);
    return length;
}

inline void fileReadBytes(u8 *bytes, u32 length, FILE *file)
{
    u8 buffer[DEFAULT_BUFFER_SIZE];

    size32 total = 0;
    size32 sz = min(length, DEFAULT_BUFFER_SIZE);
    size32 read = fread(buffer, sizeof(u8), sz, file);
    while (read && total < length)
    {
        memcpy(bytes + total, buffer, read);
        total += read;

        sz = min(length - total, DEFAULT_BUFFER_SIZE);
        read = fread(buffer, sizeof(u8), sz, file);
    }
}

inline u8 fileReadU8(FILE *file)
{
    u8 value;
    fread(&value, sizeof(u8), 1, file);
    return value;
}

inline u16 fileReadU16(FILE *file)
{
    u16 value;
    fread(&value, sizeof(u16), 1, file);
    return value;
}

inline u32 fileReadU32(FILE *file)
{
    u32 value;
    fread(&value, sizeof(u32), 1, file);
    return value;
}

inline TextFile* fileReadAllText(char *filePath)
{
    FILE *file;

    // get the length of the file
    file = fopen(filePath, "rb");
    if (!file)
    {
        return null;
    }

    u32 fileLength = flength(file);
    fclose(file);

    file = fopen(filePath, "r");
    if (!file) 
    {
        return null;
    }

    TextFile *textFile = (TextFile*)xmalloc(sizeof(TextFile));
    textFile->length = fileLength;
    if (textFile->length > 0)
    {
        textFile->contents = (uchar*)xcalloc(textFile->length, 1);
        fileReadBytes(textFile->contents, textFile->length, file);
    }
    
    fclose(file);
    return textFile;
}

inline void fileDispose(TextFile *file)
{
    if (file->contents)
    {
        free(file->contents);
        file->contents = null;
    }

    free(file);
}
