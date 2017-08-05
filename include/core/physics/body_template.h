#pragma once

#include "constraints/constraint_base.h"
#include "core/utility/resource_manager.h"

#include <glm/glm.hpp>
#include <boost/any.hpp>

#include <vector>
#include <iostream>
#include <unordered_map>
#include <functional>

namespace eversim { namespace core { namespace physics {
	
	struct particle_descriptor {
		glm::vec2 pos;
		float mass = 1.f;

		static particle_descriptor parse(std::string const& str);
	};
	struct constraint_descriptor {
		int arity;
		float stiffness = 1.f;
		std::vector<size_t> particles;
		std::string type;
		constraint_factory const* factory;
		constraint_factory::data_container extra_data;
		
		using factory_getter = std::function<constraint_factory const&(std::string const&)>;
		static constraint_descriptor parse(std::string const& str, factory_getter);
	};

	/*
	 * Class that describes how to construct a physics body.
	 */
	class body_template {
		friend class body_template_loader;
	public:
	//private:
		std::vector<particle_descriptor> particles;
		std::vector<constraint_descriptor> constraints;
	};

	/*
	 * Class to load a body template
	 * Input file should have the format:
	 *	<num_particles>
	 *	<x> <y> <mass> # particle descriptor, num_particles times
	 *	<num_constraints>
	 *	<arity> <particle_id1> ... <id_N> <stiffness> <type> # constraint descriptor, num_constrains times
	 */
	class body_template_loader final
		: public utility::resource_manager<body_template_loader, std::string, body_template>
	{
	public:
		using factory_ptr = std::unique_ptr<constraint_factory>;
		void register_factory(std::string const& type, factory_ptr);

		body_template parse(std::istream& data) const;
		
	protected:
		ptr_type load_file(std::string const& path) override;
	private:
		std::unordered_map<std::string, factory_ptr> constraint_loaders;

		std::string load_line(std::istream&) const;
		void load_particles(body_template&, std::istream&) const;
		void load_constraints(body_template&, std::istream&) const;
	};
}}}
