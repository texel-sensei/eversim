#pragma once

#include "core\rendering\attachable_shader.h"
#include "core\rendering\vertex_shader.h"
#include "core\rendering\fragment_shader.h"
#include "core\rendering\geometry_shader.h"

#include <string>
#include <vector>
#include <utility>
#include <tuple>

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
				void attach(const std::string& filename,const GLenum TYPE) const;
				void attach(const std::vector<
					std::pair<std::string,GLenum>
							>& shaders) const;
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