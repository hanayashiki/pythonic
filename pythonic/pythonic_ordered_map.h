#pragma once
#pragma warning( disable : 4200 ) 
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <vector>
#include <random>

namespace pythonic
{
	template<typename K, typename V, typename Compare>
	class ordered_map_items;

	template<typename K, typename V, typename Compare = std::less<K>>
	struct ordered_map_node
	{
		using node = ordered_map_node<K, V>;

		class const_iterator
		{
		private:
			node * n;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = const K;
			using difference_type = size_t;
			using pointer = const K *;
			using reference = const K &;

			const_iterator(node * n)
				: n(n)
			{
			}

			const_iterator operator++ () noexcept
			{
				n = n->next[0];
				return *this;
			}

			reference operator* () const noexcept
			{
				return n->kv.first;
			}

			bool operator != (const const_iterator & that) const noexcept
			{
				return n != that.n;
			}

			bool operator == (const const_iterator & that) const noexcept
			{
				return !(*this != that);
			}
		};

		std::pair<const K, V> kv;
		enum class MinMaxMark : char
		{
			Min,
			Max,
			None
		} mark = MinMaxMark::None;
		node* next[];
		
		ordered_map_node(K && key, V && value)
			: kv(std::move(key), std::move(value))
		{
		}

		ordered_map_node(const K & key, const V & value)
			: kv(key, value)
		{
		}

		~ordered_map_node() {}

		static node * malloc(size_t n)
		{
			auto size = offsetof(node, next) + sizeof(node*) * (n + 1);
			auto ptr = (node*)::malloc(size);
			memset(ptr, 0, size);
			return ptr;
		}

		static node * create(K && key, V && value, size_t n)
		{
			node * mem = malloc(n);
			return new (mem) ordered_map_node(std::move(key), std::move(value));
		}

		static node * create(const K & key, const V & value, size_t n)
		{
			node * mem = malloc(n);
			return new (mem) ordered_map_node(key, value);
		}

		static node * create(MinMaxMark min_max, size_t n)
		{
			node * mem = malloc(n);
			mem->mark = min_max;
			return mem;
		}

		static node * create_min(size_t n)
		{
			return create(MinMaxMark::Min, n);
		}

		static node * create_max(size_t n)
		{
			return create(MinMaxMark::Max, n);
		}

		void free()
		{
			if (mark == MinMaxMark::None)
			{
				PYC_DEBUG << "dtor called" << std::endl;
				this->~ordered_map_node();
			}
			else
			{
				PYC_DEBUG << "mark: " << int(mark);
				PYC_DEBUG << ", dtor not called" << std::endl;
			}
			::free(this);
		}

		void free_all()
		{
			node * cur = this;
			while (cur != nullptr)
			{
				node * next = cur->next[0];
				cur->free();
				cur = next;
			}
		}

		int key_compare(const K & key) noexcept
		{
			if (this->mark == MinMaxMark::Min)
			{
				return -1;
			}
			if (this->mark == MinMaxMark::Max)
			{
				return 1;
			}
			else 
			{
				return Compare()(kv.first, key) ? -1 :
					Compare()(key, kv.first) ? 1 :
					0;
			}
		}

		void insert_at(size_t next_slot, node * node) noexcept
		{
			node->next[next_slot] = next[next_slot];
			next[next_slot] = node;
		}

		void disconnect_at(size_t next_slot) noexcept
		{
			next[next_slot] = next[next_slot]->next[next_slot];
		}
	};

	template<typename K, typename V, typename Compare = std::less<K>>
	class ordered_map
	{
	public:
		using node = ordered_map_node<K, V, Compare>;
		using const_iterator = typename node::const_iterator;
		using value_type = std::pair<const K, V>;
		using size_type = size_t;
	private:
		node * head;
		node * rear;
		size_t count = 0;

		size_t cur_level = 0;
		const size_t max_level;

		size_t random_level()
		{
			size_t level = 0;
			while (rand() % 2 == 1)
			{
				level++;
			}
			return std::min(level, max_level);
		}

	public:

		ordered_map(size_t max_level = 4) : 
			max_level(max_level),
			rear(node::create_max(0)),
			head(node::create_min(max_level))
		{
			srand(0);
			for (size_t i = 0; i <= max_level; i++)
				head->next[i] = rear;
		}

		ordered_map(const ordered_map &) = delete;
		ordered_map(ordered_map && map) :
			head(map.head),
			rear(map.rear),
			max_level(map.max_level)
		{
		}

		ordered_map & operator = (const ordered_map &) = delete;
		ordered_map & operator = (ordered_map && t)
		{
			head->free_all();
			head = t.head;
			rear = t.rear;
			t.head = nullptr;
			t.rear = nullptr;
			max_level = t.max_level;
		}

		size_t size() const noexcept { return count; }

