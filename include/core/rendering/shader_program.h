#pragma once

#include "core\rendering\attachable_shader.h"
#include "core\rendering\vertex_shader.h"
#include "core\rendering\fragment_shader.h"
#include "core\rendering\geometry_shader.h"

#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <istream>

namespace eversim {
	namespace core {
		namespace rendering {
			class ShaderProgram
			{
			private:
				GLuint id;
				
				std::vector<std::tuple<GLenum,std::string,GLint>> attributes;
				std::vector<std::pair<GLenum,std::string>> uniforms;
				void readActiveUniforms();
				void readActiveAttributes();

			public:
				const std::string name;
				

				explicit ShaderProgram(const std::string& name);
				~ShaderProgram();

				void attach(const AttachableShader& shader) const;

				template<typename T>
				void attach(T file, const GLenum Type) const
				{
					switch (Type) {
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
						throw std::exception("Unknown GLenum when attaching a shader");
						break;
					}
				}
				template<typename T>
				void attach(const std::vector<std::pair<T, GLenum>>& shaders) const
				{
					for (const auto& shader : shaders)
						attach<T>(shader.first, shader.second);
				}

				void create();
				void link();
				void use() const;
				GLuint getID() const { return id; }
				const std::vector<std::pair<GLenum, std::string>>& getActiveUniforms() const
				{
					return uniforms; 
				}
				const std::vector<std::tuple<GLenum, std::string, GLint>>& getActiveAttributes() const
				{
					return attributes;
				}
				void logUniforms() const;
				void logAttributes() const;
				void logUnfiformslogAttributes() const { logUniforms(); logAttributes(); }

				GLint getUniformLocation(const std::string& uname) const;

			};
		}
	}
}