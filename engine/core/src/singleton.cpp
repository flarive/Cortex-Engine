#include "../include/singleton.h"

namespace engine
{
    Singleton* Singleton::singleton_ = nullptr;
    std::mutex Singleton::mutex_;

    //void Singleton::initialize(const AppSettings& appValue, const SceneSettings& sceneValue)
    //{
    //    std::lock_guard<std::mutex> lock(mutex_);
    //    if (!singleton_)
    //    {
    //        singleton_ = new Singleton(appValue, sceneValue);
    //    }
    //}

    void Singleton::initialize(const AppSettings& app)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!singleton_)
            singleton_ = new Singleton();

        singleton_->appSettings_ = app;
    }

    void Singleton::initialize(const SceneSettings& scene)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!singleton_)
            singleton_ = new Singleton();

        singleton_->sceneSettings_ = scene;
    }

    /*void Singleton::reset(const AppSettings& appValue, const SceneSettings& sceneValue)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (singleton_)
        {
            delete singleton_;
        }
        singleton_ = new Singleton(appValue, sceneValue);
    }*/

    void Singleton::reset(const AppSettings& appValue)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!singleton_)
            singleton_ = new Singleton();

        singleton_->appSettings_ = appValue;
    }

    void Singleton::reset(const SceneSettings& sceneValue)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!singleton_)
            singleton_ = new Singleton();

        singleton_->sceneSettings_ = sceneValue;
    }

    Singleton* Singleton::getInstance()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return singleton_;
    }
}