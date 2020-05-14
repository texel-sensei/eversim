#pragma once

#include <string_view>

namespace eversim::core::utility
{

	/*
	 * This struct contains informations about a single stackframe. Because instances of this struct
	 * may be initialized from a signal handler, only a very restricted set of functions can be used
	 * without causing undefined behaviour. Since we use C++17, this restriction is a bit more relaxed,
	 * but dynamic allocation is still not allowed. Because of this we store strings as char[] and not
	 * as a std::string
	 */
	struct frame_information
	{
		static constexpr int STRING_LENGTH = 256;

		frame_information(char const* func_name, char const* file_name = nullptr, int line = -1);

		std::string_view get_name() const { return name; }
		std::string_view get_simple_filename() const;
		std::string_view get_file() const { return file; }
		int get_line() const { return line; }
	private:
		char name[STRING_LENGTH];
		char file[STRING_LENGTH];
		int line = 0;

		void initialize(char const* func_name, char const* file_name, int line);
	};

	class stacktrace
	{
	public:
		static constexpr int MAX_STACKFRAMES = 75;

		/*
		 * Adds information about a new stackframe to this stacktrace. If the internal buffer
		 * if full, then this stacktrace is marked as partial and no further action is done.
		 * (There is no error logging, because this function should be able to run in a signal
		 * handler)
		 */
		void add_frame(char const* func_name, char const* file_name = nullptr, int line = -1);

		frame_information const* begin() const;
		frame_information const* end() const;

		bool is_partial() const { return partial; }
	private:
		alignas(frame_information) std::byte frames_memory[sizeof(frame_information) * MAX_STACKFRAMES];
		int num_stackframes = 0;
		bool partial = false;
	};

	/*
	 * Fills the passed stacktrace with the current status of the stack. This does NOT
	 * include the fill_stacktrace function.
	 */
	void fill_stacktrace(stacktrace* trace);

}