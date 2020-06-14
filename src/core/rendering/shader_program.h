#pragma once

#include "core/rendering/attachable_shader.h"
#include "core/rendering/vertex_shader.h"
#include "core/rendering/fragment_shader.h"
#include "core/rendering/geometry_shader.h"

#include "core/rendering/shader_loader.h"

#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <istream>

namespace eversim::core::rendering {
	class ShaderProgram
	{
		private:
			GLuint id;

			static shader_loader loader;

			std::vector<std::tuple<GLenum,std::string,GLint>> attributes;
			std::vector<std::pair<GLenum,std::string>> uniforms;
			void readActiveUniforms();
			void readActiveAttributes();

		public:
			const std::string name;


			explicit ShaderProgram(const std::string& name);
			~ShaderProgram();

			void attach(const AttachableShader& shader) const;
			void attach(std::string file, const GLenum type) const;
			void attach(const std::vector<std::pair<std::string, GLenum>>& shaders) const;
			void attach(std::istream& file, const GLenum type) const;
			void attach(const std::vector<std::pair<std::istream&, GLenum>>& shaders) const;

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
