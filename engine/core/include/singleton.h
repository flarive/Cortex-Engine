#pragma once

#include "app/scene_settings.h"

namespace engine
{
    /**
     * The Singleton class defines the `GetInstance` method that serves as an
     * alternative to constructor and lets clients access the same instance of this
     * class over and over.
     */
    class Singleton
    {
        /**
         * The Singleton's constructor should always be private to prevent direct
         * construction calls with the `new` operator.
         */

    public:
        Singleton(const SceneSettings value) : value_(value)
        {
        }

        static Singleton* singleton_;

        SceneSettings value_;

    public:

        /**
         * Singletons should not be clonable.
         */
        Singleton(Singleton& other) = delete;
        /**
         * Singletons should not be assignable.
         */
        void operator=(const Singleton&) = delete;
        /**
         * This is the static method that controls the access to the singleton
         * instance. On the first run, it creates a singleton object and places it
         * into the static field. On subsequent runs, it returns the client existing
         * object stored in the static field.
         */

         //static Singleton* GetInstance(const std::string& value);
         static Singleton* getInstance();

        ///**
        //* Static methods should be defined outside the class.
        //*/
        //static engine::Singleton* getInstance()
        //{
        //    /**
        //        * This is a safer way to create an instance. instance = new Singleton is
        //        * dangerous in case two instance threads wants to access at the same time
        //        */
        //        /* if (singleton_ == nullptr) {
        //            singleton_ = new Singleton(value);
        //        }*/
        //    return singleton_;
        //}


        SceneSettings value() const {
            return value_;
        }
    };
}