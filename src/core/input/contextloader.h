#pragma once

#include "core/input/inputcontext.h"
#include "core/input/jsonloader.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional/optional.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <istream>
#include <sstream>

namespace eversim {
	namespace core {
		namespace input {

			

			template <typename T>
			std::vector<T> as_vector(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
			{
				std::vector<T> r;
				for (auto& item : pt.get_child(key))
					r.push_back(item.second.get_value<T>());
				return r;
			}

			class InputContextLoader
			{
			private:
				static json_loader loader;

				static std::vector<InputContext> generate_contexts_from_property_tree(boost::property_tree::ptree& root);

			public:
				static std::vector<InputContext> generate_contexts_from_json(const std::string&);
				static std::vector<InputContext> generate_contexts_from_json(std::istream&);

				static json_loader* get_loader()
				{
					return &loader;
				}
			};

			}
	}
}