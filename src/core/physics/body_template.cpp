#include "core/physics/body_template.h"
#include <fstream>
#include <sstream>

using namespace std;

namespace eversim { namespace core { namespace physics {

	void body_template_loader::register_factory(std::string const& type, factory_ptr factory)
	{
		constraint_loaders[type] = move(factory);
	}

	body_template body_template_loader::parse(std::istream& data) const
	{
		data.exceptions(istream::badbit | istream::failbit);
		body_template templ;

		load_particles(templ, data);
		load_constraints(templ, data);

		return templ;
	}

	shared_ptr<body_template_loader::value_type> body_template_loader::load_file(std::string const& path) const
	{
		auto data = ifstream(path);
		return make_shared<body_template>(parse(data));
	}

	void body_template_loader::load_particles(body_template& templ, std::istream& data) const
	{
		auto in = istringstream(load_line(data));
		int num;
		in >> num;
		if(num <= 0)
		{
			throw runtime_error{"Number of particles must be positive!"};
		}

		templ.particles.resize(num);
		for(int i = 0; i < num;++i)
		{
			auto line = load_line(data);
			templ.particles[i] = particle_descriptor::parse(line);
		}
	}

	void body_template_loader::load_constraints(body_template& templ, std::istream& data) const
	{
		auto in = istringstream(load_line(data));
		int num;
		in >> num;
		if (num < 0)
		{
			throw runtime_error{ "Number of constraints must be positive!" };
		}

		templ.constraints.resize(num);
		for (int i = 0; i < num; ++i)
		{
			auto line = load_line(data);

			auto c = constraint_descriptor::parse(line,
				[this](string const& type) -> constraint_factory const&
			{
				return *constraint_loaders.at(type);
			});

			for(auto index : c.particles)
			{
				if(index >= templ.particles.size())
				{
					throw runtime_error{"Invalid particle index in constraint!"};
				}
			}
			templ.constraints[i] = move(c);
		}
	}

	string body_template_loader::load_line(istream& in) const
	{
		string line;
		
		while(true){
			in >> ws;
			getline(in, line);
			if(line.empty() || line[0] == '#')
			{
				continue;
			}
			return line;
		}
	}

	namespace {
		std::string get_remainder(istream& data)
		{
			if (data) {
				data.exceptions(istream::badbit);
				data >> ws;
				string remainder;
				getline(data, remainder);

				auto pos = remainder.find('#');
				if(pos != string::npos)
					remainder = remainder.erase(pos);
				remainder.erase(remainder.find_last_not_of(" \t") + 1);
				return remainder;
			}
			return "";
		}
	}

	particle_descriptor particle_descriptor::parse(std::string const& str)
	{
		auto desc = particle_descriptor{};
		auto data = istringstream(str);
		data.exceptions(istream::badbit | istream::failbit);

		data >> desc.pos.x >> desc.pos.y >> desc.mass;
		auto rem = get_remainder(data);
		if(!rem.empty())
		{
			throw runtime_error{"Too much data while parsing particle! " + rem};
		}
		return desc;
	}

	constraint_descriptor constraint_descriptor::parse(
		string const& str, factory_getter get_factory
	){
		auto desc = constraint_descriptor{};
		auto data = istringstream(str);
		data.exceptions(istream::badbit | istream::failbit);
		
		data >> desc.arity;
		desc.particles.resize(desc.arity);
		for(int i = 0; i < desc.arity; ++i)
		{
			data >> desc.particles[i];
		}
		data >> desc.stiffness;
		data >> desc.type;

		auto rem = get_remainder(data);

		auto extra = stringstream(rem);
		auto const& f = get_factory(desc.type);
		desc.factory = &f;

		extra.exceptions(istream::badbit | istream::failbit);
		desc.extra_data = f.parse(extra);
		extra.exceptions(istream::badbit);

		extra >> ws;
		rem.clear();
		getline(extra, rem);

		if (!rem.empty())
		{
			throw runtime_error{ "Too much data while parsing constraint! " + rem };
		}

		return desc;
	}

}}}
