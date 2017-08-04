#pragma once

#include "core/error.h"

#include <bitset>
#include <list>
#include <vector>
#include <memory>
#include <cassert>
#include <map>

namespace eversim { namespace core { namespace utility {

	template <typename T, size_t page_size = 50>
	class object_pool {
		struct node;
	public:
		explicit object_pool()
		{
			check_new_node();
		}

		~object_pool()
		{
			for (auto& c : *this)
			{
				c.~T();
			}
		}

		object_pool(object_pool const&) = delete;
		object_pool& operator=(object_pool const&) = delete;

		template <typename... Args>
		T* emplace(Args&&... args)
		{
			const auto place = allocate();
			new(place) T(std::forward<Args>(args)...);
			return place;
		}

		T* insert(T const& t)
		{
			const auto place = allocate();
			new(place) T(t);
			return place;
		}

		T* insert(T&& t)
		{
			const auto place = allocate();
			new(place) T(std::move(t));
			return place;
		}

		template<typename Iterator>
		void insert(Iterator begin, Iterator end)
		{
			for(; begin != end; ++begin)
			{
				insert(*begin);
			}
		}

		template <typename U>
		struct base_iterator {
			using difference_type = ptrdiff_t;
			using value_type = U;
			using pointer = U*;
			using reference = U&;
			using iterator_category = std::forward_iterator_tag;
			
			base_iterator() = default;

			reference operator*() const
			{
				return *pos;
			}

			pointer operator->() const
			{
				return pos;
			}

			// prefix
			base_iterator& operator++()
			{
				seek_next();
				return *this;
			}

			base_iterator operator++(int)
			{
				auto copy = *this;
				++(*this);
				return copy;
			}

			friend bool operator==(base_iterator const& a, base_iterator const& b)
			{
				return a.pos == b.pos;
			}

			friend bool operator!=(base_iterator const& a, base_iterator const& b)
			{
				return !(a == b);
			}

			operator base_iterator<const U>() const
			{
				return const_iterator{ current_node, last_pos, pos };
			}

		private:
			friend class object_pool;
			using node_iterator = std::conditional_t<
				std::is_const_v<U>,
				typename std::list<node>::const_iterator,
				typename std::list<node>::iterator
			>;
			node_iterator current_node;
			pointer last_pos;
			pointer pos = nullptr;

			void seek_next()
			{
				while (pos != last_pos)
				{
					++pos;
					if (pos == last_pos)
						break;
					if (pos == current_node->end())
					{
						++current_node;
						pos = current_node->begin();
					}
					if (current_node->is_slot_filled(pos))
					{
						break;
					}
				}
			}

			base_iterator(node_iterator n_it, pointer last_pos, pointer p = nullptr) :
				current_node(n_it), last_pos(last_pos), pos(p)
			{
				if (!pos)
				{
					pos = current_node->begin();
					if (!current_node->is_slot_filled(pos))
					{
						seek_next();
					}
				}
			}
		};

		using iterator = base_iterator<T>;
		using const_iterator = base_iterator<T const>;

		iterator begin()
		{
			return iterator{memory.begin(), current_pos };
		}

		iterator end()
		{
			return iterator{--memory.end(), current_pos, current_pos};
		}

		const_iterator begin() const
		{
			return const_iterator{memory.begin(), current_pos };
		}

		const_iterator end() const
		{
			return const_iterator{ --memory.end(), current_pos, current_pos };
		}

		size_t size() const noexcept
		{
			return size_;
		}

		bool empty() const noexcept
		{
			return size() == 0;
		}

		iterator locate(T* ptr)
		{
			auto pos = memory_map.upper_bound(ptr);
			if (pos == memory_map.begin())
				return end();
			pos = prev(pos);
			if (!pos->second->contains(ptr))
				return end();
			return iterator{ pos->second, current_pos, ptr };
		}

		const_iterator locate(T const* ptr) const
		{
			auto* const nonconst_this = const_cast<object_pool*>(this);
			return nonconst_this->locate(const_cast<T*>(ptr));
		}

		const_iterator erase(const_iterator pos)
		{
			auto* ptr = const_cast<T*>(pos.pos);
			ptr->~T();
			deallocate(ptr, const_cast<node&>(*pos.current_node));
			return ++pos;
		}

		const_iterator erase(const_iterator first, const_iterator last)
		{
			for(;first != last; ++first)
			{
				erase(first);
			}
			return last;
		}

	private:
		struct node {
			static_assert(sizeof(T) >= sizeof(node*), "Objects in a pool must at leas have the size of a pointer!");
			
			size_t indexof(T const* ptr) const noexcept
			{
				// can't use EVERSIM_ASSERT here because indexof is noexcept
				assert(ptr >= begin());
				assert(ptr < end());
				return ptr - begin();
			}

			bool contains(T const* ptr) const noexcept {
				return ptr >= begin() && ptr < end();
			}

			void mark_slot(T* ptr, bool filled)
			{
				filled_spots[indexof(ptr)] = filled;
				if (!filled)
				{
					*reinterpret_cast<node**>(ptr) = this;
				}
			}

			bool is_slot_filled(T const* ptr) const
			{
				return filled_spots[indexof(ptr)];
			}

			T* begin() noexcept
			{
				return reinterpret_cast<T*>(memory);
			}

			T* end() noexcept
			{
				return reinterpret_cast<T*>(memory + sizeof(memory));
			}

			T const* begin() const noexcept
			{
				return reinterpret_cast<T const*>(memory);
			}

			T const* end() const noexcept
			{
				return reinterpret_cast<T const*>(memory + sizeof(memory));
			}

		private:
			alignas(T) char memory[sizeof(T) * page_size];
			std::bitset<page_size> filled_spots;
		};

		using nodelist = std::list<node>;
		nodelist memory;
		std::vector<T*> free_store;
		std::map<T const*, typename nodelist::iterator> memory_map;
		T* current_pos = nullptr;
		size_t size_ = 0;

		void check_new_node()
		{
			if (current_pos == nullptr || current_pos == memory.back().end())
			{
				memory.emplace_back();
				current_pos = memory.back().begin();
				memory_map[current_pos] = std::prev(memory.end());
			}
		}

		T* allocate()
		{
			++size_;
			if (!free_store.empty())
			{
				const auto mem = free_store.back();
				free_store.pop_back();

				auto* containing_node = *reinterpret_cast<node**>(mem);
				containing_node->mark_slot(mem, true);

				return mem;
			}

			check_new_node();

			auto& current_node = memory.back();
			EVERSIM_ASSERT(current_pos != current_node.end());

			const auto ptr = current_pos++;
			current_node.mark_slot(ptr, true);
			return ptr;
		}

		void deallocate(T* ptr)
		{
			auto pos = locate(ptr);
			EVERSIM_ASSERT(pos != end());
			deallocate(ptr, *pos->current_node);
		}

		void deallocate(T* ptr, node& n)
		{
			EVERSIM_ASSERT(n.contains(ptr));
			size_--;
			free_store.push_back(ptr);
			n.mark_slot(ptr, false);
		}
	};

}}}