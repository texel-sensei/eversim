#include "core/input/inputhandler.h"

#include "core/input/contextloader.h"


#include "core/input/inputevent.h"

#include <easylogging++.h>

#include <istream>
#include <sstream>

using namespace std;

namespace eversim {
	namespace core {
		namespace input {

			InputHandler::InputHandler(const string& filename) : InputHandler(std::ifstream(filename))
			{}

			InputHandler::InputHandler(std::ifstream& file)
			{
				string content((std::istreambuf_iterator<char>(file)),
					(std::istreambuf_iterator<char>()));

				istringstream iss(content);

				*this = InputHandler(iss);
			}

			InputHandler::InputHandler(std::istringstream& file)
			{
				auto contexts = InputContextLoader::generate_contexts_from_json(file);

				for (const auto& context : contexts)
				{
					available_contexts[context.get_name()] = make_shared<InputContext>(context);
				}

				for (const auto& context : available_contexts)
				{
					context.second->list_actions();
				}
			}

			void InputHandler::push_context(const std::string& context_name)
			{
				context_stack.push_front(available_contexts.at(context_name));
			}

			std::string InputHandler::pop()
			{
				string res = context_stack.begin()->lock()->get_name();
				context_stack.pop_front();
				return res;
			}

			const std::map<std::string, std::shared_ptr<InputContext>>& InputHandler::get_available_contexts() const
			{
				return available_contexts;
			}

			void InputHandler::execute()
			{
				for (auto& context_ptr : context_stack)
				{
					if (context_ptr.expired())
					{
						LOG(ERROR) << "invalid context ptr";
						continue;
					}

					context_ptr.lock()->execute();
				}
			}

			void InputHandler::handle_event(SDL_Event& sdl_event)
			{
				auto events = InputEvent::map_event(sdl_event);

				for (auto& event : events) {
					handle_event(event);
				}
			}

			void InputHandler::handle_event(InputEvent& event)
			{
				//LOG(INFO) << event;
				if (event.get_event_type() == +RawInputConstants::event_type::INVALID) return;

				for (auto& context_ptr : context_stack)
				{
					if (context_ptr.expired())
					{
						LOG(ERROR) << "invalid context ptr";
						continue;
					}
					auto& context = *context_ptr.lock();

					if (context.handle_event(event))
						break;

				}
			}

		}
	}
}