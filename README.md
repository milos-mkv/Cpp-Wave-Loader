# Cpp Wave Loader
C++ utility library for loading WAVE audio file.

## Example
~~~~~~~~~~~~~.cpp
#include <wave_loader.hpp>
int main(int argc, const char ** argv) 
{
    WaveFile *w = LoadeWaveFile("example.wav");
    ///....
    return 0;
}
~~~~~~~~~~~~~
