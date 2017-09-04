#pragma once
#include "core\rendering\attachable_shader.h"

namespace eversim {
	namespace core {
		namespace rendering {
			class GeometryShader :
				public AttachableShader
			{
			private:


			public:
				GeometryShader(const std::string& name);
				~GeometryShader();

				void create(const std::string& filename) override;
				void create(std::istream& file) override;
			};
		}
	}
}