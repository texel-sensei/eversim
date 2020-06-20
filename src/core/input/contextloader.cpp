#include "core/input/contextloader.h"


#include <easylogging++.h>

#include <functional>


namespace eversim {
	namespace core {
		namespace input {

			using namespace std;
			namespace pt = boost::property_tree;

			json_loader InputContextLoader::loader;

			namespace {
				map<string, function<double(double)>> funcs = 
				{
					{"EXP",[](const double in) { return exp(in); }},
					{"SIN",[](const double in) { return sin(in); }},
					{"COS",[](const double in) { return cos(in); }}
				};
			}

			vector<InputContext> InputContextLoader::generate_contexts_from_property_tree(boost::property_tree::ptree& root)
			{
				vector<InputContext> result;

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
							else if (data.first.substr(0, 7) == "keypair" && type == "RANGE") {
								auto& pd = data.second;

								const vector<string> ids = { "keys","values","interval","function" };
								size_t cnt = 0;
								for (const auto& id : ids) {
									auto it = find_if(begin(pd), end(pd),
										[&](const auto& pairdata) { return pairdata.first == id; });
									if (it != end(pd)) cnt++;
								}
								if (cnt == ids.size()) {
									auto keys = as_vector<string>(pd, ids.at(0));
									auto values = as_vector<int>(pd, ids.at(1));
									auto interval = as_vector<int>(pd, ids.at(2));
									auto function = find_if(begin(pd), end(pd),
										[&](const auto& pairdata) { return pairdata.first == ids.at(3); })->second.get_value<string>();

									if (keys.size() >= 2 && values.size() >= 2 && interval.size() >= 2){
										auto func = funcs.at(function);

										inputcontext.register_button_to_range
										(
											action_name, keys[0], keys[1], 
											{values[0],values[1]}, {interval[0],interval[1]},
											func
										);
									}
							
								}
							}
						}

						for (auto& button : buttons)
							inputcontext.register_action(type, action_name, button);
					}
				}

	boost::optional<string> c = root.get_optional<string>(pairfile);

	if (c.is_initialized()) {
		auto filename = c.get();

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

std::vector<InputContext> InputContextLoader::generate_contexts_from_json(std::istream& stream)
{
	pt::ptree root;
	pt::read_json(stream, root);
	return generate_contexts_from_property_tree(root);
}

vector<InputContext> InputContextLoader::generate_contexts_from_json(const string& file)
{
	pt::ptree root;
	pt::read_json(*loader.load(file).get(),root);
	return generate_contexts_from_property_tree(root);
}

}}}
