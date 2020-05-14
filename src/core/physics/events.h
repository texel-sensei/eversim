#pragma once
#include "core/utility/event_delegate.h"
#include <vector>

namespace eversim { namespace core { namespace world {
	struct tile;
}}}

namespace eversim {namespace core { namespace physics {
	struct body;
	struct particle;

	namespace events {

		struct dyn_col {
			particle* a;
			particle* b;
		};

		struct static_col {
			particle* p;
			world::tile const* t;
			glm::vec2 normal;
		};

		using dyn_col_list = std::vector<dyn_col>;
		using static_col_list = std::vector<static_col>;

		using body_body = utility::Event<body*, body*, dyn_col_list const&>;
		using body_level = utility::Event<body*, static_col_list const&>;
	}}}}
