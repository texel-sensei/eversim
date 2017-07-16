#pragma once

#include <glm/glm.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>

namespace eversim { namespace core { namespace utility {

	template <typename T>
	class spatial_hashmap {
	public:
		using key_type = glm::vec2;
		using index_type = glm::ivec2;
		using value_type = T;
	private:
		struct hash {
			size_t operator()(index_type const& idx) const
			{
				size_t seed = 0;

				for (int i = 0; i < idx.length(); ++i)
				{
					boost::hash_combine(seed, idx[i]);
				}
				return seed;
			};
		};
		using map_type = std::unordered_multimap<index_type, value_type, hash>;

	public:

		template<typename IT>
		class base_iterator : public boost::iterator_adaptor<base_iterator<IT>, IT, value_type>{
			friend class boost::iterator_core_access;
		public:
			base_iterator() {}
			base_iterator(IT const& x): base_iterator::iterator_adaptor(x){}
		private:
			typename iterator_adaptor::reference dereference() const
			{
				return this->base()->second;
			}
		};

		using iterator = base_iterator<typename map_type::iterator>;
		using const_iterator = base_iterator<typename map_type::const_iterator>;

		explicit spatial_hashmap(float cellsize = 1.f) : cellsize(cellsize)
		{
		}

		index_type get_index(key_type const& v) const
		{
			return {v / cellsize};
		}

		void insert(key_type const& k, value_type const& v)
		{
			const auto idx = get_index(k);
			map.insert(std::make_pair(idx, v));
		}

		void insert(key_type const& k, value_type&& v)
		{
			const auto idx = get_index(k);
			//map[idx] = std::move(v);
			map.insert(std::make_pair(idx, std::move(v)));
		}

		void clear()
		{
			map.clear();
		}
		
		size_t size() const
		{
			return map.size();
		}

		bool empty() const
		{
			return map.empty();
		}

		float get_cell_size() const
		{
			return cellsize;
		}

		template<typename IT>
		struct base_cell {
			IT begin() const
			{
				return first;
			}
			IT end() const
			{
				return last;
			}
		private:
			friend class spatial_hashmap;
			IT first;
			IT last;
			base_cell(IT f, IT l) : first(f), last(l){}
		};

		using cell = base_cell<iterator>;
		using const_cell = base_cell<const_iterator>;

		cell get_cell(index_type const& idx)
		{
			iterator first, last;
			std::tie(first, last) = map.equal_range(idx);
			return cell{ first, last };
		}
		const_cell get_cell(index_type const& idx) const
		{
			iterator first, last;
			std::tie(first, last) = map.equal_range(idx);
			return const_cell{ first, last };
		}

		cell get_cell(key_type const& v)
		{
			return get_cell(get_index(v));
		}
		const_cell get_cell(key_type const& v) const
		{
			return get_cell(get_index(v));
		}
	private:
		float cellsize = 1.f;
		map_type map;
		
	};
}}}
