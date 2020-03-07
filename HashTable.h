#pragma once


#include <vector>
#include <list>

template <typename K, typename V, 
	typename Alloc=std::allocator<V>>		//V is not allocated , but we need the template to rebind
class HashTable
{
private:
	

	template <typename K, typename V >
	struct Node
	{
		K key = {};
		V value = {};
		Node *next = {};
	};
	using node_type = Node<K,V>;
	using node_allocator = typename Alloc::template rebind<node_type>::other;
	node_allocator allocator;
	using reference = V & ;
	using const_reference = const V&;
	

	std::vector<node_type*, Alloc> m_pool;

	template <typename K, typename V >

	struct Bucket {
		Bucket(int p) {			
		
			pos = p;
			count = 0;
			active = false;		
		}
		
		void removeNodes(HashTable *home) {
			auto *next = node.next;
			auto *n = next;
			while (next != nullptr) {
				next = next->next;
				home->releaseNode(n);
				n = next;
			}
			node.next = nullptr;
			active = false;
			count = 0;
		}

		~Bucket() {			
		}

		Bucket *copy(HashTable *home) const {
			Bucket *bucket = new Bucket<K, V>(pos);

			auto *this_bucket_next = this->node.next;
			
			bucket->active = this->active;
			bucket->node.key = this->node.key;
			bucket->node.value = this->node.value;

			int c = bucket->active ? 1:0;

			while (this_bucket_next != nullptr) {

				auto *node = home->getNode();
				node->key = this_bucket_next->key;
				node->value = this_bucket_next->value;				
				node->next = nullptr;
				c++;
							
				this_bucket_next = this_bucket_next->next;
				
				if (bucket->node.next == nullptr) {
					bucket->node.next = node;
				}
				else {
					//find end
					auto *node_next = bucket->node.next;
					auto *n = node_next;
					while (n != nullptr) {
						node_next = n;
						n = n->next;
					}
					node_next->next = node;
				}

			}
			assert(this->count == c);

			bucket->count = this->count;
			return bucket;
		}

		int pos;
		int count;
		bool active;
		Node<K,V> node;		

	};

	using Bucketvector = std::vector<Bucket<K, V>*, Alloc>;

	Bucketvector m_buckets;



	int m_size;  //bucket size
	int m_total;  //number of elements


	node_type* getNode() {
		if (!m_pool.empty()) {
			auto *p = m_pool.back();
			m_pool.pop_back();
			return p;

		}
		auto *node = allocator.allocate(1);
	//	allocator.construct(node);
		return node;
	}

	void  releaseNode(node_type* node) {
		m_pool.push_back(node);	
	}

	void insert(const K& key, const V& value) {

		int hash = getHash(key);
		int pos = hash % m_size;
		auto *bucket = m_buckets[pos];		
		if (!bucket->active || bucket->count == 0) {
		
			bucket->node.key = key;
			bucket->node.value = value;
			bucket->count++;
			bucket->active = true;
		}
		else {
			bucket->count++;
			auto *node = getNode();
		
			node->key = key;
			node->value = value;
			node->next = nullptr;
			if (bucket->node.next == nullptr) {
				bucket->node.next = node;
				return;
			}
			
			auto *node_next = bucket->node.next;
			auto *n = node_next;
			while (n != nullptr) {
				node_next = n;
				n = n->next;
			}
			node_next->next = node;
		}
			
		
		
		
	}
	
	void insert(std::pair<node_type*,int> &node) {

		assert(node.first->next == nullptr);

		int pos = node.second % m_size;

		auto *bucket = m_buckets[pos];
		if (!bucket->active || bucket->count == 0) {			
			bucket->node.key = std::move(node.first->key);
			bucket->node.value = std::move(node.first->value);
			bucket->count++;
			bucket->active = true;
			releaseNode(node.first);				
		}
		else {
			bucket->count++;
			if (bucket->node.next == nullptr) {
				bucket->node.next = node.first;
				return;
			}
			//find end
			auto *n = bucket->node.next;
			auto *node_next = n;
			while (n != nullptr) {
				node_next = n;
				n = n->next;
			}
			node_next->next = node.first;
			
		}
	}
	

	bool updateIfExist(int hash, const K &key, const V &value) {
		int pos = hash % m_size;
		auto *bucket = m_buckets.at(pos);
		
		if (bucket->active && bucket->node.key == key) {
			bucket->node.value = value;
			return true;
		}
		auto *next = bucket->node.next;

		while (next != nullptr) {
			if (next->key == key) {
				next->value = value;
				return true;
			}
			next = next->next;
		}
		
		return false;
	}

