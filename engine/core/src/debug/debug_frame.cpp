#include "../../include/debug/debug_frame.h"

#include "../../include/managers/log_manager.h"

#include <iostream>
#include <sstream>

void engine::DebugFrame::ensureIsCalledOncePerFrame(std::string_view className, std::string_view methodName, std::string_view instanceId)
{
    extern uint64_t globalFrameIndex;

    // reserve + append to avoid multiple temporary allocations
    std::string key;
    key.reserve(className.size() + methodName.size() + instanceId.size() + 3);
    key.append(className);
    key.append("::");
    key.append(methodName);
    key.append(":");
    key.append(instanceId);

    auto it = m_lastFramePerInstance.find(key);

    if (it != m_lastFramePerInstance.end() && it->second == globalFrameIndex)
    {
        logger.warn("{} called twice during frame {}", key, globalFrameIndex);
    }

    m_lastFramePerInstance[key] = globalFrameIndex;
}
