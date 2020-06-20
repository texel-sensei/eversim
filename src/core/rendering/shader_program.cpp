#include "core/rendering/shader_program.h"

#include <GL/glew.h>
#include <easylogging++.h>

#include <exception>
#include <iostream>
#include <map>

using namespace std;


namespace eversim {
	namespace core {
		namespace rendering {

			shader_loader ShaderProgram::loader({ "../resources/shader" , "./resources/shader" });

			ShaderProgram::ShaderProgram(const std::string& name) :
				name(name)
			{
			}


			ShaderProgram::~ShaderProgram()
			{
			}

			void ShaderProgram::readActiveAttributes()
			{
				attributes.clear();
				attributes.resize(0);
				GLint count = 0;
				std::vector<GLchar> nameData(256);
				std::vector<GLenum> properties = { GL_NAME_LENGTH , GL_TYPE, GL_LOCATION };
				std::vector<GLint> values(properties.size());

				glGetProgramInterfaceiv(id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &count);

				for (GLint i = 0; i < count; i++)
				{
					glGetProgramResourceiv(id, GL_PROGRAM_INPUT, i, properties.size(),
						&properties[0], values.size(), nullptr, &values[0]);

					nameData.resize(values[0]);

					glGetProgramResourceName(id, GL_PROGRAM_INPUT, i, nameData.size(), nullptr, &nameData[0]);
					attributes.push_back(
						std::make_tuple(
							values[1], 
							std::string(nameData.data(), nameData.size()-1),
							values[2])
					);
				}

				std::sort(begin(attributes), end(attributes), 
					[]( std::tuple<GLenum, std::string, GLint>& a,
						std::tuple<GLenum, std::string, GLint>& b )
				{
					return std::get<2>(a) < std::get<2>(b);
				});
			}

			void ShaderProgram::readActiveUniforms()
			{
				uniforms.clear();
				uniforms.resize(0);
				GLint count = 0;
				std::vector<GLchar> nameData(256);
				std::vector<GLenum> properties = { GL_NAME_LENGTH , GL_TYPE };
				std::vector<GLint> values(properties.size());

				glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count);
				for (GLint i = 0; i < count; i++)
				{
					glGetProgramResourceiv(id, GL_UNIFORM, i, properties.size(),
						&properties[0], values.size(), nullptr, &values[0]);

					nameData.resize(values[0]);
					GLenum type = values[1];

					glGetProgramResourceName(id, GL_UNIFORM, i, nameData.size(), nullptr, &nameData[0]);
					uniforms.emplace_back(type, std::string(nameData.data(), nameData.size()-1));
				}
			}

			void ShaderProgram::logUniforms() const
			{
				LOG(INFO) << "uniforms of " << name;
				for (const auto& uniform : uniforms)
				{
					LOG(INFO) << uniform.first << " " << uniform.second;
				}
			}

			void ShaderProgram::logAttributes() const
			{
				LOG(INFO) << "attributes of " << name;
				for (const auto& attribute : attributes)
				{
					LOG(INFO) << std::get<0>(attribute) << " " << std::get<1>(attribute);
				}
			}

			void ShaderProgram::attach(const AttachableShader& shader) const
			{
				if (!shader.created)
					throw runtime_error(string("The shader you try to attach is not yet compiled. Tried to attach: " + shader.name).c_str());
				glAttachShader(id, shader.getID());
				//TODO Error
				LOG(INFO) << "attached " << shader.name << " to " << name;
			}

			void ShaderProgram::create()
			{
				id = glCreateProgram();
				LOG(INFO) << "create program " << name << ":" << id;
			}

			void ShaderProgram::link()
			{
				glLinkProgram(id);
				readActiveAttributes();
				readActiveUniforms();
				LOG(INFO) << "link program " << name;
			}

			void ShaderProgram::use() const
			{
				glUseProgram(id);
			}

			GLint ShaderProgram::getUniformLocation(const std::string& uname) const
			{
				return glGetUniformLocation(getID(), uname.c_str());
			}

			void ShaderProgram::attach(std::string file, const GLenum type) const
			{
				attach(*loader.load(file, type));
			}

			void ShaderProgram::attach(const std::vector<std::pair<std::string, GLenum>>& shaders) const
			{
				for (const auto& shader : shaders)
					attach(shader.first, shader.second);
			}

			void ShaderProgram::attach(std::istream& file, const GLenum type) const
			{
				switch (type) {
				case GL_FRAGMENT_SHADER:
				{
					FragmentShader shader("default_named_FragmentShader");
					shader.create(file);
					attach(shader);
				}
				break;
				case GL_VERTEX_SHADER:
				{
					VertexShader shader("default_named_VertexShader");
					shader.create(file);
					attach(shader);
				}
				break;
				case GL_GEOMETRY_SHADER:
				{
					GeometryShader shader("default_named_GeometryShader");
					shader.create(file);
					attach(shader);
				}
				break;
				default:
					throw std::runtime_error("Unknown GLenum when attaching a shader");
					break;
				}
			}

			void ShaderProgram::attach(const std::vector<std::pair<std::istream&, GLenum>>& shaders) const
			{
				for (auto& shader : shaders)
					attach(shader.first, shader.second);
			}
		}
	}
}
