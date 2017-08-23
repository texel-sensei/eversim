#pragma once

#include "core/utility/array_view.h"

#include <glm/glm.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>
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
		
		struct node {
			boost::optional<T> value;
			int next = -1;
		};

	public:

		template<typename VT>
		class base_iterator : public boost::iterator_facade<
			base_iterator<VT>, VT, boost::forward_traversal_tag
		>{
			friend class boost::iterator_core_access;
		public:
			base_iterator(){}
			base_iterator(int cur_node, array_view<node> nodes) : cur_node(cur_node), nodes(nodes)
			{}
		private:
			int cur_node = -1;
			array_view<node> nodes;

			void increment()
			{

				if(cur_node == -1)
				{
					return;
				}
				const auto& node = nodes[cur_node];
				cur_node = node.next;
			}

			VT& dereference() const
			{
				return *(nodes[cur_node].value);
			}

			bool equal(base_iterator const& other) const
			{
				return this->cur_node == other.cur_node;
			}
		};

		using iterator = base_iterator<value_type>;
		using const_iterator = base_iterator<const value_type>;

		explicit spatial_hashmap(float cellsize = 1.f, int num_nodes = 50, int num_slots = 100)
			: cellsize(cellsize), nodes(num_nodes), slots(num_slots), hasher()
		{
			fill(begin(slots), end(slots), -1);
		}

		void reset(int num_nodes, int num_slots)
		{
			nodes.clear();
			nodes.resize(num_nodes);
			slots.resize(num_slots);
			fill(begin(slots), end(slots), -1);
			free_node = 0;
			num_items = 0;
		}

		index_type get_index(key_type const& v) const
		{
			return {v / cellsize};
		}

		template<typename VT>
		void insert(key_type const& k, VT&& v, int node_hint = -1)
		{
			const auto idx = get_index(k);
			const auto slot = hasher(idx) % slots.size();
			
			if(node_hint == -1)
			{
				node_hint = free_node;
				free_node++;
			}

			if(nodes[node_hint].value)
			{
				EVERSIM_THROW(generic_error::InvalidArgument, "Using a node double!");
			}
			auto& n = nodes[node_hint] = { std::forward<VT>(v) , int(node_hint)};
			using namespace std;
			swap(n.next, slots[slot]);
			num_items++;
		}

		void clear()
		{
			reset(nodes.size(), slots.size());
		}
		
		size_t size() const
		{
			return num_items;
		}

		bool empty() const
		{
			return size() == 0;
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
			explicit base_cell(IT f, IT l) : first(f), last(l){}
		};

		using cell = base_cell<iterator>;
		using const_cell = base_cell<const_iterator>;

		cell get_cell(index_type const& idx)
		{
			const auto slot = hasher(idx) % slots.size();
			return cell(iterator(slots[slot], nodes), iterator());
		}
		const_cell get_cell(index_type const& idx) const
		{
			const auto slot = hasher(idx) % slots.size();
			return const_cell(const_iterator(slots[slot], nodes), const_iterator());
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
		std::vector<node> nodes;
		std::vector<int> slots;
		hash hasher;
		int free_node = 0;
		int num_items = 0;
	};
}}}
