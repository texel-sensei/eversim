#pragma once

#include <glm/glm.hpp>
#include <vector>


namespace eversim { namespace core { namespace physics {
	
	struct particle;

	enum class constraint_type {
		equality,
		inequality
	};

	class constraint{
	public:
		explicit constraint(size_t N) : arity(N) {}
		virtual ~constraint(){};

		constraint(constraint const&) = delete;
		constraint& operator=(constraint const&) = delete;

		virtual float operator()() const = 0;
		virtual std::vector<glm::vec2> grad() const = 0;

		constraint_type get_type() const noexcept { return type; }
		size_t get_arity() const noexcept { return arity;}

		std::vector<particle*> particles;
		float stiffness = 1.f;
	protected:
		constraint_type type = constraint_type::equality;
		size_t arity;
	};

}}}
