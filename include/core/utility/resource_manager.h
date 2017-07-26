#pragma once

#include "core/utility/filesystem_wrapper.h"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace eversim { namespace core { namespace utility {
	
	/*
	 * CRTP based class to simplify resource loading.
	 * Template parameters:
	 *	T	specific class that does the loading
	 *	KT	key to describe the loaded object
	 *	VT	loaded type
	 *	
	 *	the subclass must have the function 'load_file'
	 *
	 	class example_loader : public utility::resource_manager<example_loader, string, string>
		{
		public:
			std::shared_ptr<value_type> load_file(string const& s) const
			{
				return make_shared<value_type>(s);
			}
		};
	 */
	template<typename T, typename KT, typename VT>
	class resource_manager
	{
	public:
		using value_type = VT;
		using key_type = KT;

		struct file_not_found_error : std::runtime_error
		{
			using std::runtime_error::runtime_error;
		};

		void add_search_directory(std::string const& dir)
		{
			search_directories.push_back(dir);
		}

		std::shared_ptr<value_type> load(key_type const& key)
		{
			auto sp = cache[key].lock();
			if (!sp) {
				const auto filename = as_derived().key_to_name(key);
				const auto file = search(filename);
				cache[key] = sp = as_derived().load_file(file.string());
			}
			return sp;
		}

		std::string const& key_to_name(std::string const& s)
		{
			return s;
		}

	private:
		std::vector<std::string> search_directories;
		std::map<key_type, std::weak_ptr<value_type>> cache;

		T& as_derived() noexcept {
			return static_cast<T&>(*this);
		}

		T const& as_derived() const noexcept {
			return static_cast<T const&>(*this);
		}

		fs::path search(fs::path const& desc) const
		{
			for(auto const& dir : search_directories)
			{
				auto full_path = dir / desc;
				if(fs::exists(full_path))
				{
					return full_path;
				}
			}
			throw file_not_found_error{ "Could not find " + desc.string() };
		}
		
	};

} /*utility*/ } /*core*/ } /*eversim*/