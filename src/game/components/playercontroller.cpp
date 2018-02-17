#include "game\components\playercontroller.h"

#include "core\system\gameobject.h"
#include "core\system\components\physics_component.h"
#include "core\system\game.h"

#include "core\physics\physics_manager.h"
#include "core\physics\body.h"



namespace eversim::game::components {
	using namespace core::system;
	using namespace core::input;
	std::unique_ptr<component> PlayerController::do_clone() const
	{
		return std::make_unique<PlayerController>(
			const_cast<gameobject*>(&(this->get_owner())),*handler_ptr);
	}

	namespace {
		float get_framefactor(const eversim::core::utility::clock::duration &dur)
		{
			using namespace std::literals;
			return float(dur.count()) / 
				std::chrono::duration_cast<eversim::core::utility::clock::duration>(1s).count();
		}
	}

	PlayerController::PlayerController(gameobject * owner, core::input::InputHandler & handler) :
		component(owner), handler_ptr(&handler)
	{
		using namespace eversim::core;
		using namespace core::physics;
		
		delg.connect([&handler](physics::body* ptr,
			physics::events::static_col_list const& list)
		{
			for (const auto& sc : list)
			{
				if (std::abs(sc.normal.y - 1.) < 0.001)
				{
					handler.pop({ std::string("midjumpjumped") , "midjump" });
					break;
				}
			}
		}, owner->get_game().get_physics().level_collision_events());

		handler.get_context("game")->register_function_range(
			"STEER",
			[this,&handler](InputContext& context, glm::vec2 v) {
			//LOG(INFO) << v.x << "/" << v.y;
			double value = v.x;
			if (std::abs(value) - 0.2 < 0.) return;
			double a = (value < 0.) ? -1. : 1.;
			auto pc = get_owner().get_component<physics_component>();
			auto& velocity = pc->get_body().velocity;
			velocity += 3.f*get_framefactor(get_frametime())*glm::vec2(a*std::exp(std::abs(value)), 0);
		}
		);

		handler.get_context("game")->register_function_button(
			"JUMP",
			[this, &handler](InputContext& context)
		{
			auto pc = get_owner().get_component<physics_component>();
			auto& velocity = pc->get_body().velocity;
			velocity += 5.f*glm::vec2(0, 1);
			handler.push_context("midjump");
		}
		);

		handler.get_context("midjump")->register_function_button(
			"DOUBLEJUMP",
			[this, &handler](InputContext& context)
		{
			auto pc = get_owner().get_component<physics_component>();
			auto& velocity = pc->get_body().velocity;
			velocity += 3.5f*glm::vec2(0, 1);
			handler.push_context("midjumpjumped");
		}
		);
	}

}