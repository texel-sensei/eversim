#include "core/input/contextloader.h"

#include <easylogging++.h>

namespace eversim {	namespace core { namespace input {

using namespace std;
namespace pt = boost::property_tree;

vector<InputContext> InputContextLoader::generate_contexts_from_json(const string& filename) 
{
	ifstream ifs(filename);
	string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));

	istringstream iss(content);

	return generate_contexts_from_json(iss);
}

vector<InputContext> InputContextLoader::generate_contexts_from_json(
	std::basic_istream<pt::ptree::key_type::value_type>& file)
{
	vector<InputContext> result;

	pt::ptree root;

	pt::read_json(file, root);
	
	for (auto &context : root)
	{
		result.emplace_back(context.first);
		auto& inputcontext = result.back();

		for (auto &action : context.second)
		{
			auto val = as_vector<string>(context.second, action.first);

			inputcontext.register_action(val.at(1), action.first, val.at(0));
		}
	}

	return result;
}

}}}