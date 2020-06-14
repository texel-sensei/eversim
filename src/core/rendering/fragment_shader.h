#pragma once
#include "core/rendering/attachable_shader.h"

namespace eversim::core::rendering {
	class FragmentShader :
		public AttachableShader
	{
		public:
			explicit FragmentShader(const std::string& name);
			~FragmentShader();

			void create(const std::string& filename) override;
			void create(std::istream& file) override;
	};
}
