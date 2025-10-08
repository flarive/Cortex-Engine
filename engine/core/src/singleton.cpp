#include "../include/singleton.h"

engine::Singleton* engine::Singleton::singleton_ = nullptr;
std::mutex engine::Singleton::mutex_;

void engine::Singleton::initialize(const SceneSettings& value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (singleton_ == nullptr) {
        singleton_ = new Singleton(value);
    }
}

engine::Singleton* engine::Singleton::getInstance()
{
    return singleton_;
}