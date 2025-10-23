#include "../../include/managers/audio_manager.h"

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"


#include "../../include/managers/log_manager.h"

engine::AudioManager::AudioManager()
{
    initOpenAL();
}

engine::AudioManager::~AudioManager()
{
    for (auto& [id, data] : m_audioMap)
    {
        alDeleteSources(1, &data.source);
        alDeleteBuffers(1, &data.buffer);
    }

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

void engine::AudioManager::initOpenAL()
{
    m_device = alcOpenDevice(nullptr);
    m_context = alcCreateContext(m_device, nullptr);
    alcMakeContextCurrent(m_context);
}

void engine::AudioManager::loadOgg(const std::string& id, const std::string& filename)
{
    int channels{}, sampleRate{};
    short* output;
    int samples = stb_vorbis_decode_filename(filename.c_str(), &channels, &sampleRate, &output);

    if (samples < 0) {
        logger.warn("Failed to decode OGG file:{}", filename);
    }

    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    ALuint buffer, source;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, output, samples * channels * sizeof(short), sampleRate);
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);

    free(output);

    // Insert into flat map (sorted vector)
    auto it = std::lower_bound(
        m_audioMap.begin(), m_audioMap.end(), id,
        [](const std::pair<std::string, AudioData>& pair, const std::string& key) {
            return pair.first < key;
        });

    if (it != m_audioMap.end() && it->first == id) {
        it->second = { source, buffer }; // Replace existing
    }
    else {
        m_audioMap.insert(it, { id, {source, buffer} });
    }
}

engine::AudioManager::AudioData* engine::AudioManager::findAudio(const std::string& id)
{
    auto it = std::lower_bound(
        m_audioMap.begin(), m_audioMap.end(), id,
        [](const std::pair<std::string, AudioData>& pair, const std::string& key) {
            return pair.first < key;
        });

    if (it != m_audioMap.end() && it->first == id)
        return &it->second;

    return nullptr;
}

void engine::AudioManager::play(const std::string& id)
{
    AudioData* audio = findAudio(id);
    if (audio) {
        alSourcePlay(audio->source);
    }
    else {
        logger.warn("Audio ID not found:{}", id);
    }
}