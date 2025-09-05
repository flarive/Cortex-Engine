#pragma once

namespace engine
{
	class Component
	{
    public:
        Component() = default;
        virtual ~Component() = default;

        virtual void init() = 0;
        virtual void update() = 0;
        virtual void draw() = 0;

        //virtual unsigned int getTypeID() = 0;
	};

    /// <summary>
    /// CRTP
    /// </summary>
    template<typename Derived>
    class ComponentBase : public Component
    {
    public:
        ComponentBase() = default;
        virtual ~ComponentBase() = default;

        virtual unsigned int getTypeID() const final
        {
            return getStaticTypeID();
        }

        static unsigned int getStaticTypeID()
        {
            static unsigned int typeID = []()
                {
                static unsigned int counter = 0;
                return counter++;
                }();
            return typeID;
        }
    };
}

