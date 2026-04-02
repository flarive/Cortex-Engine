#include "../../include/debug/debug_frame.h"

#include "../../include/managers/log_manager.h"

#include <iostream>
#include <sstream>

void engine::DebugFrame::ensureIsCalledOncePerFrame(const std::string& className, const std::string& methodName)
{
    extern uint64_t globalFrameIndex;
    static uint64_t lastFrameSeen = UINT64_MAX;
    static int callsThisFrame = 0;

    if (globalFrameIndex != lastFrameSeen)
    {
        // New frame
        if (callsThisFrame > 1)
        {
            std::ostringstream oss;
            oss << className << "::" << methodName << " called " << callsThisFrame << " times in frame " << lastFrameSeen << std::endl;
            logger.warn("{}", oss.str());
        }

        callsThisFrame = 0;
        lastFrameSeen = globalFrameIndex;
    }

    callsThisFrame++;

    //std::ostringstream oss;
    //oss << className << "::" << methodName << " - frame=" << globalFrameIndex << " call#=" << callsThisFrame << std::endl;
    //logger.info("{}", oss.str());
}
