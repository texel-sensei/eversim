#pragma once

#include "core/system/program.h"

#include <vector>

namespace eversim {
	namespace core {
		namespace system {
			class programsequencer {
			private:
				std::vector<program*> sequence;
			public:
				void push_back(program* prog);
				void run();
			};
		}
	}
}