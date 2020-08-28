#include "core/utility/stacktrace.h"
#include <boost/stacktrace/stacktrace_fwd.hpp>

#include <boost/stacktrace.hpp>

#include <csignal>
#include <string>

#include <easylogging++.h>

namespace
{
	void signal_strncpy(char* dst, char const* src, size_t n)
	{
		auto i = 0u;
		for(; i < (n-1) && src[i]; ++i)
		{
			dst[i] = src[i];
		}
		dst[i] = '\0';
	}
}

namespace eversim::core::utility
{
	frame_information::frame_information(char const* func_name, char const* file_name, int line)
	{
		initialize(func_name, file_name, line);
	}

	std::string_view frame_information::get_simple_filename() const
	{
		auto txt = get_file();
		const auto idx = txt.find_last_of("./\\");
		if(idx == std::string_view::npos)
		{
			return txt;
		}

		// no '.', so we assume that we have a system header
		if(txt[idx] == '/' || txt[idx] == '\\')
		{
			return txt.substr(idx+1);
		}
		const auto level = 2;
		auto position = idx;
		for(int i = 0; i < level; ++i)
		{
			auto second_level = txt.rfind('/', position - 1);
			if (second_level == std::string_view::npos)
			{
				second_level = txt.rfind('\\', position - 1);
			}
			if (second_level == std::string_view::npos)
			{
				break;
			}
			position = second_level;
		}
		
		return txt.substr(position + 1);
	}

	void frame_information::initialize(char const* func_name, char const* file_name, int line)
	{
		signal_strncpy(name, func_name, STRING_LENGTH);
		if (file_name) {
			signal_strncpy(file, file_name, STRING_LENGTH);
		}
		else
		{
			file[0] = '\0';
		}
		this->line = line;
	}

	void stacktrace::add_frame(char const* func_name, char const* file_name, int line)
	{
		if (num_stackframes >= MAX_STACKFRAMES)
		{
			partial = true;
			return;
		}
		auto* base = reinterpret_cast<frame_information*>(frames_memory);
		new (base + num_stackframes) frame_information(func_name, file_name, line);
		++num_stackframes;
	}

	frame_information const* stacktrace::begin() const
	{
		if (num_stackframes == 0) return nullptr;
		return reinterpret_cast<frame_information const*>(frames_memory);
	}

	frame_information const* stacktrace::end() const
	{
		return begin() + num_stackframes;
	}

	void fill_stacktrace(stacktrace* trace) {
		auto current = boost::stacktrace::stacktrace();

		for(auto&& frame : current) {
			trace->add_frame(frame.name().c_str(), frame.source_file().c_str(), frame.source_line());
		}
	}
}
