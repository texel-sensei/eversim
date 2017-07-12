#pragma once

#include "core/utility/plattform.h"

#include <easylogging++.h>
#include <glm/glm.hpp>

namespace eversim {
	namespace core {
		namespace rendering {
			class RenderableEntity {
			public:
				virtual void draw();
			};
		}
	}
}