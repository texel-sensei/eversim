#pragma once
#include <SDL2/SDL.h>
#include <initializer_list>
#include <memory>
#include <stdexcept>

namespace eversim { namespace core { namespace sdl {
	namespace detail {
		inline void cleanup(SDL_Window* w) {
			if(!w) return;
			SDL_DestroyWindow(w);
		}

		inline void cleanup(SDL_Renderer* w) {
			if(!w) return;
			SDL_DestroyRenderer(w);
		}
		inline void cleanup(SDL_GLContext w) {
			if(!w) return;
			SDL_GL_DeleteContext(w);
		}
	} /* detail */

	template<typename... Args>
	void cleanup(Args&&... args) {
		static_assert(sizeof...(args) >= 1, "Can't call cleanup without arguments!");
		std::initializer_list<int>{
			(detail::cleanup(std::forward<Args>(args)),0)...
		};
	}


	namespace detail {
		template<typename SDL_OBJ>
		class SDL_Ptr{
		public:
			SDL_Ptr() = default;
			SDL_Ptr(SDL_OBJ* p) : ptr(p){}

			operator SDL_OBJ*(){
				return ptr.get();	
			}

			operator SDL_OBJ const*() const{
				return ptr.get();	
			}
			operator bool() const {
				return ptr;
			}

		private:
			struct deleter{
				void operator()(SDL_OBJ* o) {
					cleanup(o);
				}
			};
			
			std::unique_ptr<SDL_OBJ, deleter> ptr;
		};
	} /* detail */

	struct sdl_error : public std::runtime_error {
		using runtime_error::runtime_error;
	};

	using window_ptr = detail::SDL_Ptr<SDL_Window>;
	inline window_ptr make_window(
			char const* title,
			int x,
			int y,
			int w,
			int h,
			uint32_t flags
	) {
		auto wi = SDL_CreateWindow(title, x, y, w, h, flags);
		auto o =  window_ptr{wi};
		if(!o) {
			throw sdl_error("Failed to create window!");
		}
		return o;
	}

	using renderer_ptr = detail::SDL_Ptr<SDL_Renderer>;
	inline renderer_ptr make_renderer(SDL_Window* w, int device, uint32_t flags)
	{
		auto o = renderer_ptr{SDL_CreateRenderer(w, device, flags)};
		if(!o) {
			throw sdl_error("Failed to create renderer!");
		}
		return o;
	}

	using context_ptr = detail::SDL_Ptr<void>; // SDL_GLContext is a typedef to void*
	inline context_ptr make_context(SDL_Window* w)
	{
		auto o = context_ptr{ SDL_GL_CreateContext(w) };
		if(!o) {
			throw sdl_error{ "Failed to create context!" };
		}
		return o;
	}

} /* sdl */ } /* core */ } /* eversim */
