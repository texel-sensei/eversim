#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>


namespace eversim { namespace core { namespace physics {
	struct constraint_descriptor;
	struct body;
	struct particle;

	enum class constraint_type {
		equality,
		inequality
	};

	struct body_offset_ptr {
		body const* base;
		size_t offset;

		particle* resolve() const;
		particle* operator->() const;
		particle& operator*() const;

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

		constraint(constraint const&) = delete;
		constraint& operator=(constraint const&) = delete;

		virtual float operator()() const = 0;
		virtual std::vector<glm::vec2> grad() const = 0;

		constraint_type get_type() const noexcept { return type; }
		size_t get_arity() const noexcept { return arity;}

		std::vector<pointer> particles;
		float stiffness = 1.f;
	protected:
		constraint_type type = constraint_type::equality;
		size_t arity;
	};

	class constraint_factory {
	public:
		virtual ~constraint_factory(){}

		using data_container = std::shared_ptr<void>;
		virtual data_container parse(std::istream&) const = 0;
		virtual std::unique_ptr<constraint> build(constraint_descriptor const&, body const*) const = 0;
	};

}}}
