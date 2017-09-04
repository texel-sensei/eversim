#include "core\rendering\fragment_shader.h"

using namespace std;

namespace eversim {
	namespace core {
		namespace rendering {
			FragmentShader::FragmentShader(const std::string& name) : AttachableShader(name)
			{
				id = glCreateShader(GL_FRAGMENT_SHADER);
			}


			FragmentShader::~FragmentShader()
			{
			}

			void FragmentShader::create(const std::string& filename)
			{
				compile(filename);

				created = true;
			}

			void FragmentShader::create(std::istream& file)
			{
				compile(file);

				created = true;
			}
		}
	}
}