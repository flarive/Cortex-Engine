#pragma once


#include "component.h"

#include "../model.h"

namespace engine
{
	class ModelComponent : public ComponentBase<ModelComponent>
	{
	public:

		ModelComponent() = default;
		ModelComponent(std::shared_ptr<Model> model);
		~ModelComponent() = default;

		void init() override;
		void update() override;
		void draw() override;

		std::shared_ptr<Model> getModel()
		{
			return m_model;
		}


		static unsigned int getStaticTypeID() {
			return 5;
		}

	private:

		std::shared_ptr<Model> m_model{};

	};
}