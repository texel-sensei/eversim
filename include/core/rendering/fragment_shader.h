#pragma once
#include "core\rendering\attachable_shader.h"

namespace eversim {
	namespace core {
		namespace rendering {
			class FragmentShader :
				public AttachableShader
			{
			public:
				FragmentShader(const std::string& name);
				~FragmentShader();

				void create(const std::string& filename) override;
			};
		}
	}
}
