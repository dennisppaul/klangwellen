#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>

void write_WAV_file(const char* filename, int sampleRate, int numChannels, int bitsPerSample, const int16_t* data, int numSamples);
void write_WAV_file(const char* filename, int sampleRate, int numChannels, const float* data, int numSamples);
