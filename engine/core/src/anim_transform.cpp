#include "../include/anim_transform.h"


#include "../include/managers/log_manager.h"
#include "../include/debug/debug_frame.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

engine::AnimTransform::AnimTransform(const Transform& from_, const Transform& to_, AnimMode mode_, float duration_, bool loop_)
    : from(from_), to(to_), mode(mode_), duration(duration_), loop(loop_)
{}

void engine::AnimTransform::setup(const Transform& current,
    const Transform& from_,
    const Transform& to_,
    AnimMode mode_,
    bool loop_)
{
    mode = mode_;
    elapsed = 0.0f;
    loop = loop_;

    if (mode == AnimMode::Absolute)
    {
        from = from_;
        to = to_;
    }
    else // Relative
    {
        // Start from current
        from = current;

        // Convert delta into absolute target
        to.setLocalPosition(current.getLocalPosition() + to_.getLocalPosition());
        to.setLocalRotation(current.getLocalRotation() + to_.getLocalRotation());
        to.setLocalScale(current.getLocalScale() * to_.getLocalScale());
    }
}

//bool engine::AnimTransform::update(float dt, Transform& outTransform)
//{
//    //std::ostringstream oss2;
//    //oss2 << "[AnimTransform::update] dt: " << dt
//    //    << "s, elapsed: " << elapsed
//    //    << "s, duration: " << duration
//    //    << "s, t: " << (elapsed / duration);
//
//    //logger.info("{}", oss2.str());
//
//
//
//    DebugFrame::ensureIsCalledOncePerFrame("AnimTransform", "update");
//    
//    elapsed += dt;
//
//    float t = glm::clamp(elapsed / duration, 0.0f, 1.0f);
//
//    // Interpolate TRS
//    outTransform.setLocalPosition(
//        glm::mix(from.getLocalPosition(), to.getLocalPosition(), t)
//    );
//
//    outTransform.setLocalRotation(
//        glm::mix(from.getLocalRotation(), to.getLocalRotation(), t)
//    );
//
//    outTransform.setLocalScale(
//        glm::mix(from.getLocalScale(), to.getLocalScale(), t)
//    );
//
//    //std::ostringstream oss;
//    //oss << "Frame dt=" << dt << " now=" << timeNow();
//    //logger.info("{}", oss.str());
//
//    return (t >= 1.0f); // finished?
//}


bool engine::AnimTransform::update(float dt, Transform& outTransform)
{
    DebugFrame::ensureIsCalledOncePerFrame("AnimTransform", "update");

    elapsed += dt;

    // Calculate interpolation factor
    float t = elapsed / duration;

    // If looping, wrap t around using modulo
    if (loop)
    {
        t = fmod(t, 1.0f);
    }
    else
    {
        // Clamp to [0, 1] for non-looping animations
        t = glm::clamp(t, 0.0f, 1.0f);
    }

    // Interpolate TRS
    outTransform.setLocalPosition(
        glm::mix(from.getLocalPosition(), to.getLocalPosition(), t)
    );

    outTransform.setLocalRotation(
        glm::mix(from.getLocalRotation(), to.getLocalRotation(), t)
    );

    outTransform.setLocalScale(
        glm::mix(from.getLocalScale(), to.getLocalScale(), t)
    );

    // Return whether the animation is finished (only relevant for non-looping animations)
    return (!loop && t >= 1.0f);
}



std::string engine::AnimTransform::timeNow()
{
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t now_c = system_clock::to_time_t(now);

    std::tm local_tm;
#if defined(_WIN32)
    localtime_s(&local_tm, &now_c);
#else
    localtime_r(&now_c, &local_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%H:%M:%S");

    return oss.str();
}