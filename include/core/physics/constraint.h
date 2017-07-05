#pragma once

#include <glm/glm.hpp>
#include <array>

namespace eversim { namespace core { namespace physics {
	
	struct particle;

	enum class constraint_type {
		equality,
		inequality
	};

	template<size_t N>
	class constraint {
	public:
		constexpr static auto arity = N;
		virtual ~constraint(){};

		virtual float operator()() const = 0;
		virtual std::array<glm::vec2,N> grad() const = 0;

		constraint_type get_type() const noexcept { return type; }

		std::array<particle*, N> particles;
		float stiffness = 1.f;
	protected:
		constraint_type type = constraint_type::equality;
	};

}}}
