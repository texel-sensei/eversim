#pragma once
#include <imgui/imgui.h>
#include <string>

namespace eversim {namespace editor {namespace core {
	
	enum class display_type {
		window,
		child_window,
		inplace
	};

	class base_window {
	public:
		explicit base_window(std::string const& name, bool visible = false);
		virtual ~base_window(){}

		std::string const& get_name() const { return name; }
		bool is_visible() const noexcept { return visible; }
		void set_visible(bool b) noexcept { visible = b; }

		virtual bool has_menu() const { return false; }
		virtual void draw_menu() {}

		void draw(display_type type = display_type::window);
	protected:
		float alpha = -1;
		ImGuiWindowFlags flags = 0;

		virtual void begin_window() {}
		virtual void draw_content() = 0;
	private:
		std::string name;
		bool visible;

		bool header(display_type);
		void footer(display_type);
	};

} } } 