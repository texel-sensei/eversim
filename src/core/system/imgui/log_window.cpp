#include "core/system/imgui/log_window.h"
#include <easylogging++.h>

namespace eversim { namespace core { namespace system { namespace imgui {

		class log_window::log_handler : public el::LogDispatchCallback {
		public:
			log_window* window = nullptr;
			std::string name;

			void handle(const el::LogDispatchData* handlePtr) override{
				const auto builder = handlePtr->logMessage()->logger()->logBuilder();
				const auto append_newline = (handlePtr->dispatchAction() == el::base::DispatchAction::NormalLog);
				window->add_line(builder->build(handlePtr->logMessage(), append_newline));
			}
		};

	log_window::log_window(std::string const& name)
		: base_window(name)
	{
		const auto logger_name = "imgui_" + name;
		el::Helpers::installLogDispatchCallback<log_handler>(logger_name);
		handler = el::Helpers::logDispatchCallback<log_handler>(logger_name);
		handler->window = this;
		handler->name = logger_name;
	}

	log_window::~log_window()
	{
		el::Helpers::uninstallLogDispatchCallback<log_window>(handler->name);
		handler = nullptr;
	}

	bool log_window::has_menu() const
	{
		return false;
	}

	void log_window::draw_menu()
	{
		// nothing yet
	}

	void log_window::begin_window()
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
	}

	void log_window::clear()
	{
		buffer.clear();
		line_offsets.clear();
	}

	void log_window::draw_content()
	{
		if (ImGui::Button("Clear"))
			clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		filter.Draw("Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (copy)
			ImGui::LogToClipboard();

		if (filter.IsActive())
		{
			const char* buf_begin = buffer.begin();
			const char* line = buf_begin;
			for (int line_no = 0; line != nullptr; line_no++)
			{
				const char* line_end = (
					                       line_no < line_offsets.Size)
					                       ? buf_begin + line_offsets[line_no]
					                       : nullptr;
				if (filter.PassFilter(line, line_end))
					ImGui::TextUnformatted(line, line_end);
				line = line_end && line_end[1] ? line_end + 1 : nullptr;
			}
		} else
		{
			ImGui::TextUnformatted(buffer.begin());
		}

		if (ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		ScrollToBottom = false;
		ImGui::EndChild();
	}

	void log_window::add_line(std::string const& line)
	{
		auto old_size = buffer.size();
		buffer.append(line.c_str());
		for (auto new_size = buffer.size(); old_size < new_size; ++old_size)
		{
			if (buffer[old_size] == '\n')
			{
				line_offsets.push_back(old_size);
			}
		}
		ScrollToBottom = true;
	}
}}}}
