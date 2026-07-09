#pragma once

#include <string>
#include <functional>
#include <any>

namespace engine
{
    enum class UIEventType
    {
        ButtonClicked,
        EntitySelectionChanged,
        SceneSettingChanged,
        WindowClosed,
        Custom
    };

    struct UIEvent final
    {
        UIEventType type;
        std::string sender;     // window/widget name
        std::any payload;    // button name, value, etc.
    };

    // event bus
    using EventCallback = std::function<void(const UIEvent&)>;
}