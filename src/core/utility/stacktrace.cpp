#include "core/utility/stacktrace.h"

#if defined(_WIN32)
#	include <windows.h>
#	include <processthreadsapi.h>
#	include <DbgHelp.h>
#endif

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


#if defined(_WIN32)
	//The code requires you to link against the DbgHelp.lib library

	void fill_stacktrace(stacktrace* trace) {
		const auto process = GetCurrentProcess();
		// Set up the symbol options so that we can gather information from the current
		// executable's PDB files, as well as the Microsoft symbol servers.  We also want
		// to undecorate the symbol names we're returned.  If you want, you can add other
		// symbol servers or paths via a semi-colon separated list in SymInitialized.
		::SymSetOptions(
			SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME
			| SYMOPT_LOAD_LINES
		);

		if (!SymInitialize(process, "http://msdl.microsoft.com/download/symbols", TRUE)) return;

		// Capture up to 25 stack frames from the current call stack.  We're going to
		// skip the first stack frame returned because that's the GetStackWalk function
		// itself, which we don't care about.
		const auto num_addrs = stacktrace::MAX_STACKFRAMES;
		void* addrs[num_addrs] = { nullptr };
		const auto frames = CaptureStackBackTrace(0, num_addrs - 1, addrs, nullptr);

		for (auto i = 1u; i < frames; i++) {
			// Allocate a buffer large enough to hold the symbol information on the stack and get 
			// a pointer to the buffer.  We also have to set the size of the symbol structure itself
			// and the number of bytes reserved for the name.
			constexpr auto buffer_size = (sizeof(SYMBOL_INFO) + 1024 + sizeof(ULONG64) - 1) / sizeof(ULONG64);
			ULONG64 buffer[buffer_size] = { 0 };
			auto *info = reinterpret_cast<SYMBOL_INFO *>(buffer);
			info->SizeOfStruct = sizeof(SYMBOL_INFO);
			info->MaxNameLen = 1024;

			auto addr = reinterpret_cast<DWORD64>(addrs[i]);

			// Attempt to get information about the symbol and add it to our output parameter.
			DWORD64 displacement = 0;
			const bool got_name = SymFromAddr(
				process, addr,
				&displacement, info
			);
			
			IMAGEHLP_LINE64 line;
			line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

			DWORD line_displacement = 0;
			const bool got_line = SymGetLineFromAddr64(process, addr, &line_displacement, &line);
			const auto func_name = (got_name) ? info->Name : "(?)";
			const auto file_name = (got_line) ? line.FileName : nullptr;
			const auto line_nr = (got_line) ? line.LineNumber : -1;

			if(got_name || got_line)
			{
				trace->add_frame(func_name, file_name, line_nr);
			}
		}

		SymCleanup(process);

	}

#else
	void fill_stacktrace(stacktrace* trace) {}
#endif
}
