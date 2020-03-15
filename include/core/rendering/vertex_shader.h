#pragma once
#include "core/rendering/attachable_shader.h"

namespace eversim {
	namespace core {
		namespace rendering {
			class VertexShader :
				public AttachableShader
			{
			private:


			public:
				VertexShader(const std::string& name);
				~VertexShader();

				void create(const std::string& filename) override;
				void create(std::istream& file) override;
			};
		}
	}
}
