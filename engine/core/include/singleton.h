#pragma once

#include "app/app_settings.h"
#include "app/scene_settings.h"
#include <mutex>

namespace engine
{
    class Singleton final
    {
    private:
        //Singleton(const AppSettings appValue, const SceneSettings sceneValue) : appSettings_(appValue), sceneSettings_(sceneValue) {}
        Singleton() = default;
        static Singleton* singleton_;
        static std::mutex mutex_;
        AppSettings appSettings_;
        SceneSettings sceneSettings_;

    public:
        // Delete copy constructor and assignment operator
        Singleton(Singleton& other) = delete;
        void operator=(const Singleton&) = delete;

        // Initialize the singleton with a value
        //static void initialize(const AppSettings& appValue, const SceneSettings& sceneValue);
        static void initialize(const AppSettings& app);
        static void initialize(const SceneSettings& scene);

        // Reset the singleton with a new value
        //static void reset(const AppSettings& appValue, const SceneSettings& sceneValue);
        static void reset(const AppSettings& appValue);
        static void reset(const SceneSettings& sceneValue);

        // Get the singleton instance
        static Singleton* getInstance();

        // Get the value (return by reference)
        AppSettings& appSettings() { return appSettings_; }

        // Get the value (return by reference)
        SceneSettings& sceneSettings() { return sceneSettings_; }
    };
}
