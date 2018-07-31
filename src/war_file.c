WarFile* loadWarFile(char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        return NULL;
    }

    u64 fileLength = flength(file);

    WarFile *warFile = (WarFile*)xcalloc(1, sizeof(WarFile));
    warFile->archiveID = fileReadU32(file);
    warFile->numberOfEntries = fileReadU32(file);

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
            warFile->type = WAR_FILE_TYPE_UNKOWN;
            break;
        }
    }

    fileReadBytes((u8*)warFile->offsets, warFile->numberOfEntries * sizeof(u32), file);

    for (int i = 0; i < warFile->numberOfEntries; ++i)
    {
        // placeholders in demo versions
        if (warFile->offsets[i] == 0xFFFFFFFF ||
            warFile->offsets[i] == 0x00000000)
        {
            warFile->resources[i].placeholder = true;
            continue;
        }
        
        u32 compressedLength;
        
        if (i == warFile->numberOfEntries - 1)
        {
            compressedLength = (u32)fileLength - warFile->offsets[i];
        }
        else
        {
            s32 j = i+1;
            u32 nextOffset = warFile->offsets[j];
            while (nextOffset == 0xFFFFFFFF || nextOffset == 0x00000000)
            {
                if (j + 1 >= warFile->numberOfEntries)
                {
                    nextOffset = (u32)fileLength;
                    break;
                }

                nextOffset = warFile->offsets[++j];
            }

            compressedLength = nextOffset - warFile->offsets[i];
        }

        fseek(file, warFile->offsets[i], SEEK_SET);

        u32 size = fileReadU32(file);
        u32 length = (size & 0x1FFFFFFF);
        bool compressed = (size & 0xE0000000) != 0;

        u8 *data = (u8*)xcalloc(length, sizeof(u8));
        if (!compressed)
        {
            fileReadBytes(data, length, file);
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

            while (b < compressedLength)
            {
                u8 cmask = fileReadU8(file);
                b++;

                for (s32 a = 0; a < 8 && bufwinPos < length; ++a)
                {
                    if (cmask % 2 == 1) // uncompressed byte 
                    {
                        u8 bufbyte = fileReadU8(file);
                        b++;
                        
                        bufwin[bufwinPos % BUFWIN_SIZE] = bufbyte;
                        data[bufwinPos] = bufbyte;
                        bufwinPos++;
                    }
                    else // compressed block begin
                    {
                        u16 offset = fileReadU16(file);
                        u16 numbytes = offset / BUFWIN_SIZE;
                        offset = offset % BUFWIN_SIZE;
                        b += 2;

                        for (s32 m = 0; m <= numbytes + 2 && bufwinPos < length; ++m)
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

    fclose(file);
    return warFile;
}

