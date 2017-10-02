#include "editor/windows/log_window.h"
#include <easylogging++.h>
#include <boost/format.hpp>

namespace eversim { namespace editor { namespace windows {
	
	using namespace editor::core;

	class log_window::log_handler : public el::LogDispatchCallback {
	public:
		log_window* window = nullptr;
		std::string name;

		void handle(const el::LogDispatchData* handlePtr) override
		{
			window->add_line(*handlePtr->logMessage());
		}
	};

	log_window::log_window(std::string const& name)
		: base_window(name), buffer(4000)
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

		auto formatter = boost::format("%s\t%s");

		for(auto const& line : buffer)
		{
			const auto lvl_str = el::LevelHelper::convertToString(line.level());
			const auto msg = (formatter % lvl_str % line.message()).str();

			if(filter.IsActive() && !filter.PassFilter(msg.c_str()))
			{
				continue;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, level_colors[line.level()]);
			ImGui::TextUnformatted(msg.c_str());
			if(ImGui::IsItemHovered())
			{
				auto info_format = boost::format("%s:%d [%s]");
				const auto info_str = (
					info_format % line.file() % line.line() % line.func()
					).str();
				ImGui::SetTooltip(info_str.c_str());
			}
			ImGui::PopStyleColor();
		}

		if (ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		ScrollToBottom = false;
		ImGui::EndChild();
	}

	void log_window::add_line(el::LogMessage const& line)
	{
		buffer.push_back(line);
		ScrollToBottom = true;
	}
}}}
