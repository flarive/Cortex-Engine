#pragma once

#include <memory>

namespace engine
{
    class App;

    class AppManager final
    {
    public:

        AppManager() = default;
        ~AppManager();


        /// <summary>
        /// Create a new app
        /// </summary>
        template <typename T, typename... Args>
        std::shared_ptr<T> createApp(Args&&... args)
        {
            static_assert(std::is_base_of_v<App, T>);
            m_app = std::make_shared<T>(std::forward<Args>(args)...);
            return std::static_pointer_cast<T>(m_app); // Explicitly cast to std::shared_ptr<T>
        }


        App& getApp()
        {
            return *m_app;
        }

        const App& getApp() const
        {
            return *m_app;
        }

        const std::shared_ptr<App>& getAppPtr() const
        {
            return m_app;
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
        std::shared_ptr<App> m_app{};
    };
}