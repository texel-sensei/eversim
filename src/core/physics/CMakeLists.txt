add_library(physics SHARED
	AABB.h
	body.cpp
	body.h
	body_template.cpp
	body_template.h
	errors.cpp
	errors.h
	events.h
	particle.cpp
	particle.h
	physics_manager.cpp
	physics_manager.h


	constraints/angle_constraint.cpp
	constraints/angle_constraint.h
	constraints/constraint_base.cpp
	constraints/constraint_base.h
	constraints/distance_constraint.cpp
	constraints/distance_constraint.h
	constraints/static_collision_constraint.cpp
	constraints/static_collision_constraint.h
)

target_link_libraries(physics
	PUBLIC
		glm
		utility
		Boost::boost

		core_error
		world
		rendering
		easyloggingpp
)
