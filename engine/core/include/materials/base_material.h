#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"



namespace engine
{
	/// <summary>
	/// Abstract class for renderers
	/// </summary>
	class BaseMaterial : public NonCopyable
	{
	public:




		BaseMaterial();
		virtual ~BaseMaterial() = default;


	
	};
}