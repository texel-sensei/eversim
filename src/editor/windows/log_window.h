#pragma once
#include "editor/core/base_window.h"
#include <boost/circular_buffer.hpp>
#include <easylogging++.h>

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
		boost::circular_buffer<el::LogMessage> buffer;
		ImGuiTextFilter filter;
		bool ScrollToBottom = true;
		std::map<el::Level, ImVec4> level_colors = {
			{ el::Level::Error,   { 1,0,0,1}},
			{ el::Level::Warning, { 1,1,0,1 } },
			{ el::Level::Debug,   { 1,1,1,1 } },
			{ el::Level::Info,    { 1,.6f,0,1 } },
			{ el::Level::Trace,   { .6f,.6f,.6f,1 } },
		};

		class log_handler;
		log_handler* handler;
		void add_line(el::LogMessage const&);
	};

}}}