		ordered_map<K, V> & insert(const K & key, const V & val)
		{
			auto level = random_level();
			return insert(std::forward<const K &>(key), std::forward<const V &>(val), level);
		}

		ordered_map<K, V> & insert(const K & key, const V & val, size_t level)
		{
			this->cur_level = std::max(this->cur_level, level);

			node ** prev = new node*[level + 1];
			node * n = find(key, level, prev, false);
			if (n != nullptr)
			{
				n->kv.second = val;
			}
			else
			{
				count++;
				node * new_node = node::create(key, val, level);
				for (size_t i = 0; i <= level; i++)
				{
					prev[i]->insert_at(i, new_node);
				}
			}
			delete prev;
			return *this;
		}

		ordered_map<K, V> & erase(const K & key)
		{
			node ** prev = new node*[this->cur_level + 1];
			memset(prev, 0, this->cur_level + 1);

			node * n = find(key, this->cur_level, prev, true);

			if (n != nullptr)
			{
				count--;
				for (size_t i = 0; i < this->cur_level; i++)
				{
					if (prev[i]->next[i] == n)
					{
						prev[i]->disconnect_at(i);
					}
				}
				n->free();
			}
			else
			{
				delete prev;
				throw std::exception("invalid ordered_map<K, V> key");
			}
			delete prev;
			return *this;
		}

		V & at(const K & key)
		{
			node * n = find(key, 0, nullptr, false);
			if (n != nullptr)
			{
				return n->kv.second;
			}
			else
			{
				throw std::exception("invalid ordered_map<K, V> key");
			}
		}

		V & operator[](const K & key) noexcept
		{
			node * n = find(key, 0, nullptr, false);
			return n->kv.second;
		}

		/*
		 node * return : if found, the node; else nullptr.
		 key : the const ref to the key to search
		 node_level : the level of the key; it is used for insert
		 prev : a pointer to an array to save the previous nodes of the key's node
		 record_prev : whether to save to 'prev'
		 */
		node * find(const K & key, size_t node_level, node ** prev, bool record_prev) noexcept
		{
			size_t level = cur_level;
			node * cur = head; // Starting from head node, which is -inf
			node * ans = nullptr;
			while (cur != nullptr)
			{
				int cmp = cur->key_compare(key); // Compare with current key
				//std::cout << "comparing 1 with: " << cur->kv.first << "@" << level << "cmp: " << cmp << std::endl;
				if (cmp == 0)
				{
					ans = cur; // If equal, the key is found
					break;
				}
				else
				{
					node * next = cur->next[level];
					int cmp = next->key_compare(key); // Notice that the key has not been found on cur or before, we compare the next key
					//std::cout << "cur_key: " << cur->kv.first << std::endl;
					//std::cout << "comparing 2 with: " << next->kv.first << "@" << level << "cmp: " << cmp << std::endl;
					if (cmp < 0)
					{
						//std::cout << "move to next" << std::endl;
						cur = cur->next[level]; // If the key is smaller than our key, it means we can safely forward on the current level
					}
					else 
					{
						if (cmp == 0 && !record_prev)
						{
							cur = cur->next[level];
						}
						else
						{
							if (cmp == 0)
							{
								ans = next;
							}
							//std::cout << "level -= 1; level = " << level << std::endl;
							if (level <= node_level && prev != nullptr)
							{
								prev[level] = cur;
							}
							if (level == 0) break;
							level -= 1;
						}
					}
				}
			}

			return ans;
		}

		const_iterator begin() const
		{
			return const_iterator(head->next[0]);
		}

		const_iterator end() const
		{
			return const_iterator(rear);
		}

		ordered_map_items<K, V, Compare> items() const noexcept;

		~ordered_map()
		{
			head->free_all();
		}
	};

	template<typename K, typename V, typename Compare = std::less<K>>
	class ordered_map_items
	{
		using node = ordered_map_node<K, V, Compare>;
		node * head;
		node * rear;
	public:
		ordered_map_items(node * head, node * rear) :
			head(head), rear(rear)
		{
		}

		class const_iterator
		{
		private:
			node * n;
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef const std::pair<const K, V> value_type;
			typedef size_t difference_type;
			typedef const std::pair<const K, V> * pointer;
			typedef const std::pair<const K, V> & reference;

			const_iterator(node * n) : n(n) {}

			void operator ++ () noexcept { n = n->next[0]; }
			reference operator* () const noexcept { return n->kv; }
			bool operator != (const const_iterator & that) const noexcept { return n != that.n; }
			bool operator == (const const_iterator & that) const noexcept { return !(*this != that); }
		};

		const_iterator begin() const { return const_iterator(head->next[0]); }
		const_iterator end() const { return const_iterator(rear); }
	};

	template<typename K, typename V, typename Compare>
	inline ordered_map_items<K, V, Compare> ordered_map<K, V, Compare>::items() const noexcept
	{
		return ordered_map_items<K, V, Compare>(this->head, this->rear);
	}
}