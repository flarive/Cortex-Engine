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

        //virtual unsigned int getTypeID() const = 0;
	};

    /// <summary>
    /// CRTP
    /// </summary>
    /// <typeparam name="Derived"></typeparam>
    template<typename Derived>
    class ComponentBase : public Component
    {
    public:
        ComponentBase() = default;
        virtual ~ComponentBase() = default;



        //virtual void init() = 0;
        //virtual void update() = 0;
        //virtual void draw() = 0;


        virtual unsigned int getTypeID() const final {
            return getStaticTypeID();
        }

        static unsigned int getStaticTypeID() {
            static unsigned int typeID = []() {
                static unsigned int counter = 0;
                return counter++;
                }();
            return typeID;
        }
    };
}

