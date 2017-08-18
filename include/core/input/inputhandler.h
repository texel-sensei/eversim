#pragma once

#include "core/input/inputcontext.h"

#include <SDL2/SDL.h>

#include <map>
#include <string>
#include <list>
#include <memory>


namespace eversim {
	namespace core {
		namespace input {

			class InputHandler
			{
			private:
				std::map<std::string,std::shared_ptr<InputContext>> available_contexts;
				std::list<std::weak_ptr<InputContext>> context_stack;

			public:
				explicit InputHandler(const std::string& filename);

				void push_context(const std::string& context_name);
				std::string pop();

				std::shared_ptr<InputContext> get_context(const std::string& context_name) const
				{
					return available_contexts.at(context_name);
				}

				const std::map<std::string, std::shared_ptr<InputContext>>& get_available_contexts() const;

				void handle_event(SDL_Event& sdl_event);

				void execute();
			};

		}
	}
}
