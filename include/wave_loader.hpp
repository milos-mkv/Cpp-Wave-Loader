//------------------------------------------------------------------------------
/// @file       wave_loader.hpp
/// @author     Milos Milicevic (milosh.mkv@gmail.com)
/// @brief      Single header library for loading WAVE audio files.
/// @version    0.1
/// @date       2020-08-31
///
/// @copyright  Copyright (c) 2020
///
/// Distributed under the MIT software license, see the accompanying
/// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//------------------------------------------------------------------------------

#ifndef WAVE_LOADER_H
#define WAVE_LOADER_H

#include <fstream>
#include <string.h>

struct WaveFile
{
	// The canonical WAVE format starts with the RIFF header:
    
    /// @brief  Contains the letters "RIFF" in ASCII form (0x52494646 big-endian form).
    ///
	int chunk_id;

    /// @brief  36 + SubChunk2Size, or more precisely: 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
    ///         This is the size of the rest of the chunk following this number. 
    ///         This is the size of the entire file in bytes minus 8 bytes for the  two fields not included in this count: ChunkID and ChunkSize.
    ///
	int chunk_size;

    /// @brief  Contains the letters "WAVE" (0x57415645 big-endian form).
    ///
	int format;

	// The "WAVE" format consists of two subchunks: "fmt " and "data":
	// The "fmt " subchunk describes the sound data's format:

    /// @brief  Contains the letters "fmt " (0x666d7420 big-endian form).
    ///
	int subchunk_1_id;

    /// @brief  16 for PCM.  This is the size of the rest of the Subchunk which follows this number.
    ///
	int subchunk_1_size;

    /// @brief  PCM = 1 (i.e. Linear quantization) Values other than 1 indicate some form of compression. 
    ///
	short audio_format;

    /// @brief  Mono = 1, Stereo = 2, etc. 
    ///
	short num_channels;

	/// @brief  8000, 44100, etc. 
	///
	int sample_rate;

	/// @brief  == SampleRate * NumChannels * BitsPerSample/8 
	///
	int byte_rate;

    /// @brief  == NumChannels * BitsPerSample/8
    ///         The number of bytes for one sample including all channels. I wonder what happens when this number isn't an integer?
    ///
	short block_align;

    /// @brief  8 bits = 8, 16 bits = 16, etc.
    ///
	short bits_per_sample;

	// Here should come some extra parameters which i will avoid.
	// The "data" subchunk contains the size of the data and the actual sound:
    
    /// @brief  Contains the letters "data" (0x64617461 big-endian form).
    ///
	int subchunk_2_id;

    /// @brief  == NumSamples * NumChannels * BitsPerSample/8
    ///         This is the number of bytes in the data. You can also think of this as the size of the read of the subchunk following this number.
	///
    int subchunk_2_size;

    /// @brief  The actual sound data.
    ///
	char * data;
};

static int is_big_endian()
{
	int a = 1;
	return !((char*)&a)[0];
}

static int convert_to_int(char *buffer, int len)
{
	int a = 0;
	if(!is_big_endian())
	{
		for(int i = 0; i < len; i++)
		{
			((char*) &a)[i] = buffer[i];
		}
	}
	else
	{
		for(int i = 0; i < len; i++)
		{
			((char*) &a)[3- i] = buffer[i];
		}
	}
	return a;
}

//------------------------------------------------------------------------------
/// @brief  Load wave file. 
/// 
/// @param file_path    Path to wave file.
/// @return WaveFile* 
WaveFile * LoadeWaveFile(const char * file_path)
{
    WaveFile * wave_file = new WaveFile();
    std::ifstream file(file_path);

    char buffer[4];
    file.read(buffer, 4);
    if(strncmp(buffer, "RIFF", 4) != 0)
	{
		printf("This is not valid WAV file!");
	}
    wave_file->chunk_id = convert_to_int(buffer, 4);
    file.read(buffer, 4);
    wave_file->chunk_size = convert_to_int(buffer, 4);
    file.read(buffer, 4);
    wave_file->format = convert_to_int(buffer, 4);
    file.read(buffer, 4);
    wave_file->subchunk_1_id = convert_to_int(buffer, 4);
    file.read(buffer, 2);
    wave_file->audio_format = convert_to_int(buffer, 2);
    file.read(buffer, 2);
    wave_file->num_channels = convert_to_int(buffer, 2);
    file.read(buffer, 4);
    wave_file->sample_rate = convert_to_int(buffer, 4);
    file.read(buffer, 4);
    wave_file->byte_rate = convert_to_int(buffer, 4);
    file.read(buffer, 2);
    wave_file->block_align = convert_to_int(buffer, 2);
    file.read(buffer, 2);
    wave_file->bits_per_sample = convert_to_int(buffer, 2);
    file.read(buffer, 4);
    if(strncmp(buffer, "data", 4) != 0)
    {
        file.read(buffer, 4);
        int _extra_size = convert_to_int(buffer, 4);
        char * _extra_data = new char[_extra_size];
        file.read(_extra_data, _extra_size);
        file.read(buffer, 4);
        wave_file->subchunk_2_id = convert_to_int(buffer, 4);
        delete _extra_data;
    }
    else
    {
        wave_file->subchunk_2_id = convert_to_int(buffer, 4);
    }

    file.read(buffer, 4);
    wave_file->subchunk_2_size = convert_to_int(buffer, 4);
    wave_file->data = new char[wave_file->subchunk_2_size];
    file.read(wave_file->data, wave_file->subchunk_2_size);

    return wave_file;
}

#endif