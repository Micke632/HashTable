#pragma once


#include <vector>

template <typename K, typename V >
class HashTable
{

	template <typename K, typename V >
	struct Node
	{
		K key = {};
		V value = {};
		int hash;
		Node *next;	
	};

	template <typename K, typename V >
	struct Bucket {
		Bucket(int p) {
			head = new Node<K, V>();			
			head->next = nullptr;			
			pos = p;
			count = 0;
		}
		~Bucket() {
			delete head;		
		}
		int pos;
		int count;
		Node<K, V> *head;		

	};

	std::vector <Bucket<K, V>*> m_buckets;

	int m_size;  //bucket size
	int m_total;  //number of elements

	void insert(Node<K, V> *node) {

		int pos = node->hash % m_size;

		auto *bucket = m_buckets.at(pos);
		if (bucket->count == 0) {
			//insert after head	
			bucket->head->next = node;		
			node->next = nullptr;		
			bucket->count = 1;

		}
		else {
			auto *n = bucket->head->next;
			auto *node_next = n;
			while (n != nullptr) {
				node_next = n;
				n = n->next;
			}
			node_next->next = node;
			bucket->count++;

		}
	}
		template< typename K >
		typename std::enable_if< !std::is_integral< K >::value, int >::type
			getHash(const K &key) const {
			int h = key.getHash();
			return h;
		}

		template< typename K >
		typename std::enable_if< std::is_integral< K >::value, int >::type
			getHash(const K &key) const {
			return key;
		}

		void rehash() {

			m_buckets.resize(m_size * 2);

			for (int i = m_size; i < m_size * 2; i++) {
				m_buckets[i] = new Bucket<K, V>(i);
			}
			int old = m_size;
			m_size *= 2;

			std::vector<Node<K, V>*> moveNodes;

			for (int i = 0; i < old; i++) {
				auto *bucket = m_buckets[i];
				if (bucket && bucket->head->next) {
					auto *prev = bucket->head;
					auto *next = bucket->head->next;
					while (next != nullptr) {

						int h = getHash(next->key);
						int pos = h % m_size;
						if (pos != bucket->pos) {

							auto *next_n = next->next;
							prev->next = next_n;						
							bucket->count--;
							moveNodes.push_back(next);
						}
						prev = next;
						next = next->next;
					}
				}
			}


			for (auto *n : moveNodes) {
				insert(n);
			}
		}

	public:

		using reference = V & ;
		using const_reference = const V&;

		class iterator {

			Node<K, V>* findNext() {
				if (m_lastBucket == m_buckets->size()) {
					return nullptr;
				}

				auto *bucket = (*m_buckets)[m_lastBucket];

				auto *next = bucket->head->next;
				if (next == nullptr) {
					m_lastBucket++;
					return findNext();
				}
			
				while (next != nullptr) {
						
					if (node && node == next) {
						node = node->next;
						break;
					}
					if (!node) {
						node = next;
						break;
					}
					
					next = next->next;															
				}
				
				if (!node) {				
					m_lastBucket++;
					return findNext();
				}

				return node;
				
			}
		public:

			iterator() : m_buckets(nullptr), m_lastBucket(0), node(nullptr)
			{

			}
			iterator(std::vector<Bucket<K, V>*> *ptr, int tot) : m_buckets(ptr), m_lastBucket(0), node(nullptr), m_total(tot), m_count(0)
			{
				node = findNext();
			}
			iterator operator++() {
				if (++m_count == m_total)
					node = nullptr;
				else
					node = findNext();

				return *this;
			}
			bool operator!=(const iterator & other) const {
				return node != other.node;
			}
			std::pair<K,V> operator*() const { return std::make_pair<>(node->key,node->value); }
		private:
			std::vector <Bucket<K, V>*> *m_buckets;
			int m_lastBucket;			
			int m_total;
			int m_count;
			Node<K, V> *node;

		};

		iterator begin() {
			return iterator(&m_buckets, m_total);
		}
		iterator end() {
			return iterator();	//nullptr is end..
		}

		HashTable(int size = 20) {
			m_size = size;
			m_total = 0;
			m_buckets.reserve(size);
			for (int i = 0; i < m_size; i++) {
				m_buckets.push_back(new Bucket<K, V>(i));
			}
		}

		~HashTable()
		{
			for (int i = 0; i < m_size; i++) {
				delete m_buckets[i];
			}
			m_buckets.clear();
		};

		void add(const K& key, const V& value) {

			int hash = getHash(key);

			//remove if exist
			remove(key);

			auto *node = new Node<K, V>();
			node->key = std::move(key);
			node->value = std::move(value);
			node->hash = hash;

			insert(node);
			m_total++;

			if (m_total > 10) {
				//just a value so not cause rehash during simple unit tests
				float f = (1.0f*m_total) / m_size;
				if (f > 0.75) {
					rehash();
				}
			}
		}

		int size() const {
			return m_total;
		}

		int bucketSize() const {
			return m_size;
		}

		const_reference at(K const &key) const {
			int h = getHash(key);
			int pos = h % m_size;

			auto *bucket = m_buckets.at(pos);
			auto *next = bucket->head->next;
			if (next) {
				while (next != nullptr) {
					if (next->key == key) {
						return next->value;
					}
					next = next->next;
				}
			}

			throw std::exception("");
		}


		reference get(K const &key) {
			return const_cast<V&>(const_cast<const HashTable*>(this)->at(key));
		}

		const_reference get(K const &key) const {
			return at(key);
		}


		bool remove(K const &key) {
			int h = getHash(key);
			int pos = h % m_size;

			auto *bucket = m_buckets.at(pos);

			if (bucket->head->next) {
				auto *prev = bucket->head;
				auto *next = bucket->head->next;
				
				while (next != nullptr) {

					if (next->key == key) {

						auto *next_n = next->next;				
						prev->next = next_n;
						bucket->count--;
						m_total--;
						delete next;
						return true;
					}

					prev = next;
					next = next->next;
				}
			}
			
			return false;
		}



		bool exist(K const &key) const {
			int h = getHash(key);
			int pos = h % m_size;
			auto *bucket = m_buckets.at(pos);
			if (bucket->count == 0) return false;

			auto *next = bucket->head->next;
			while (next != nullptr) {
				if (next->key == key) {
					return true;
				}
				next = next->next;
			}
			
			return false;
		}

		void clear() {

			for (int i = 0; i < m_size; i++) {
				auto *bucket = m_buckets[i];

				auto *next = bucket->head->next;
				auto *n = next;
				while (next != nullptr) {
					next = next->next;
					delete n;					
					n = next;
				}							
				bucket->head->next = nullptr;
				
				bucket->count = 0;
			}
			m_total = 0;
		}

	};




