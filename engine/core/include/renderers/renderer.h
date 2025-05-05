#pragma once

namespace engine
{
	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		virtual void setup() = 0;
	};
}