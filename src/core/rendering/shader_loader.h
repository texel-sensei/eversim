#pragma once

#include "core/utility/resource_manager.h"
#include "core/rendering/attachable_shader.h"
#include "core/rendering/fragment_shader.h"
#include "core/rendering/vertex_shader.h"
#include "core/rendering/geometry_shader.h"

#include <vector>

namespace eversim::core::rendering {

	class fragment_loader final :
		public utility::resource_manager<fragment_loader, std::string, AttachableShader>
	{
		protected:
			ptr_type load_file(std::string const& s) override
			{
				auto shader = std::make_shared<FragmentShader>("default_named_FragmentShader");
				shader->create(s);
				return shader;
			}
	};

	class vertex_loader final :
		public utility::resource_manager<vertex_loader, std::string, AttachableShader>
	{
		protected:
			ptr_type load_file(std::string const& s) override
			{
				auto shader = std::make_shared<VertexShader>("default_named_VertexShader");
				shader->create(s);
				return shader;
			}
	};

	class geometry_loader final :
		public utility::resource_manager<geometry_loader, std::string, AttachableShader>
	{
		protected:
			ptr_type load_file(std::string const& s) override
			{
				auto shader = std::make_shared<GeometryShader>("default_named_GeometryShader");
				shader->create(s);
				return shader;
			}
	};


	class shader_loader {
		private:
			fragment_loader fl;
			vertex_loader vl;
			geometry_loader gl;
		public:
			shader_loader() {}
			explicit shader_loader(const std::vector<std::string> paths)
			{
				for (auto& path : paths)
					add_search_directory(path);
			}

			void add_search_directory(const std::string& dir)
			{
				fl.add_search_directory(dir);
				vl.add_search_directory(dir);
				gl.add_search_directory(dir);
			}

			std::shared_ptr<AttachableShader> load(const std::string& file, GLuint type)
			{
				switch (type) {
					case GL_FRAGMENT_SHADER:
						return fl.load(file);
					case GL_VERTEX_SHADER:
						return vl.load(file);
					case GL_GEOMETRY_SHADER:
						return gl.load(file);
				}
				return nullptr;
			}
	};
}
