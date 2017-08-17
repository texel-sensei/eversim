#pragma once

#include "core/system/inputcontext.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <vector>
#include <string>

namespace eversim {
	namespace core {
		namespace system {

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
			public:
				static std::vector<InputContext> generate_contexts_from_json(const std::string& filename);
			};

			}
	}
}