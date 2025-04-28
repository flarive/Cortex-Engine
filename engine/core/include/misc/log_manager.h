#pragma once

#include <string>

namespace engine
{
    class LogManager
    {
    public:

        LogManager();


        static void info(const std::string& msg);


    private:

        void stdout_example();
        void rotating_example();


    };
}