#pragma once

#include <memory>

namespace engine
{
    class App;

    class AppManager
    {
    public:
        template <typename T, typename... Args>
        T& createApp(Args&&... args)
        {
            static_assert(std::is_base_of_v<App, T>);
            m_app = std::make_unique<T>(std::forward<Args>(args)...);
            return static_cast<T&>(*m_app);
        }

        App& getApp()
        {
            return *m_app;
        }

        const App& getApp() const
        {
            return *m_app;
        }

        bool hasApp() const
        {
            return m_app != nullptr;
        }

        void shutdown()
        {
            m_app.reset();
        }

    private:
        std::unique_ptr<App> m_app{};
    };
}