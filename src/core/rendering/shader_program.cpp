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
			ShaderProgram::ShaderProgram(const std::string& name) :
				name(name)
			{
			}


			ShaderProgram::~ShaderProgram()
			{
			}

			void ShaderProgram::attach(const AttachableShader& shader)
			{
				if (!shader.created)
					throw exception(string("The shader you try to attach is not yet compiled. Tried to attach: " + shader.name).c_str());
				glAttachShader(id, shader.getID());
				//TODO Error
				LOG(INFO) << "attached " << shader.name << " to " << name;
			}

			void ShaderProgram::attach(const std::string& filename, const GLenum TYPE)
			{
				switch (TYPE) {
				case GL_FRAGMENT_SHADER:
				{
					FragmentShader shader("default_named_FragmentShader");
					shader.create(filename);
					attach(shader); 
				}
				break;
				case GL_VERTEX_SHADER:
				{
					VertexShader shader("default_named_VertexShader");
					shader.create(filename);
					attach(shader); 
				}
				break;
				case GL_GEOMETRY_SHADER:
				{
					GeometryShader shader("default_named_GeometryShader");
					shader.create(filename);
					attach(shader); 
				}
				break;
				default:
					throw exception("Unknown GLenum when attaching a shader");
					break;
				}
			}
			void ShaderProgram::attach(const std::vector<
				std::pair<std::string, GLenum>
			>& shaders)
			{
				for (const auto& shader : shaders)
					attach(shader.first, shader.second);
			}

			void ShaderProgram::create()
			{
				id = glCreateProgram();
				LOG(INFO) << "create program " << name << ":" << id;
			}

			void ShaderProgram::link()
			{
				glLinkProgram(id);
				LOG(INFO) << "link program " << name;
			}

			void ShaderProgram::use() const
			{
				glUseProgram(id);
				//TODO error
			}
		}
	}
}