#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
typedef struct {
    char riff[4];
    int chunkSize;
    char wave[4];
    char fmt[4];
    int subChunkSize;
    short audioFormat;
    short numChannels;
    int sampleRate;
    int byteRate;
    short blockAlign;
    short bitsPerSample;
} WavHeader;

typedef struct {
    char id[4];
    int size;
} WavDataHeader;
#pragma pack()

void *loadWAV(const char *filename, ALenum *format, ALsizei *size, ALsizei *freq) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open file %s\n", filename);
        return NULL;
    }

    WavHeader header;
    fread(&header, sizeof(WavHeader), 1, file);

    if (strncmp(header.riff, "RIFF", 4) != 0 || strncmp(header.wave, "WAVE", 4) != 0) {
        printf("Invalid WAV file\n");
        fclose(file);
        return NULL;
    }

    WavDataHeader dataHeader;
    fread(&dataHeader, sizeof(WavDataHeader), 1, file);

    *size = dataHeader.size;
    *freq = header.sampleRate;

    if (header.numChannels == 1) {
        if (header.bitsPerSample == 8)
            *format = AL_FORMAT_MONO8;
        else if (header.bitsPerSample == 16)
            *format = AL_FORMAT_MONO16;
    } else if (header.numChannels == 2) {
        if (header.bitsPerSample == 8)
            *format = AL_FORMAT_STEREO8;
        else if (header.bitsPerSample == 16)
            *format = AL_FORMAT_STEREO16;
    }

    void *data = malloc(dataHeader.size);
    fread(data, dataHeader.size, 1, file);
    fclose(file);

    return data;
}

int main(int argc, char **argv) {
  printf("Hello OAL\n");
  int g_bEAX;
  ALCcontext *Context = NULL;
  ALCdevice *Device = alcOpenDevice(NULL);
  if (Device) {
    Context = alcCreateContext(Device, NULL);
    alcMakeContextCurrent(Context);
  }

  g_bEAX = alIsExtensionPresent("EAX2.0");
  
  alGetError();

    ALsizei buffersCount = 1;

  ALuint g_Buffers[buffersCount];
  
  ALenum error;
  alGenBuffers(buffersCount, g_Buffers);

  if ((error = alGetError()) != AL_NO_ERROR) {
    printf("alGenBuffers : %d", error);
    return 1;
  }

  ALenum format;
  void *data;
  ALsizei size;
  ALsizei freq;

  data = loadWAV("test.wav", &format, &size, &freq);
  if ((error = alGetError()) != AL_NO_ERROR) {
    printf("alutLoadWAVFile test.wav : %d\n", error);
    alDeleteBuffers(buffersCount, g_Buffers);
    return 1;
  }

  alBufferData(g_Buffers[0], format, data, size, freq);
  if ((error = alGetError()) != AL_NO_ERROR) {
    printf("alBufferData buffer 0 : %d\n", error);
    alDeleteBuffers(buffersCount, g_Buffers);
    return 1;
  }

  free(data);

  ALuint source;
  alGenSources(1, &source);
  if ((error = alGetError()) != AL_NO_ERROR) {
    printf("alGenSources 1 : %d\n", error);
    return 1;
  }

  alSourcei(source, AL_BUFFER, g_Buffers[0]);
  if ((error = alGetError()) != AL_NO_ERROR) {
    printf("alSourcei AL_BUFFER 0 : %d\n", error);
  }

  alSourcePlay(source);
  if ((error = alGetError()) != AL_NO_ERROR) {
    printf("alSourcePlay : %d\n", error);
    return 1;
  }

#if __EMSCRIPTEN__
  printf("Do not close because of Emscripten\n");
#else
  ALint source_state;
  do {
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    sleep(1);
  } while (source_state == AL_PLAYING);

  printf("Success\n");

  Context = alcGetCurrentContext();
  Device = alcGetContextsDevice(Context);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(Context);
  alcCloseDevice(Device);
#endif

  return 0;
}