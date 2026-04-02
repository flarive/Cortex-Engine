#pragma once

#include <string>

namespace engine
{
	class DebugFrame final
	{
	public:
		static void ensureIsCalledOncePerFrame(const std::string& className, const std::string& methodName);
	};
}

