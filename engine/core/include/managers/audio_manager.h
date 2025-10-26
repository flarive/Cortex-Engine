#pragma once

#include <vector>
#include <string>
#include <AL/al.h>
#include <AL/alc.h>

#define ALSOFT_LOGFILE "openal_log.txt"

namespace engine
{
    class AudioManager final
    {
    public:
        AudioManager();
        ~AudioManager();

        void loadOgg(const std::string& id, const std::string& filename);
        void play(const std::string& id);
        void clean();

    private:
        ALCdevice* m_device{};
        ALCcontext* m_context{};

        struct AudioData {
            ALuint source;
            ALuint buffer;
        };

        std::vector<std::pair<std::string, AudioData>> m_audioMap{};

        void initOpenAL();
        AudioData* findAudio(const std::string& id);
    };
}
