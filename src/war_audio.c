// Holds global MIDI playback state
static double g_Msec;               //current playback time
static tml_message* g_MidiMessage;  //next message to be played

void audioDataCallback(ma_device* sfx, void* output, const void* input, ma_uint32 frameCount)
{
    NOT_USED(input);

    // WarContext* context = (WarContext*)sfx->pUserData;
    // if (!context) 
    // {
    //     return;
    // }

    // s16* stream = (s16*)output;

    // s32 samplesPerSecond = 44100;
    // s32 toneHz = 256;
    // s32 wavePeriod = samplesPerSecond / toneHz;
    // s32 sampleSize = sizeof(s16);
    // s32 toneVolume = 32767;
    // #define PI 3.14159265359f

    // static u32 wavePos = 0;
    // s16 *data = (s16*)output;
    // for (s32 i = 0; i < frameCount ; i++)
    // {
    //     f32 t = 2.0f * PI * (f32)wavePos / (f32)wavePeriod;
    //     f32 sineValue = sinf(t);
    //     s16 sampleValue = (s16)(sineValue * toneVolume);
    //     data[i] = sampleValue;
    //     ++wavePos;
    // }

    // u8* data = context->resources[501]->wave.data;
    // s32 dataLength = context->resources[501]->wave.length;
    
    // printf("frameCount: %d\n", frameCount);
    // printf("pos: %d\n", context->resources[501]->wave.pos);

    // for (s32 i = 0; i < frameCount; i++)
    // {
    //     if (context->resources[501]->wave.pos >= dataLength)
    //     {
    //         context->resources[501]->wave.pos = 0;
    //     }

    //     // this internally do this: (sample - 0x80) << 8;
    //     // this can be used smarter, and copy whole pieces of the samples
    //     ma_pcm_u8_to_s16(&stream[i], &data[context->resources[501]->wave.pos++], 1, ma_dither_mode_none);
    // }

	// s32 sampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK;

    // while (frameCount)
    // {
    //     //We progress the MIDI playback and then process TSF_RENDER_EFFECTSAMPLEBLOCK samples at once
	// 	if (sampleBlock > frameCount)
    //     {
    //         sampleBlock = frameCount;
    //     }

    //     g_Msec += sampleBlock * (1000.0 / 44100.0);

    //     //Loop through all MIDI messages which need to be played up until the current playback time
    //     while (g_MidiMessage && g_MidiMessage->time <= g_Msec)
    //     {
    //         switch (g_MidiMessage->type)
	// 		{
	// 			case TML_PROGRAM_CHANGE: //channel program (preset) change (special handling for 10th MIDI channel with drums)
	// 				tsf_channel_set_presetnumber(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->program, (g_MidiMessage->channel == 9));
	// 				break;
	// 			case TML_NOTE_ON: //play a note
	// 				tsf_channel_note_on(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key, g_MidiMessage->velocity / 127.0f);
	// 				break;
	// 			case TML_NOTE_OFF: //stop a note
	// 				tsf_channel_note_off(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->key);
	// 				break;
	// 			case TML_PITCH_BEND: //pitch wheel modification
	// 				tsf_channel_set_pitchwheel(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->pitch_bend);
	// 				break;
	// 			case TML_CONTROL_CHANGE: //MIDI controller messages
	// 				tsf_channel_midi_control(g_TinySoundFont, g_MidiMessage->channel, g_MidiMessage->control, g_MidiMessage->control_value);
	// 				break;
	// 		}

    //         g_MidiMessage = g_MidiMessage->next;
    //     }

	// 	// Render the block of audio samples in float format
	// 	tsf_render_short(g_TinySoundFont, stream, sampleBlock, 0);

    //     frameCount -= sampleBlock;
    //     stream += sampleBlock;
    // }
}

bool initAudio(WarContext* context)
{
    // g_MidiMessage = tml_load_memory(midiData, midiLength);
    // if (!g_MidiMessage)
    // {
    //     logError("Could not load MIDI file\n");
    //     return 1;
    // }

    context->soundFont = tsf_load_filename("GMGeneric.SF2");
    if (!context->soundFont)
    {
        logError("Could not load SoundFont\n");
        return false;
    }

   //Initialize preset on special 10th MIDI channel to use percussion sound bank (128) if available
    tsf_channel_set_bank_preset(context->soundFont, 9, 128, 0);

    // Set the SoundFont rendering output mode
    tsf_set_output(context->soundFont, TSF_MONO, 44100, 0.0f);

    ma_device_config sfxConfig;
    sfxConfig = ma_device_config_init(ma_device_type_playback);
    sfxConfig.playback.format = ma_format_s16;
    sfxConfig.playback.channels = 1;
    sfxConfig.sampleRate = 44100;
    sfxConfig.dataCallback = audioDataCallback;
    sfxConfig.pUserData = context;

    printf("1\n");

    if (ma_device_init(NULL, &sfxConfig, &context->sfx) != MA_SUCCESS) {
        logError("Failed to open playback device.\n");
        return false;
    }

    printf("2\n");

    if (ma_device_start(&context->sfx) != MA_SUCCESS) {
        logError("Failed to start playback device.\n");
        ma_device_uninit(&context->sfx);
        return false;
    }

   return true;
}

