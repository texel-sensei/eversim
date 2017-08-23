#pragma once

#include "core/utility/array_view.h"

#include <glm/glm.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>
#include <unordered_map>
#include <atomic>

namespace eversim { namespace core { namespace utility {

	namespace detail {
		inline void swap(int& a, int& b) noexcept
		{
			int tmp = a;
			a = b;
			b = tmp;
		}

		// does only guarantee atomicity on parameter b
		inline void swap(int& a, std::atomic<int>& b) noexcept
		{
			a = b.exchange(a);
		}

		/*
		 * Wrapper class to allow copying of atomics.
		 * The copy itself is not atomic!
		 */
		template <typename T>
		struct atomwrapper
		{
			std::atomic<T> _a;

			atomwrapper()
				:_a()
			{}

			atomwrapper(const std::atomic<T> &a)
				:_a(a.load())
			{}

			atomwrapper(T a) : _a(a){}

			atomwrapper(const atomwrapper &other)
				:_a(other._a.load())
			{}

			atomwrapper &operator=(const atomwrapper &other)
			{
				_a.store(other._a.load());
				return *this;
			}

			operator std::atomic<T>&() {
				return _a;
			}

			operator T()
			{
				return _a;
			}
		};

		template<typename T>
		struct get_copy_wrapper {
			using type = T;
		};

		template<typename T>
		struct get_copy_wrapper<std::atomic<T>> {
			using type = atomwrapper<T>;
		};

		template<typename T>
		using get_copy_wrapper_t = typename get_copy_wrapper<T>::type;
	}

	template <typename T, typename SI = int>
	class spatial_hashmap {
	public:
		using key_type = glm::vec2;
		using index_type = glm::ivec2;
		using value_type = T;
		using slot_index_t = SI;
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
				node_hint = free_node++;
			}

			if(nodes[node_hint].value)
			{
				EVERSIM_THROW(generic_error::InvalidArgument, "Using a node double!");
			}
			auto& n = nodes[node_hint] = { std::forward<VT>(v) , int(node_hint)};
			using namespace std;
			detail::swap(n.next, slots[slot]);
			++num_items;
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

		void set_cell_size(float x)
		{
			cellsize = x;
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
		std::vector<detail::get_copy_wrapper_t<slot_index_t>> slots;
		hash hasher;
		slot_index_t free_node = 0;
		slot_index_t num_items = 0;
	};
}}}
