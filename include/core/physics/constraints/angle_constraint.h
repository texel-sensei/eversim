#pragma once

#include "core/physics/constraints/constraint_base.h"
#include "core/physics/particle.h"
#include <array>

namespace eversim { namespace core { namespace physics {

	class angle_constraint : public constraint {
		friend class angle_constraint_factory;
	public:

		explicit angle_constraint(float target_angle)
			: constraint(3), target_angle(target_angle)
		{
			type = constraint_type::equality;
		}

		float operator()() const override
		{
			const auto v1 = normalize(particles[0]->projected_position - particles[1]->projected_position);
			const auto v2 = normalize(particles[2]->projected_position - particles[1]->projected_position);

			const auto angle = acosf(dot(v1, v2));
			return angle - target_angle;
		}

		void grad(utility::array_view<glm::vec2> out) const override
		{
			EVERSIM_ASSERT(out.size() == get_arity());

			const auto a = particles[0]->projected_position;
			const auto b = particles[1]->projected_position;
			const auto c = particles[2]->projected_position;

			const auto grad_acos = [](float x) {return -1.f / sqrtf(1 - x*x); };

			const auto grad_endpoint = [b](glm::vec2 const& x, glm::vec2 const& other)
			{
				const auto ab = length(x - b);
				const auto cb = length(other - b);
				const auto uA = 1.f / length(x - b);
				const auto vA = (x.x - b.x) * (other.x - b.x) / cb + (x.y - b.y)*(other.y - b.y) / cb;
				const auto grad_A_v = (other - b) / cb;
				const auto grad_A_u = -(x - b) / (powf(ab, 3.f));
				const auto grad_A_dot = grad_A_u * vA + uA * grad_A_v;

				return grad_A_dot;
			};
			
			const auto bU = dot(a, c) + dot(-a, b) + dot(-c, b) + dot(b, b);
			const auto grad_B_U = -a - c + 2.f * b;

			const auto bV = length(a - b) * length(c - b);
			const auto grad_B_V = -2.f*(a - b)*length(c - b) - 2.f*(c - b)*length(a - b);

			const auto grad_B_dot = (grad_B_U*bV - bU*grad_B_V) / powf(bV, 2.f);

			const auto v1 = normalize(a - b);
			const auto v2 = normalize(c - b);

			const auto cos_angle = dot(v1, v2);
			const auto k = grad_acos(cos_angle);
			// acosf(dot(v1,v2)) => grad_acos(dot(..)) * grad(dot(..))

			out[0] = k*grad_endpoint(a, c);
			out[1] = k*grad_B_dot;
			out[2] = k*grad_endpoint(c, a);
		}

	private:
		float target_angle;
	};

	class angle_constraint_factory : public constraint_factory {
	public:
		data_container parse(std::istream&) const override;
		std::unique_ptr<constraint> build(constraint_descriptor const&, body const*) const override;

	};

}}}