	bool removeAt(int hash,const K&key) {
		int pos = hash % m_size;
		auto *bucket = m_buckets[pos];

		if (bucket->active && bucket->node.key == key) {
			bucket->active = false;
			m_total--;
			bucket->count--;
			assert(bucket->count >= 0);

			moveToFront(bucket);	//check if we can move the next to the front

			return true;
		}
	
		if (bucket->node.next) {
			auto *next = bucket->node.next;
			if (next->key == key) {
				auto *next_n = next->next;
				releaseNode(next);
				bucket->count--;
				assert(bucket->count >= 0);
				bucket->node.next = next_n;
				m_total--;						

				return true;
			}
			auto *prev = next;
			next = bucket->node.next->next;
			while (next != nullptr) {

				if (next->key == key) {

					auto *next_n = next->next;
					prev->next = next_n;
					bucket->count--;
					assert(bucket->count >= 0);
					m_total--;
					releaseNode(next);
					return true;
				}

				prev = next;
				next = next->next;
			}
		}
		
		return false;
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

	bool check(Bucket<K, V> *bucket) const {
		int tot = 0;
		if (bucket->active) {
			tot = 1;

		}
		if (bucket->count == 1) 
		{
			if (bucket->active) {
				assert(nullptr == bucket->node.next);
			}

		}

		if (bucket->node.next) {
			auto *next = bucket->node.next;
			while (next != nullptr) {
				tot++;
				next = next->next;

				if (tot > bucket->count) {
					assert(true);
				}
			}
		}

		return bucket->count == tot;
	}

	void moveToFront(Bucket<K,V> *bucket) {
		
		if (!bucket->active && bucket->node.next) {
			auto *next = bucket->node.next;
			bucket->node.key = std::move(next->key);
			bucket->node.value = std::move(next->value);
			bucket->active = true;
			if (next->next) {			
				bucket->node.next = next->next;
			}
			else {
				bucket->node.next = nullptr;
			}

			releaseNode(next);
		}
	}

	

	void rehash() {
		
		m_buckets.resize(m_size * 2);

		for (int i = m_size; i < m_size * 2; i++) {
			m_buckets[i] = new Bucket<K, V>(i);
		}
		int old = m_size;
		m_size *= 2;

		std::vector< std::pair<node_type*,int> > moveNodes;
		moveNodes.reserve(old);

		for (int i = 0; i < old; i++) {
			auto *bucket = m_buckets[i];

			if (bucket->active) {
				int h = getHash(bucket->node.key);
				int pos = h % m_size;
				if (pos != bucket->pos) {
					auto *node = getNode();				
					node->key = std::move(bucket->node.key);
					node->value = std::move(bucket->node.value);
					node->next = nullptr;
					bucket->active = false;
					bucket->count--;
					assert(bucket->count >= 0);
					moveNodes.emplace_back( node,h  );
				}
			}
	
			auto *HOME = bucket->node.next;  	//points to bucket->node.next
			auto *node = bucket->node.next;
			auto *prev = bucket->node.next;		//points to previos active node

			while (node) {
								
				int h = getHash(node->key);
				int pos = h % m_size;
				auto *next_node = node->next;
				if (pos != bucket->pos) {
					//remove the node from this bucket
					if (node == HOME) {
						bucket->node.next = next_node;
						HOME = bucket->node.next;
					}
					else {
						prev->next = next_node;
					}
					bucket->count--;
					assert(bucket->count >= 0);
					node->next = nullptr;
					moveNodes.emplace_back( node,h  );
					
				} 
				else {
					prev = node;
				}

				node = next_node;
				
			}

			moveToFront(bucket);

		}


		for (auto &n : moveNodes) {
			insert(n);
		}
		
	}

	const_reference	at(K const &key) const {
		int h = getHash(key);
		int pos = h % m_size;

		auto *bucket = m_buckets.at(pos);
		if (bucket->active && bucket->node.key == key) {
			return bucket->node.value;
		}
		auto *next = bucket->node.next;

		while (next != nullptr) {
			if (next->key == key) {
				return next->value;
			}
			next = next->next;
		}

		throw std::runtime_error("not found");

	}

	public:

				
		node_allocator& getAllocator() {
			return allocator;
		}

		class iterator {

			friend class HashTable;

			Node<K, V>* findNext() const {
				if (m_lastBucket == m_buckets->size()) {
					if (m_checkFront) {
						return nullptr;
					}
					else {
						//reached end , now check all nodes at front
						m_checkFront = true;
						m_lastBucket = 0;
					}
				}
				
				auto *bucket = (*m_buckets)[m_lastBucket];

				if (m_checkFront) {
					if (bucket->active) {
						m_headNode.key = bucket->node.key;
						m_headNode.value = bucket->node.value;		
						m_lastBucket++;
						return &m_headNode;
					}								
					m_lastBucket++;
					return findNext();
				}

				auto *next = bucket->node.next;
				if (next == nullptr) {
				
					m_lastBucket++;
					return findNext();
				}

				while (next != nullptr) {

					if (m_node && m_node == next) {
						m_node = m_node->next;
						break;
					}
					if (!m_node) {
						m_node = next;
						break;
					}

					next = next->next;
				}

				if (!m_node) {
					m_lastBucket++;				
					return findNext();
				}
				
				return m_node;

			}
		

			iterator() : 
				m_buckets(nullptr), 
				m_lastBucket(0), 
				m_node(nullptr),
				m_total(0),
				m_count(0)
			{
			}

			iterator(Node<K, V> *n) :
				m_buckets(nullptr),
				m_lastBucket(0),
				m_node(n),
				m_total(0),
				m_count(0)
			{
			}


			iterator(const Bucketvector *ptr, int tot) :
				m_buckets(ptr),
				m_lastBucket(0), 
				m_node(nullptr), 
				m_total(tot), 
				m_count(0),
				m_checkFront(false)
			{
				m_node = findNext();
			}
		public:
			iterator operator++() {
				if (++m_count >= m_total)
					m_node = nullptr;	//we have seen all, just exit
				else
					m_node = findNext();

				return *this;
			}
			bool operator!=(const iterator & other) const {
				return m_node != other.m_node;
			}
			bool operator==(const iterator & other) const {
				return m_node == other.m_node;
			}
			std::pair<K, V> operator*() const { return { m_node->key,m_node->value }; }
		private:
			const Bucketvector *m_buckets;
			mutable int m_lastBucket;
			int m_total;
			int m_count;
			mutable Node<K, V> m_headNode;
			mutable Node<K, V> *m_node;
			mutable bool m_checkFront;

		};

		iterator begin() const {
			return iterator(&m_buckets, m_total);
		}
		iterator end() const {
			return iterator();	//m_node =nullptr is end..
		}
		
		explicit HashTable(int size = 20) {
			m_pool.reserve(20);
			m_size = std::max(size,1);
			m_total = 0;
			m_buckets.reserve(size);
			for (int i = 0; i < m_size; i++) {
				m_buckets.push_back(new Bucket<K, V>(i));
			}
		}

		~HashTable()
		{
			destroy();
		};

		bool check() const{
			int tot = 0;
			for (int i = 0; i < m_size; i++) {
				if (!check(m_buckets[i])) return false;
				tot += m_buckets[i]->count;
			}

			return tot == m_total;
		}

		void destroy() {
			for (int i = 0; i < m_size; i++) {
				m_buckets[i]->removeNodes(this);
				delete m_buckets[i];
			}
			m_buckets.clear();
			m_size = 0;
			m_total = 0;

			while (!m_pool.empty()) {
				allocator.destroy(m_pool.back());
				allocator.deallocate(m_pool.back(),1);
				m_pool.pop_back();
			}

		}

		HashTable(HashTable &&other)
		{
			m_pool = std::move(other.m_pool);
			m_size = other.m_size;
			m_total = other.m_total;			
			m_buckets = std::move(other.m_buckets);					
			other.m_size = 0;
			other.m_total = 0;		
		}

		HashTable& operator=(HashTable &&other) {

			destroy();

			m_pool = std::move(other.m_pool);
			m_size = other.m_size;
			m_total = other.m_total;
			m_buckets = std::move(other.m_buckets);
			other.m_size = 0;
			other.m_total = 0;

			return *this;
		}

		HashTable& operator=(const HashTable &other) 
		{
			destroy();

			m_size = other.m_size;
			m_total = other.m_total;
			m_buckets.reserve(m_size);
			for (int i = 0; i < m_size; i++) {
				auto *b = other.m_buckets[i]->copy(this);
				m_buckets.push_back(b);
			}
			return *this;
		}

		HashTable(const HashTable &other)
		{		
			m_size = other.m_size;
			m_total = other.m_total;
			m_buckets.reserve(m_size);
			for (int i = 0; i < m_size; i++) {
				auto *b = other.m_buckets[i]->copy(this);
				m_buckets.push_back(b);
			}

		}
		
		reference operator[](const K &key)  {

			if (exist(key)) {
				return get(key);
			}
			else {
				V v = {};
				add(key, v);
				return get(key);
			}
		}

		const_reference operator[](const K &key) const {
			
			return get(key);
			
		}

		
		void add(const K& key, const V& value) {

			int hash = getHash(key);


			if (updateIfExist(hash, key, value)) {
				return;
			}

			insert(key,value);

			m_total++;

			if (m_total > 10) {
				//just a value so not cause rehash during simple unit tests
				float f = (1.0f*m_total) / m_size;
				if (f > 0.80) {
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
		

			
		iterator find(K const &key) const {
			int h = getHash(key);
			int pos = h % m_size;

			auto *bucket = m_buckets.at(pos);			
			if (bucket->active && bucket->node.key == key) {
				return iterator(&bucket->node);				
			}
			auto *next = bucket->node.next;
			while (next != nullptr) {
				if (next->key == key) {
					return iterator(next);
				}

				next = next->next;
			}
			
			return end();
		}

		reference get(K const &key) {
			return const_cast<V&>(const_cast<const HashTable*>(this)->at(key));
		}

		const_reference get(K const &key) const {
			return at(key);
		}

	
		bool remove(K const &key) {
			int h = getHash(key);
			return removeAt(h,key);
		}



		bool exist(K const &key) const {
			int h = getHash(key);
			int pos = h % m_size;
			auto *bucket = m_buckets[pos];
			if (bucket->count == 0) return false;
			if (bucket->active && bucket->node.key == key) {
				return true;
			}
			auto *next = bucket->node.next;
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
				bucket->removeNodes(this);
			}
			m_total = 0;
		}

	};

	



