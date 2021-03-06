#pragma once

#include "core/physics/particle.h"
#include "core/physics/body.h"

#include <glm/glm.hpp>
#include <boost/any.hpp>

#include <vector>
#include <memory>


namespace eversim { namespace core { namespace physics {
	struct constraint_descriptor;
	struct body;

	enum class constraint_type {
		equality,
		inequality
	};

	struct body_offset_ptr {
		body const* base;
		size_t offset;

		body_offset_ptr() = default;
		body_offset_ptr(body const* b, size_t o) : base(b), offset(o){}
		body_offset_ptr(particle const* p);

		particle* resolve() const
		{
			return &base->get_particle(offset);
		}
		particle* operator->() const { return resolve(); }
		particle& operator*() const { return *resolve(); }

		friend bool operator==(body_offset_ptr const& a, particle const* b) {
			return a.resolve() == b;
		}
		friend bool operator==(particle const* a, body_offset_ptr const& b) {
			return b == a;
		}
		friend bool operator!=(body_offset_ptr const& a, particle const* b) {
			return !(a == b);
		}
		friend bool operator!=(particle const* a, body_offset_ptr const& b) {
			return b != a;
		}
	};

	class constraint{
	public:
		using pointer = body_offset_ptr;

		explicit constraint(size_t N) : arity(N) { particles.resize(N); }
		virtual ~constraint(){};

		virtual float operator()() const = 0;
		virtual void grad(utility::array_view<glm::vec2> out) const = 0;

		constraint_type get_type() const noexcept { return type; }
		size_t get_arity() const noexcept { return arity;}

		bool is_alive() const;

		std::vector<pointer> particles;
		float stiffness = 1.f;
	protected:
		constraint_type type = constraint_type::equality;
		size_t arity;

		constraint(constraint const&) = default;
		constraint& operator=(constraint const&) = default;
	};

	class constraint_factory {
	public:
		virtual ~constraint_factory(){}

		using data_container = boost::any;
		virtual data_container parse(std::istream&) const = 0;
		virtual std::unique_ptr<constraint> build(constraint_descriptor const&, body const*) const = 0;
	};

}}}
