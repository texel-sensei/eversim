#include "core/physics/physics_manager.h"

namespace eversim { namespace core { namespace physics {
	
	void physics_manager::add_particle(particle const& p)
	{
		particles.push_back(p);
	}

	void physics_manager::integrate(float dt)
	{
		apply_external_forces(dt);
		damp_velocities();
		for(auto& p : particles)
		{
			p.projected_position = p.pos + dt * p.vel;
		}

		int num_iterations = 5;
		for(auto i = 0; i < num_iterations; ++i)
		{
			project_constraints();
		}

		for(auto& p : particles)
		{
			p.vel = (p.projected_position - p.pos) / dt;
			p.pos = p.projected_position;
		}
	}


	void physics_manager::apply_external_forces(float dt)
	{
		for(auto& p : particles)
		{
			p.vel += dt * glm::vec2{ 0,-1 };
		}
	}
	void physics_manager::damp_velocities()
	{
		for(auto& p : particles)
		{
			p.vel *= 0.99; // TODO: improve
		}
	}
	void physics_manager::project_constraints()
	{
		
	}
}}}
