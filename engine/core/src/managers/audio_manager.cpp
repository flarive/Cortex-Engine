#include "../../include/managers/audio_manager.h"

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"


#include "../../include/managers/log_manager.h"

engine::AudioManager::AudioManager()
{
    //initOpenAL(); // uncomment !
}

engine::AudioManager::~AudioManager()
{
    clean();
}


void engine::AudioManager::initOpenAL()
{
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        logger.warn("Failed to open OpenAL device");
        return;
    }
    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) {
        logger.warn("Failed to create OpenAL context");
        alcCloseDevice(m_device);
        return;
    }
    alcMakeContextCurrent(m_context);
}

void engine::AudioManager::loadOgg(const std::string& id, const std::string& filename)
{
    int channels{}, sampleRate{};
    short* output;
    int samples = stb_vorbis_decode_filename(filename.c_str(), &channels, &sampleRate, &output);

    if (samples < 0) {
        logger.warn("Failed to decode OGG file:{}", filename);
        if (output) free(output); // Ensure cleanup
        return;
    }

    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    ALuint buffer, source;

    alGenBuffers(1, &buffer);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        logger.warn("OpenAL error after alGenBuffers: {}", error);
        return;
    }


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
        ALint state;
        alGetSourcei(audio->source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            alSourcePlay(audio->source);
        }
        else {
            logger.warn("Audio ID {} already playing", id);
        }
    }
    else {
        logger.warn("Audio ID not found:{}", id);
    }
}

void engine::AudioManager::clean()
{
    for (auto& [id, data] : m_audioMap) {
        ALint state;
        alGetSourcei(data.source, AL_SOURCE_STATE, &state);
        if (state == AL_PLAYING) {
            alSourceStop(data.source);
        }
        // Detach buffer from source
        alSourcei(data.source, AL_BUFFER, 0);
        // Delete source and buffer
        alDeleteSources(1, &data.source);
        alDeleteBuffers(1, &data.buffer);
    }
    m_audioMap.clear();

    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        logger.warn("OpenAL error during cleanup: {}", error);
    }

    // Destroy context and close device
    if (m_context) {
        // Make context non-current
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
        m_context = nullptr;
    }
    if (m_device) {
        alcCloseDevice(m_device);
        m_device = nullptr;
    }
}

