#pragma once

#include <vector>
#include <string>
#include <atomic>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

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

        using InitCallback = std::function<void(bool success)>;

        void setInitCallback(InitCallback callback);
        bool isInitialized() const;

        

        void loadOgg(const std::string& id, const std::string& filename);
        void play(const std::string& id);
        void clean();

    private:
        ALCdevice* m_device{};
        ALCcontext* m_context{};

        std::atomic<bool> m_initialized{ false };
        std::thread m_initThread{};

        InitCallback m_initCallback;
        std::mutex m_callbackMutex;
        std::condition_variable m_callbackCV;

        struct AudioData {
            ALuint source;
            ALuint buffer;
        };

        std::vector<std::pair<std::string, AudioData>> m_audioMap{};

        //void initOpenAL();
        void initOpenALInternal();
        AudioData* findAudio(const std::string& id);
    };
}
