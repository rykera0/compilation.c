#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <structures.h>

WAVHEADER read(FILE *file);
int check_format(WAVHEADER header);
DWORD get_data_size(int N, char *songs[]);
int get_block_size(WAVHEADER header);
int combine(int N, char *songs[], char *bgm, float vol, char *name);

//defines constant for length of time between songs
const int seconds = 7;

int main(int argc, char *argv[])
{
	//checks for correct usage
	if (argc < 3)
	{
		printf("correct usage: ./compilation song1.wav ... songN.wav -b [bgFile] -v [bgVolume] -o [outputName]\n");
		return 1;
	}
	//creates an array to store song filepaths and declares initial values for variables 
	char *songs[argc];
	char *bgm = NULL;
	float vol = 1;
	char *name = "output.wav";
	int count = 0;
	//looks through given command prompt to store song filepaths and change values of variables as needed
	for (int i = 1; i < argc; i++)
	{
		char *input = argv[i];
		if (strcasecmp(input, "-b") == 0)
		{
			bgm = argv[i + 1];
			i++;
		}
		else if (strcasecmp(input, "-v") == 0)
		{
			char *s = argv[i + 1];
			vol = atof(s);	
			i++;
		}
		else if (strcasecmp(input, "-o") == 0)
		{
			name = argv[i + 1];
			i++;
		}
		else
		{
			songs[count] = input;
			count++;
		}
	}
	//combines files into compilation
	return combine(count, songs, bgm, vol, name);
}

int combine(int N, char *songs[], char *bgm, float vol, char *name)
{
	//finds total size of output file and creates output file for writing
	DWORD size = get_data_size(N, songs);
	FILE *output = fopen(name, "w");
	FILE *bg = NULL;
	int bg_block_size = 0;
	int bg_blocks = 0;
	//if bg file is provided, checks to see if it's a wav file and finds information about it
	if (bgm != NULL)
	{
		bg = fopen(bgm, "r");
		WAVHEADER bg_header = read(bg);
		if (check_format(bg_header) == 1)
		{
			printf("background file is not a wav file\n");
			fclose(bg);
			fclose(output);
			return 2;
		}
		bg_block_size = get_block_size(bg_header);
		bg_blocks = bg_header.subchunk2Size / bg_block_size;
	}
	//initializes arrays to put the music and background samples into for writing into the output file
	SAMPLE audio[2];
	SAMPLE background[2];
	//loops over all given songs and creates the compilation
	for (int i = 0; i < N; i++)
	{
		FILE *song = fopen(songs[i], "r");
		//reads up to the end of the header of the file and saves its non-LIST header information to a WAVHEADER structure
		WAVHEADER header = read(song);
		//checks to see if the file is a valid WAV file. if not, closes the current files
		if (check_format(header) == 1)
		{
			printf("one or more provided songs are not wav files\n");
			fclose(song);
			if (bgm != NULL)
			{
				fclose(bg);
			}
			fclose(output);
			return 3;
		}
		//if first song in the list, its header is used to write the header of the output file
		if (i == 0)
		{
			WAVHEADER header2 = header;
			header2.subchunk2Size = size;
			fwrite(&header2, sizeof(WAVHEADER), 1, output);
		}
		//gets information about the file and sets a variable for the fade transition
		int block_size = get_block_size(header);
		int blocks = header.subchunk2Size / block_size;
		double fade = seconds * header.sampleRate;
		for (int j = 0; j < blocks; j++)
		{
			//reads song sample into memory, applies transition fading as needed, mixes with bg if given, then writes to output file
			int right = blocks - j;
			fread(&audio, block_size, 1, song);
			if (j <= fade)
			{
				double decrease = j / fade;
				decrease *= decrease;
				audio[0] *= decrease;
				audio[1] *= decrease;
			}
			else if (right <= fade)
			{
				double decrease = right / fade;
				decrease *= decrease;
				audio[0] *= decrease;
				audio[1] *= decrease;
			}
			if (bgm != NULL)
			{
				fread(&background, bg_block_size, 1, bg);
				background[0] *= vol;
				background[1] *= vol;
				audio[0] += background[0];
				audio[1] += background[1];
			}
			fwrite(&audio, block_size, 1, output);
		}
		fclose(song);
	}
	//closes files and returns 
	fclose(output);
	if (bgm != NULL)
	{
		fclose(bg);
	}
	return 0;
}

WAVHEADER read(FILE *file)
{
	//reads header info (before LIST chunk) into memory then checks to see if file is valid
	int num = sizeof(WAVHEADER) - 8;
	WAVHEADER header;
	fread(&header, num, 1, file);
	if (check_format(header) == 1)
	{
		return header;
	}
	BYTE check[2];
	int count = 0;
	//reads through and skips over LIST chunk data and ends after it finds "DATA" identifier
	while (count < 2)
	{
		fread(&check, 2, 1, file);
		if (strcmp(check, "da") == 0)
		{
			count++;
		}
		else if (strcmp(check, "ta") == 0)
		{
			count++;
		}
		else
		{
			count = 0;
		}
	}
	//reads up to sample data then returns header
	strcpy(header.subchunk2ID, "data");
	fread(&header.subchunk2Size, 4, 1, file);
	return header;
}

//checks if header.format string is equal to "WAVE"
int check_format(WAVHEADER header)
{
    char *s = "WAVE";
    for (int i = 0; i < 4; i++)
    {
        if (header.format[i] != s[i])
        {
            return 1;
        }
    }
    return 0;
}


//loops over all given songs then sums up their data sizes to find size of final output file
DWORD get_data_size(int N, char *songs[])
{
	DWORD size = 0;
	for (int i = 0; i < N; i++)
	{
		FILE *song = fopen(songs[i], "r");
		WAVHEADER header = read(song);
		size += header.subchunk2Size;
		fclose(song);
	}
	return size;
}

//gets block size of file
int get_block_size(WAVHEADER header)
{
	int channels = header.numChannels;
	int bytes = header.bitsPerSample / 8;
	return (channels * bytes);
}
