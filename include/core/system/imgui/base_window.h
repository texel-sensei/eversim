#pragma once
#include <imgui/imgui.h>
#include <string>

namespace eversim {namespace core {namespace system {namespace imgui {
	
	enum class display_type {
		window,
		child_window,
		inplace
	};

	class base_window {
	public:
		explicit base_window(std::string const& name, bool visible = false);
		virtual ~base_window(){}

		void draw(display_type type = display_type::window);
	protected:
		ImGuiWindowFlags flags = 0;

		virtual void draw_content() = 0;
	private:
		std::string name;
		bool visible;

		void header(display_type);
		void footer(display_type);
	};

} } } }