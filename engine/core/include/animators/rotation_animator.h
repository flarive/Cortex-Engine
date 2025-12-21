#pragma once

#include "animator.h"

namespace engine
{
	class RotationAnimator final : public Animator
	{
	public:
		RotationAnimator() = default;
		~RotationAnimator() = default;

		virtual AnimatorType getTypeID() const
		{
			return AnimatorType::rotation;
		}
	};
}