#pragma once

#include <glm/glm.hpp>
#include <array>

namespace eversim { namespace core { namespace physics {
	
	struct particle;

	enum class constraint_type {
		equality,
		inequality
	};

	class abstract_constraint {
		template<size_t N>
		friend class constraint;

		abstract_constraint() = default;
	};

	template<size_t N>
	class constraint : public abstract_constraint {
	public:
		virtual ~constraint(){};

		constexpr static size_t arity()
		{
			return N;
		}

		virtual float operator()() const = 0;
		virtual std::array<glm::vec2,N> grad() const = 0;

		constraint_type get_type() const noexcept { return type; }

		std::array<particle*, N> particles;
		float stiffness = 1.f;
	protected:
		constraint_type type = constraint_type::equality;
	};

}}}