/**
 * Extension function of MemoryBuffer to read variable lengths integer values.
 */
bool mbReadUIntVar(MemoryBuffer* buffer, u32* value)
{
    u32 v = 0;
    u8 byte;
    for(s32 i = 0; i < 4; ++i)
    {
        if(!mbRead(buffer, &byte))
            return false;

        v = (v << 7) | (u32)(byte & 0x7F);
        if((byte & 0x80) == 0)
            break;
    }
    *value = v;
    return true;
}

/**
 * Extension function of MemoryBuffer to write variable lengths integer values.
 */
bool mbWriteUIntVar(MemoryBuffer* buffer, u32 value)
{
    s32 byteCount = 1;
    u32 v = value & 0x7F;
    value >>= 7;
    while (value)
    {
        v = (v << 8) | 0x80 | (value & 0x7F);
        ++byteCount;
        value >>= 7;
    }
    
    for(s32 i = 0; i < byteCount; ++i)
    {
        u8 byte = v & 0xFF;
        if(!mbWrite(buffer, byte))
            return false;
        v >>= 8;
    }
    return true;
}

typedef struct _MidiToken
{
    s32 time;
    u32 bufferLength;
    u8* buffer;
    u8  type;
    u8  data;
} MidiToken;

static s32 compareTokens(const MidiToken left, const MidiToken right)
{
    return left.time - right.time;
}

shlDeclareList(MidiTokenList, MidiToken)
shlDefineList(MidiTokenList, MidiToken)

static MidiToken* MidiTokenListAppend(MidiTokenList* list, s32 time, u8 type)
{
    MidiToken token = {0};
    token.time = time;
    token.type = type;
    MidiTokenListAdd(list, token);
    return &list->items[list->count - 1];
}

/**
 * This code is a port in C of the XMI2MID converter by Peter "Corsix" Cawley 
 * in the War1gus repository. You can find the original C++ code here: 
 * https://github.com/Wargus/war1gus/blob/master/xmi2mid.cpp
 */
