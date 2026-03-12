#pragma once

#include <vector>
#include <stdexcept>

#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.h"

#include "iarchive.h"
#include "stream/ScopedArchiveBuffer.h"

namespace sound
{

/**
 * greebo: Loader class creating an AL buffer from an OGG file.
 */
class OggFileLoader
{
public:
    /**
     * greebo: Determines the OGG file length in seconds.
     * @throws: std::runtime_error if an error occurs.
     */
    static float GetDuration(ArchiveFile& vfsFile)
    {
        archive::ScopedArchiveBuffer buffer(vfsFile);

        int error = 0;
        stb_vorbis *vorbis = stb_vorbis_open_memory(
            buffer.buffer,
            static_cast<int>(buffer.length),
            &error,
            nullptr
        );

        if ( !vorbis )
        {
            throw std::runtime_error("stb_vorbis: failed to open OGG data (error " + std::to_string(error) + ")");
        }

        float duration = stb_vorbis_stream_length_in_seconds(vorbis);
        stb_vorbis_close(vorbis);

        return duration;
    }

    /**
     * greebo: Loads an OGG file from the given stream into OpenAL,
     * returns the openAL buffer handle.
     *
     * @throws: std::runtime_error if an error occurs.
     */
    static ALuint LoadFromFile(ArchiveFile& vfsFile)
    {
        archive::ScopedArchiveBuffer buffer(vfsFile);

        int channels   = 0;
        int sampleRate = 0;
        short* decoded = nullptr;

        // Decode the entire stream to interleaved 16-bit PCM in one call.
        int numSamples = stb_vorbis_decode_memory(
            buffer.buffer,
            static_cast<int>(buffer.length),
            &channels,
            &sampleRate,
            &decoded
        );

        if (numSamples < 0 || !decoded)
        {
            throw std::runtime_error("stb_vorbis: failed to decode OGG data");
        }

        ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        ALsizei freq  = static_cast<ALsizei>(sampleRate);

        // numSamples is per-channel; total shorts = numSamples * channels
        ALsizei dataSize = static_cast<ALsizei>(numSamples * channels * sizeof(short));

        ALuint bufferNum = 0;
        alGenBuffers(1, &bufferNum);
        alBufferData(bufferNum, format, decoded, dataSize, freq);

        // stb_vorbis allocates the output buffer with malloc; free it.
        free(decoded);

        return bufferNum;
    }
};

}
