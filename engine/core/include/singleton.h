//#pragma once
//
//#include "app/scene_settings.h"
//#include <mutex>
//
//namespace engine
//{
//    class Singleton final
//    {
//    private:
//        Singleton(const SceneSettings value) : sceneSettings_(value) {}
//        static Singleton* singleton_;
//        static std::mutex mutex_;
//        SceneSettings sceneSettings_;
//
//    public:
//        // Delete copy constructor and assignment operator
//        Singleton(Singleton& other) = delete;
//        void operator=(const Singleton&) = delete;
//
//        // Initialize the singleton with a value
//        static void initialize(const SceneSettings& value);
//
//        // Get the singleton instance
//        static Singleton* getInstance();
//
//        // Get the value (return by const reference)
//        SceneSettings& sceneSettings() { return sceneSettings_; }
//    };
//}

#pragma once

#include "app/scene_settings.h"
#include <mutex>

namespace engine
{
    class Singleton final
    {
    private:
        Singleton(const SceneSettings value) : sceneSettings_(value) {}
        static Singleton* singleton_;
        static std::mutex mutex_;
        SceneSettings sceneSettings_;

    public:
        // Delete copy constructor and assignment operator
        Singleton(Singleton& other) = delete;
        void operator=(const Singleton&) = delete;

        // Initialize the singleton with a value
        static void initialize(const SceneSettings& value);

        // Reset the singleton with a new value
        static void reset(const SceneSettings& value);

        // Get the singleton instance
        static Singleton* getInstance();

        // Get the value (return by reference)
        SceneSettings& sceneSettings() { return sceneSettings_; }
    };
}
