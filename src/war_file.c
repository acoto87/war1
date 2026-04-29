#include "war_file.h"

#include "war_log.h"

WarFile* wfile_loadWarFile(WarContext* context, StringView filePath)
{
    SDL_IOStream *stream = SDL_IOFromFile(wsv_data(filePath), "rb");
    if (!stream)
    {
        logError("Couldn't process the DATA.WAR file. The file doesn't exists at %s.", wsv_data(filePath));
        return NULL;
    }

    Sint64 fileLength = SDL_GetIOSize(stream);

    WarFile *warFile = (WarFile*)war_malloc(sizeof(WarFile));
    SDL_ReadU32LE(stream, &warFile->archiveID);
    SDL_ReadU32LE(stream, &warFile->numberOfEntries);

    switch (warFile->archiveID)
    {
        case 0x18:
        case 0x1A:
        {
            warFile->type = WAR_FILE_TYPE_RETAIL;
            break;
        }

        case 0x19:
        {
            warFile->type = WAR_FILE_TYPE_DEMO;
            break;
        }

        default:
        {
            warFile->type = WAR_FILE_TYPE_UNKNOWN;
            break;
        }
    }

    if (warFile->type == WAR_FILE_TYPE_UNKNOWN)
    {
        logError("Couldn't process the DATA.WAR file. The file type %u is not the RETAIL or DEMO version of the game.", warFile->archiveID);
        war_free(warFile);
        SDL_CloseIO(stream);
        return NULL;
    }

    SDL_ReadIO(stream, warFile->offsets, warFile->numberOfEntries * sizeof(u32));

    for (s32 i = 0; i < (s32)warFile->numberOfEntries; ++i)
    {
        // placeholders in demo versions
        if (warFile->offsets[i] == 0xFFFFFFFF ||
            warFile->offsets[i] == 0x00000000)
        {
            warFile->resources[i].placeholder = true;
            continue;
        }

        u32 compressedLength;

        if (i == (s32)warFile->numberOfEntries - 1)
        {
            compressedLength = (u32)fileLength - warFile->offsets[i];
        }
        else
        {
            s32 j = i + 1;
            u32 nextOffset = warFile->offsets[j];
            while (nextOffset == 0xFFFFFFFF || nextOffset == 0x00000000)
            {
                if (j + 1 >= (s32)warFile->numberOfEntries)
                {
                    nextOffset = (u32)fileLength;
                    break;
                }

                nextOffset = warFile->offsets[++j];
            }

            compressedLength = nextOffset - warFile->offsets[i];
        }

        SDL_SeekIO(stream, warFile->offsets[i], SDL_IO_SEEK_SET);

        u32 size;
        SDL_ReadU32LE(stream, &size);
        u32 length = (size & 0x1FFFFFFF);
        bool compressed = (size & 0xE0000000) != 0;

        u8 *data = (u8*)war_malloc(length * sizeof(u8));
        if (!compressed)
        {
            SDL_ReadIO(stream, data, length);
        }
        else
        {
/*
decompression algorithm as described in
The File Formats of WarCraft: Orcs & Humans December 4, 2015
http://www.blizzardarchive.com/pub/Misc/Wc1Book_041215.pdf

for o:=0 to 4095 do
bufwin[o] := 0; // init our 4096 byte buffer with zero

i :=0;

while (i < filesize) do
begin;
warfile.read(cmask, 1);
i := i + 1;
for a:=0 to 7 do
begin;
if (cmask mod 2 = 1) then // uncompressed byte
begin;
    warfile.read(bufbyte, 1);
    bufwin[tmp.position mod 4096] := bufbyte;
    tmp.write(bufbyte, 1);
    i := i + 1;
end;
else // compressed block begin;
    warfile.read(offset, 2);
    numbytes := offset div 4096;
    offset := offset mod 4096;
    i := i + 2;
    for m := 0 to numbytes + 2 do
    begin;
        bufbyte := bufwin[(offset + m) mod 4096];
        bufwin[(tmp.position) mod 4096] := bufbyte;
        tmp.write(bufbyte, 1);
    end;
end;
cmask := cmask div 2;
end;
end;

tmp.size := finalsize; // Crop the file, just in case
*/
#define BUFWIN_SIZE 4096

            u8 bufwin[BUFWIN_SIZE];
            memset(bufwin, 0, BUFWIN_SIZE);

            s32 b = 0;
            s32 bufwinPos = 0;

            while (b < (s32)compressedLength)
            {
                u8 cmask;
                SDL_ReadU8(stream, &cmask);
                b++;

                for (s32 a = 0; a < 8 && bufwinPos < (s32)length; ++a)
                {
                    if (cmask % 2 == 1) // uncompressed byte
                    {
                        u8 bufbyte;
                        SDL_ReadU8(stream, &bufbyte);
                        b++;

                        bufwin[bufwinPos % BUFWIN_SIZE] = bufbyte;
                        data[bufwinPos] = bufbyte;
                        bufwinPos++;
                    }
                    else // compressed block begin
                    {
                        u16 offset;
                        SDL_ReadU16LE(stream, &offset);
                        u16 numbytes = offset / BUFWIN_SIZE;
                        offset = offset % BUFWIN_SIZE;
                        b += 2;

                        for (s32 m = 0; m <= numbytes + 2 && bufwinPos < (s32)length; ++m)
                        {
                            u8 bufbyte = bufwin[(offset + m) % BUFWIN_SIZE];

                            bufwin[bufwinPos % BUFWIN_SIZE] = bufbyte;
                            data[bufwinPos] = bufbyte;
                            bufwinPos++;
                        }
                    }

                    cmask /= 2;
                }
            }
        }

        warFile->resources[i].placeholder = false;
        warFile->resources[i].index = i;
        warFile->resources[i].offset = warFile->offsets[i];
        warFile->resources[i].compressed = compressed;
        warFile->resources[i].compressedLength = compressedLength;
        warFile->resources[i].length = length;
        warFile->resources[i].data = data;
    }

    SDL_CloseIO(stream);
    return warFile;
}