u8* transcodeXmiToMid(u8* xmiData, size_t xmiLength, size_t* midLength)
{
    MemoryBuffer bufInput = {0};
    mbInitFromMemory(&bufInput, xmiData, xmiLength);

    MemoryBuffer bufOutput = {0};
    mbInitEmpty(&bufOutput);

    if (!mbScanTo(&bufInput, "EVNT", 4))
    {
        mbFree(&bufOutput);
        return NULL;
    }

    if (!mbSkip(&bufInput, 8))
    {
        mbFree(&bufOutput);
        return NULL;
    }

    MidiTokenListOptions options = {0};

    MidiTokenList lstTokens;
    MidiTokenListInit(&lstTokens, options);

    MidiToken* token;
    s32 tokenTime = 0;
    s32 tempo = 500000;
    bool tempoSet = false;
    bool end = false;
    u8 tokenType, extendedType;
    u32 intVar;

    while (!mbIsEOF(&bufInput) && !end)
    {
        while (true)
        {
            if (!mbRead(&bufInput, &tokenType))
            {
                mbFree(&bufOutput);
                return NULL;
            }

            if (tokenType & 0x80)
                break;

            tokenTime += (s32)tokenType * 3;
        }

        token = MidiTokenListAppend(&lstTokens, tokenTime, tokenType);
        token->buffer = bufInput._pointer + 1;

        switch (tokenType & 0xF0)
        {
            case 0xC0:
            case 0xD0:
            {
                if (!mbRead(&bufInput, &token->data))
                {
                    mbFree(&bufOutput);
                    return NULL;
                }

                token->buffer = NULL;
                break;
            }

            case 0x80:
            case 0xA0:
            case 0xB0:
            case 0xE0:
            {
                if (!mbRead(&bufInput, &token->data))
                {
                    mbFree(&bufOutput);
                    return NULL;
                }

                if (!mbSkip(&bufInput, 1))
                {
                    mbFree(&bufOutput);
                    return NULL;
                }

                break;
            }

            case 0x90:
            {
                if (!mbRead(&bufInput, &extendedType))
                {
                    mbFree(&bufOutput);
                    return NULL;
                }

                token->data = extendedType;

                if (!mbSkip(&bufInput, 1))
                {
                    mbFree(&bufOutput);
                    return NULL;
                }

                mbReadUIntVar(&bufInput, &intVar);
                token = MidiTokenListAppend(&lstTokens, tokenTime + intVar * 3, tokenType);
                token->data = extendedType;
                token->buffer = (u8*)"\0";
            
                break;
            }

            case 0xF0:
            {
                extendedType = 0;

                if (tokenType == 0xFF)
                {
                    if (!mbRead(&bufInput, &extendedType))
                    {
                        mbFree(&bufOutput);
                        return NULL;
                    }

                    if (extendedType == 0x2F)
                        end = true;
                    else if (extendedType == 0x51)
                    {
                        if (tempoSet)
                        {
                            mbSkip(&bufInput, 1);
                            mbReadInt24BE(&bufInput, &tempo);
                            tempo *= 3;
                            tempoSet = true;
                            mbSkip(&bufInput, 1);
                        }
                        else
                        {
                            MidiTokenListRemoveAt(&lstTokens, lstTokens.count - 1);
                            mbReadUIntVar(&bufInput, &intVar);
                            if (!mbSkip(&bufInput, intVar))
                            {
                                mbFree(&bufOutput);
                                return NULL;
                            }
                            break;
                        }
                    }
                }

                token->data = extendedType;
                mbReadUIntVar(&bufInput, &token->bufferLength);
                token->buffer = bufInput._pointer;

                if (!mbSkip(&bufInput, token->bufferLength))
                {
                    mbFree(&bufOutput);
                    return NULL;
                }

                break;
            }
        }
    }

    if (lstTokens.count == 0)
    {
        mbFree(&bufOutput);
        return NULL;
    }
    if (!mbWriteString(&bufOutput, "MThd\0\0\0\x06\0\0\0\x01", 12))
    {
        mbFree(&bufOutput);
        return NULL;
    }
    if (!mbWriteUInt16BE(&bufOutput, (tempo * 3) / 25000))
    {
        mbFree(&bufOutput);
        return NULL;
    }
    if (!mbWriteString(&bufOutput, "MTrk\xBA\xAD\xF0\x0D", 8))
    {
        mbFree(&bufOutput);
        return NULL;
    }

    MidiTokenListSort(&lstTokens, compareTokens);

    tokenTime = 0;
    tokenType = 0;
    end = false;

    for (s32 i = 0; i < lstTokens.count && !end; i++)
    {
        MidiToken t = lstTokens.items[i];

        if (!mbWriteUIntVar(&bufOutput, t.time - tokenTime))
        {
            mbFree(&bufOutput);
            return NULL;
        }

        tokenTime = t.time;

        if (t.type >= 0xF0)
        {
            tokenType = t.type;
            if (!mbWrite(&bufOutput, tokenType))
            {
                mbFree(&bufOutput);
                return NULL;
            }

            if (tokenType == 0xFF)
            {
                if (!mbWrite(&bufOutput, t.data))
                {
                    mbFree(&bufOutput);
                    return NULL;
                }

                if (t.data == 0x2F)
                    end = true;
            }

            if (!mbWriteUIntVar(&bufOutput, t.bufferLength))
            {
                mbFree(&bufOutput);
                return NULL;
            }
            if (!mbWriteBytes(&bufOutput, t.buffer, t.bufferLength))
            {
                mbFree(&bufOutput);
                return NULL;
            }
        }
        else
        {
            if (t.type != tokenType)
            {
                tokenType = t.type;

                if (!mbWrite(&bufOutput, tokenType))
                {
                    mbFree(&bufOutput);
                    return NULL;
                }
            }

            if (!mbWrite(&bufOutput, t.data))
            {
                mbFree(&bufOutput);
                return NULL;
            }

            if (t.buffer)
            {
                if (!mbWriteBytes(&bufOutput, t.buffer, 1))
                {
                    mbFree(&bufOutput);
                    return NULL;
                }
            }
        }
    }

    size32 length = mbPosition(&bufOutput) - 22;
    mbSeek(&bufOutput, 18);
    mbWriteUInt32BE(&bufOutput, length);

    u8* midData = mbGetData(&bufOutput, midLength);

    mbFree(&bufOutput);
    return midData;
}

u8* changeSampleRate(u8* samplesIn, s32 length, s32 factor)
{
    assert(factor >= 1);

    s32 newLength = length * factor;
    u8* samplesOut = (u8*)xmalloc(newLength);

    samplesOut[0] = samplesIn[0];

    for (s32 i = 1, j = 0; i < length; i++)
    {
        u8 a = samplesIn[i - 1];
        u8 b = samplesIn[i];

        f32 dt = 1 / factor;
        for (s32 k = 0; k < factor - 1; k++)
        {
            // linear interpolation: a + (b - a) * t
            f32 t = dt * (k + 1);
            samplesOut[j++] = (u8)(a + (b - a) * t);
        }

        samplesOut[j++] = b;
    }

    return samplesOut;
}
