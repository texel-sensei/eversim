#pragma once

#include <catch2/catch.hpp>
#include <glm/glm.hpp>
#include <sstream>

class glm_equals : public Catch::MatcherBase<glm::vec2> {
public:
	explicit glm_equals(glm::vec2 v) : expected(v) {}

	bool match(glm::vec2 const& v) const override {
		return v.x == Approx(expected.x) && v.y == Approx(expected.y);
	}

	std::string describe() const override
	{
		std::ostringstream ss;
		ss << "is approximate the same as (" << expected.x << ", " << expected.y << ")";
		return ss.str();
	}
private:
	glm::vec2 expected;
};

inline glm_equals is_approx(glm::vec2 expected)
{
	return glm_equals(expected);
}
inline glm_equals is_approx(float x, float y)
{
	return glm_equals({ x,y });
}

namespace Catch {
	template<typename F>
	struct StringMaker<glm::tvec2<F>> {
		static std::string convert(glm::tvec2<F> const& v)
		{
			using namespace std;
			using namespace std::literals;
			return "("s + to_string(v.x) + ", " + to_string(v.y) + ")";
		}
	};
}
