#pragma once
#include "core/utility/resource_manager.h"
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

namespace eversim { namespace core { namespace physics {
	
	struct particle_descriptor {
		glm::vec2 pos;

		static particle_descriptor parse(std::string const& str);
	};
	struct constraint_descriptor {
		int arity;
		float stiffness;
		std::vector<size_t> particles;
		std::string type;

		static constraint_descriptor parse(std::string const& str);
	};

	/*
	 * Class that describes how to construct a physics body.
	 */
	class body_template {
		friend class body_template_loader;
	public:
	private:
		std::vector<particle_descriptor> particles;
		std::vector<constraint_descriptor> constraints;
	};

	/*
	 * Class to load a body template
	 * Input file should have the format:
	 *	<num_particles>
	 *	<x> <y> <weight> # particle descriptor, num_particles times
	 *	<num_constraints>
	 *	<arity> <particle_id1> ... <id_N> <stiffness> <type> # constraint descriptor, num_constrains times
	 */
	class body_template_loader 
		: public utility::resource_manager<body_template_loader, std::string, body_template>
	{
	public:
		body_template parse(std::istream& data) const;
	
		std::shared_ptr<value_type> load_file(std::string const& path) const;
	private:
		std::string load_line(std::istream&) const;
		void load_particles(body_template&, std::istream&) const;
		void load_constraints(body_template&, std::istream&) const;
	};
}}}