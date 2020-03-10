#pragma once


#include <vector>


template <typename K, typename V, 
	typename Alloc=std::allocator<V>>		
class HashTable
{
public:
	using size_type = unsigned int;

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
	using reference = V& ;
	using const_reference = const V&;
	
	using hash_type = unsigned int;

	std::vector<node_type*, Alloc> m_pool;

	template <typename K, typename V >
	struct Bucket {	
		
		Bucket() {			
			count = 0;
			active = false;		
		}
		
		void removeNodes(node_allocator &allocator) {
			auto *next = node.next;
			auto *n = next;
			while (next != nullptr) {
				next = next->next;
				allocator.destroy(n);
				allocator.deallocate(n, 1);							
				n = next;
			}
			node.next = nullptr;
			active = false;
			count = 0;
		}


		Bucket *copy(HashTable *home) const {
			Bucket *bucket = new Bucket<K, V>();

			auto *this_bucket_next = this->node.next;
			
			bucket->active = this->active;
			if (bucket->active) {
				bucket->node.key = this->node.key;
				bucket->node.value = this->node.value;
			}

			unsigned short nr_of_nodes = bucket->active ? 1:0;

			while (this_bucket_next != nullptr) {

				auto *node = home->getNode();
				node->key = this_bucket_next->key;
				node->value = this_bucket_next->value;				
				node->next = nullptr;
				nr_of_nodes++;
							
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
			assert(this->count == nr_of_nodes);

			bucket->count = this->count;
			return bucket;
		}
				
		unsigned short count;
		bool active;
		Node<K,V> node;		

	};

	using Bucketvector = std::vector<Bucket<K, V>*, Alloc>;

	
	
public:

	

	class iterator {		

	public:	
		using iterator_category = std::forward_iterator_tag;
		using value_type = void;
		using difference_type = size_type;
		using pointer = void;
		using reference = void;

		friend class HashTable;

		node_type* findNext() const {

			if (m_lastBucket >= m_buckets->size()) {
				return nullptr;
			}

			auto *bucket = (*m_buckets)[m_lastBucket];
			if (!m_frontChecked) {
				if (bucket->active) {
					m_frontChecked = true;
					return &bucket->node;
				}

				m_frontChecked = true;
				m_node = nullptr;
			}

			auto *node = bucket->node.next;
			if (node == nullptr) {
				m_frontChecked = false;
				m_lastBucket++;
				return findNext();
			}

			if (m_frontChecked) {
				if (m_node == &bucket->node) {
					m_node = nullptr;
				}
			}

			while (node != nullptr) {

				if (m_node && m_node == node) {
					m_node = m_node->next;
					break;
				}
				if (!m_node) {
					m_node = node;
					break;
				}

				node = node->next;
			}


			if (!m_node) {
				m_lastBucket++;
				m_frontChecked = false;
				return findNext();
			}
			return m_node;

		}

		//end iterator
		iterator() :
			m_buckets(nullptr),
			m_lastBucket(0),
			m_node(nullptr),
			m_size(0),
			m_count(0)
		{
		}

		iterator(node_type *n,
			const Bucketvector *ptr,
			size_type pos,
			size_type tot,
			bool frontChecked = true) :
			m_buckets(ptr),
			m_lastBucket(pos),
			m_node(n),
			m_size(tot),
			m_count(0),
			m_frontChecked(frontChecked)
		{
			if (m_node == nullptr)
				m_node = findNext();
		}


	public:
		//pre
		iterator& operator++() {
			if (++m_count >= m_size)
				m_node = nullptr;
			else
				m_node = findNext();

			return *this;
		}
		//post
		iterator operator++(int ) {
						
			iterator it(m_node, m_buckets, m_lastBucket, m_size, m_frontChecked);
			if (++m_count >= m_size)
				m_node = nullptr;
			else
				m_node = findNext();
				
			return it;
		}
		
		bool operator!=(const iterator &other) const {
			return m_node != other.m_node;
		}
		bool operator==(const iterator &other) const {
			return m_node == other.m_node;
		}

	

		std::pair<const K&, V&> operator*() const { return { m_node->key,m_node->value }; }
	private:
		const Bucketvector *m_buckets;
		mutable size_type m_lastBucket;
		size_type m_size;
		size_type m_count;
		mutable node_type *m_node;
		mutable bool m_frontChecked;

	};



private:

	iterator createIterator(node_type *node, size_type pos, bool frontChecked = true) const {
		return iterator(node, &m_buckets, pos, m_size, frontChecked);
	}

	node_type* getNode() {
		if (!m_pool.empty()) {
			auto *p = m_pool.back();
			m_pool.pop_back();
			return p;

		}
			
		auto *node = allocator.allocate(1);
		//allocator.construct(node);
		return node;
	}

	void  releaseNode(node_type* node) {
		m_pool.push_back(node);	
	}

