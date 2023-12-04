#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
    char *file;
    bool bgm;
    bool output;
} song;

typedef uint8_t   BYTE;
typedef uint16_t  WORD;
typedef int16_t   SAMPLE;
typedef uint32_t  DWORD;

typedef struct
{
    BYTE   chunkID[4];
    DWORD  chunkSize;
    BYTE   format[4];
    BYTE   subchunk1ID[4];
    DWORD  subchunk1Size;
    WORD   audioFormat;
    WORD   numChannels;
    DWORD  sampleRate;
    DWORD  byteRate;
    WORD   blockAlign;
    WORD   bitsPerSample;
    BYTE   subchunk2ID[4];
    DWORD  subchunk2Size;
} __attribute__((__packed__))
WAVHEADER;
