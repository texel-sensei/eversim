#pragma once
#include "editor/core/base_window.h"

namespace eversim { namespace editor { namespace windows {

	class log_window : public core::base_window {
	public:
		explicit log_window(std::string const& name = "log");
		~log_window();

		log_window(log_window const&) = delete;
		log_window* operator=(log_window const&) = delete;

		bool has_menu() const override final;
		void draw_menu() override final;
	protected:
		void begin_window() override final;
		void clear();
		void draw_content() override final;

	private:
		ImGuiTextBuffer buffer;
		ImGuiTextFilter filter;
		ImVector<int> line_offsets;
		bool ScrollToBottom = true;

		class log_handler;
		log_handler* handler;
		void add_line(std::string const& line);
	};

}}}
