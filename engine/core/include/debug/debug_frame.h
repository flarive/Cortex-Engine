#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <cstdint>


namespace engine
{
	class DebugFrame final
	{
	public:
		static void ensureIsCalledOncePerFrame(std::string_view className, std::string_view methodName, std::string_view instanceId);

	private:
		inline static std::unordered_map<std::string, uint64_t> m_lastFramePerInstance;
	};
}