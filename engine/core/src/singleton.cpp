#include "../include/singleton.h"

/**
 * Static methods should be defined outside the class.
 */
engine::Singleton* engine::Singleton::getInstance()
{
    /**
     * This is a safer way to create an instance. instance = new Singleton is
     * dangerous in case two instance threads wants to access at the same time
     */
     /* if (singleton_ == nullptr) {
          singleton_ = new Singleton(value);
      }*/
    return singleton_;
}