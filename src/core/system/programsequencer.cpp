#include "core/system/programsequencer.h"

namespace eversim {
	namespace core {
		namespace system {
			void programsequencer::push_back(program* prog)
			{
				sequence.push_back(prog);
			}

			void programsequencer::run(){
				for (auto prog : sequence)
				{
					prog->run();
				}
			}
		}
	}
}