	void insert(hash_type hash , const K& key, const V& value) {
				
		size_type pos = hash % m_bucketSize;
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
	
	void insert(std::pair<node_type*, hash_type> &node) {

		assert(node.first->next == nullptr);

		size_type pos = node.second % m_bucketSize;

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
	

	bool updateIfExist(hash_type hash, const K &key, const V &value) {
		size_type pos = hash % m_bucketSize;
		auto *bucket = m_buckets[pos];
		
		if (bucket->active && bucket->node.key == key) {
			bucket->node.value = value;
			return true;
		}
		auto *node = bucket->node.next;

		while (node != nullptr) {
			if (node->key == key) {
				node->value = value;
				return true;
			}
			node = node->next;
		}
		
		return false;
	}
	

	template< typename K >
	typename std::enable_if< !std::is_integral< K >::value, hash_type >::type
		getHash(const K &key) const {
		hash_type h = key.getHash();
		return h;
	}

	template< typename K >
	typename std::enable_if< std::is_integral< K >::value, hash_type >::type
		getHash(const K &key) const {
		return key;
	}

	bool check(Bucket<K, V> *bucket) const {
		size_type tot = 0;
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
			auto *node = bucket->node.next;
			while (node != nullptr) {
				tot++;
				node = node->next;

				if (tot > bucket->count) {
					assert(true);
				}
			}
		}

		return bucket->count == tot;
	}

	void moveToFront(Bucket<K,V> *bucket) {
		
		if (!bucket->active && bucket->node.next) {
			auto *node = bucket->node.next;
			bucket->node.key = std::move(node->key);
			bucket->node.value = std::move(node->value);
			bucket->active = true;
			if (node->next) {
				bucket->node.next = node->next;
			}
			else {
				bucket->node.next = nullptr;
			}		
			releaseNode(node);
		}
	}

	

	void rehash() {
		
		m_buckets.resize(m_bucketSize * 2);

		for (size_type i = m_bucketSize; i < m_bucketSize * 2; i++) {
			m_buckets[i] = new Bucket<K, V>();
		}
		size_type old = m_bucketSize;
		m_bucketSize *= 2;

		std::vector< std::pair<node_type*, hash_type> > moveNodes;
		moveNodes.reserve(old);
		
		for (size_type i = 0; i < old; i++) {
			auto *bucket = m_buckets[i];
			if (bucket->count == 0) continue;

			if (bucket->active) {
				hash_type h = getHash(bucket->node.key);
				size_type pos = h % m_bucketSize;
				if (pos != i) {
					auto *node = getNode();				
					node->key = std::move(bucket->node.key);
					node->value = std::move(bucket->node.value);
					node->next = nullptr;
					bucket->active = false;
					bucket->count--;
					assert(bucket->count >= 0);
					moveNodes.emplace_back( node, h );
				}
			}
	
			auto *HOME = bucket->node.next;  	//points to bucket->node.next
			auto *node = bucket->node.next;
			auto *prev = bucket->node.next;		//points to previos active node

			while (node) {
								
				hash_type h = getHash(node->key);
				size_type pos = h % m_bucketSize;
				auto *next_node = node->next;
				if (pos != i) {
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
					moveNodes.emplace_back( node, h );
					
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
		hash_type h = getHash(key);
		size_type pos = h % m_bucketSize;

		auto *bucket = m_buckets[pos];
		if (bucket->active && bucket->node.key == key) {
			return bucket->node.value;
		}
		auto *node = bucket->node.next;

		while (node != nullptr) {
			if (node->key == key) {
				return node->value;
			}
			node = node->next;
		}

		throw std::runtime_error("not found");

	}

	iterator removeAtEx(hash_type hash, const K& key) {
		size_type pos = hash % m_bucketSize;
		auto *bucket = m_buckets[pos];

		if (bucket->active && bucket->node.key == key) {
			bucket->active = false;
			m_size--;
			bucket->count--;
			assert(bucket->count >= 0);

			moveToFront(bucket);	//check if we can move the next to the front

			return bucket->active ? createIterator(&bucket->node, pos) : createIterator(bucket->node.next, pos);

		}

		if (bucket->node.next) {
			auto *node = bucket->node.next;
			if (node->key == key) {
				auto *next_n = node->next;
				releaseNode(node);
				bucket->count--;
				assert(bucket->count >= 0);
				bucket->node.next = next_n;
				m_size--;

				return createIterator(next_n, pos);
			}
			auto *prev = node;
			node = bucket->node.next->next;
			while (node != nullptr) {

				if (node->key == key) {

					auto *next_n = node->next;
					prev->next = next_n;
					bucket->count--;
					assert(bucket->count >= 0);
					m_size--;
					releaseNode(node);

					return createIterator(next_n, pos);
				}

				prev = node;
				node = node->next;
			}
		}

		return end();
	}


	public:

				
		node_allocator& get_allocator() {
			return allocator;
		}


		
		iterator begin() const {
			return createIterator(nullptr,0,false);
		}
		iterator end() const {
			return iterator();
		}
		
		explicit HashTable(size_type bucketSize = 20) {
		
			m_pool.reserve(20);
			m_bucketSize =  std::max(bucketSize, (size_type)1);
			m_size = 0;
			m_buckets.reserve(m_bucketSize);
			for (size_type i = 0; i < m_bucketSize; i++) {
				m_buckets.push_back(new Bucket<K, V>());
			}
		}

		~HashTable()
		{
			destroy();
		};

		bool check() const{
			size_type tot = 0;
			for (size_type i = 0; i < m_bucketSize; i++) {
				if (!check(m_buckets[i])) return false;
				tot += m_buckets[i]->count;
			}

			return tot == m_size;
		}

		void destroy() {
			for (size_type i = 0; i < m_bucketSize; i++) {
				m_buckets[i]->removeNodes(allocator);
				delete m_buckets[i];
			}
			m_buckets.clear();
			m_bucketSize = 0;
			m_size = 0;

			while (!m_pool.empty()) {
				allocator.destroy(m_pool.back());
				allocator.deallocate(m_pool.back(),1);
				m_pool.pop_back();
			}

		}

		HashTable(HashTable &&other)
		{
			m_pool = std::move(other.m_pool);
			m_bucketSize = other.m_bucketSize;
			m_size = other.m_size;
			m_buckets = std::move(other.m_buckets);					
			other.m_bucketSize = 0;
			other.m_size = 0;
		}

		HashTable& operator=(HashTable &&other) {

			destroy();

			m_pool = std::move(other.m_pool);
			m_bucketSize = other.m_bucketSize;
			m_size = other.m_size;
			m_buckets = std::move(other.m_buckets);
			other.m_bucketSize = 0;
			other.m_size = 0;

			return *this;
		}

		HashTable& operator=(const HashTable &other) 
		{
			destroy();

			m_bucketSize = other.m_bucketSize;
			m_size = other.m_size;
			m_buckets.reserve(m_bucketSize);
			for (size_type i = 0; i < m_bucketSize; i++) {
				auto *b = other.m_buckets[i]->copy(this);
				m_buckets.push_back(b);
			}
			return *this;
		}

		HashTable(const HashTable &other)
		{		
			m_bucketSize = other.m_bucketSize;
			m_size = other.m_size;
			m_buckets.reserve(m_bucketSize);
			for (size_type i = 0; i < m_bucketSize; i++) {
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

			hash_type hash = getHash(key);

			if (updateIfExist(hash, key, value)) {
				return;
			}

			insert(hash, key, value);

			m_size++;

			if (m_size > 10) {
				//just a value so not cause rehash during simple unit tests
				float f = (1.0f*m_size) / m_bucketSize;
				if (f > 0.85) {
					rehash();
				}
			}
		}
		
	

		size_type size() const {
			return m_size;
		}

		size_type getBucketSize() const {
			return m_bucketSize;
		}
		

			
		iterator find(K const &key) const {
			hash_type h = getHash(key);
			size_type pos = h % m_bucketSize;

			auto *bucket = m_buckets[pos];			
			if (bucket->active && bucket->node.key == key) {
				return createIterator(&bucket->node,pos);				
			}
			auto *node = bucket->node.next;
			while (node != nullptr) {
				if (node->key == key) {
					return createIterator(node,pos);
				}

				node = node->next;
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
			size_type temp = m_size;
			hash_type h = getHash(key);
			removeAtEx(h, key);
			return temp > m_size;
		}



		iterator remove(const iterator &it) {
			if (it == end()) return end();
			hash_type h = getHash(it.m_node->key);
			return removeAtEx(h, it.m_node->key);
		}


		bool exist(K const &key) const {
			hash_type h = getHash(key);
			size_type pos = h % m_bucketSize;
			auto *bucket = m_buckets[pos];
			if (bucket->count == 0) return false;
			if (bucket->active && bucket->node.key == key) {
				return true;
			}
			auto *node = bucket->node.next;
			while (node != nullptr) {
				if (node->key == key) {
					return true;
				}
				node = node->next;
			}
			
			return false;
		}

		//clears and trims down the internal structure to bucketSize
		void clearAndTrim(size_type bucketSize) {
			
			clear();
			
			if (bucketSize > m_bucketSize) return;

			bucketSize = std::max(bucketSize, (size_type)1);

			for (size_type i = bucketSize; i < m_bucketSize; i++) {
				delete m_buckets[i];
			}
			
			m_buckets.resize(bucketSize);
			m_bucketSize = bucketSize;

			m_buckets.shrink_to_fit();
		}

		void clear() {

			for (size_type i = 0; i < m_bucketSize; i++) {
				auto *bucket = m_buckets[i];
				bucket->removeNodes(allocator);
			}			
			m_size = 0;

			while (!m_pool.empty()) {
				allocator.destroy(m_pool.back());
				allocator.deallocate(m_pool.back(), 1);
				m_pool.pop_back();
			}
			
		}



		private:

			Bucketvector m_buckets;

			size_type m_size;	 
			size_type m_bucketSize; 
										  
	};

	


