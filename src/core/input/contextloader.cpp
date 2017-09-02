#include "core/input/contextloader.h"


#include <easylogging++.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional/optional.hpp>

namespace eversim {	namespace core { namespace input {

using namespace std;
namespace pt = boost::property_tree;

json_loader InputContextLoader::loader;

vector<InputContext> InputContextLoader::generate_contexts_from_json(const string& file)
{

	vector<InputContext> result;

	pt::ptree root;
	pt::read_json(*loader.load(file).get(),root);

	const string pairfile("pairfile");
	
	for (auto &context : root)
	{
		if (context.first == pairfile)
			continue;

		result.emplace_back(context.first);
		auto& inputcontext = result.back();

		for (auto &action : context.second)
		{
			auto action_name = action.first;

			string type = "";
			vector<string> buttons;

			for (auto &data : action.second)
			{
				if (data.first == "type")
				{
					type = data.second.get_value<string>();
				}
				else if (data.first == "keys")
				{
					auto tmp = as_vector<string>(action.second, data.first);
					buttons.insert(buttons.end(), tmp.begin(), tmp.end());
				}
			}

			for (auto& button : buttons)
				inputcontext.register_action(type, action_name, button);
		}
	}
	
	boost::optional<string> c = root.get_optional<string>(pairfile);

	if (c.is_initialized()) {
		auto filename =	c.get();
		
		pt::ptree pairtree;
		pt::read_json(*loader.load(filename).get(), pairtree);

		for (auto& context_d : pairtree) {

			auto it = find_if(begin(result), end(result), [&]
			(const InputContext& c)
			{
				return c.get_name() == context_d.first;
			});

			if (it != result.end())
			{
				auto& context = *it;

				for (auto &type : context_d.second)
				{
					if (type.first == "RANGE") {
						for (auto &rangepair : type.second) {
							
							auto tmp = as_vector<string>(type.second, rangepair.first);
							if (tmp.size() >= 2) {
								context.add_input_pair_range(
									tmp.at(0), tmp.at(1), rangepair.first
									);
							}
						}
					}
				}
			}
		}
	}
	
	return result;
}

}}}