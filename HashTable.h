#pragma once


#include <vector>
#include <functional>

namespace stml {

	template <typename K, typename V,
		typename Alloc = std::allocator<V>>
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
		using node_type = Node<K, V>;
		using node_allocator = typename Alloc::template rebind<node_type>::other;
		node_allocator allocator;
		using reference = V & ;
		using const_reference = const V&;

		using hash_type = std::size_t;

		std::vector<node_type*, Alloc> m_pool;

		const int MAX_ITEMS_IN_BUCKET = 3;			
		const float LOAD_FACTOR = 0.85;				

		const std::vector<unsigned int> m_bucketSizes = { 4, 16, 64, 128, 512, 1024, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152 };
		//const std::vector<unsigned int> m_bucketSizes = { 3, 13, 97, 311, 719, 1931,7793, 19391, 37199, 99371, 193939, 500299, 700577, 1300463, 1801529, 3000251 };
		unsigned int m_currentBucketSizeIndex = 1;		//default 16
		std::hash<std::string> m_stringHasher;

		static void setLast(node_type *node_next , node_type *node) {					

			auto *n = node_next;
			while (n != nullptr) {
				node_next = n;
				n = n->next;
			}
			node_next->next = node;
		}
		
	
		size_type calcPos(hash_type hash) const {
			return hash & m_bucketSize -1;
		}

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

				unsigned short nr_of_nodes = bucket->active ? 1 : 0;

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
						setLast(bucket->node.next, node);
					}

				}
				assert(this->count == nr_of_nodes);

				bucket->count = this->count;
				return bucket;
			}

			unsigned short count;		//nr of elements in this bucket. Not really used and can be removed
			bool active;		   // flag if the first node is used ( it is not in the linked list)
			Node<K, V> node;

		};

		using bucket_vector = std::vector<Bucket<K, V>*, Alloc>;



	public:



		class iterator_base {

		public:

			using iterator_category = std::forward_iterator_tag;
			using value_type = void;
			using difference_type = size_type;
			using pointer = void;
			using reference = void;
		protected:
			friend class HashTable;

			node_type* findNext() const {

				if (m_lastBucket >= m_buckets->size()) {
					return nullptr;
				}

				auto *bucket = (*m_buckets)[m_lastBucket];
				if (!m_checkedFirst) {
					if (bucket->active) {
						m_checkedFirst = true;
						return &bucket->node;
					}

					m_checkedFirst = true;
					m_node = nullptr;
				}

				auto *node = bucket->node.next;
				if (node == nullptr) {
					m_checkedFirst = false;
					m_lastBucket++;
					return findNext();
				}

				if (m_checkedFirst) {
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
					m_checkedFirst = false;
					return findNext();
				}
				return m_node;

			}

			//end iterator
			iterator_base() :
				m_buckets(nullptr),
				m_lastBucket(0),
				m_node(nullptr),
				m_size(0),
				m_count(0)
			{
			}

			iterator_base(node_type *n,
				const bucket_vector *ptr,
				size_type pos,
				size_type tot,
				bool checkedFirst) :
				m_buckets(ptr),
				m_lastBucket(pos),
				m_node(n),
				m_size(tot),
				m_count(0),
				m_checkedFirst(checkedFirst)
			{
				if (m_node == nullptr)
					m_node = findNext();
			}


		public:
					

			bool operator != (const iterator_base &other) const {
				return m_node != other.m_node;
			}
			bool operator == (const iterator_base &other) const {
				return m_node == other.m_node;
			}
	
						
		protected:
			const bucket_vector *m_buckets;
			mutable size_type m_lastBucket;
			size_type m_size;
			size_type m_count;
			mutable node_type *m_node;
			mutable bool m_checkedFirst;	//first item in every bucket works different than the rest so a boolean is needed

		};

		class iterator : public iterator_base {
			
			friend class HashTable;

			iterator(){}
			iterator(node_type *n,
				const bucket_vector *ptr,
				size_type pos,
				size_type tot,
				bool checkedFirst):iterator_base(n,ptr,pos,tot,checkedFirst){

			}

		public:
			std::pair<const K&, V&> operator*() const { return { m_node->key,m_node->value }; }

			//pre
			iterator & operator++() {
				if (++m_count >= m_size)
					m_node = nullptr;
				else
					m_node = findNext();

				return *this;
			}

			//post
			iterator operator++(int) {

				iterator it(m_node, m_buckets, m_lastBucket, m_size, m_checkedFirst);
				if (++m_count >= m_size)
					m_node = nullptr;
				else
					m_node = findNext();

				return it;
			}

		};
	
		iterator createIterator(node_type *node, size_type pos, bool skipFirst = true) const {
			return iterator(node, &m_buckets, pos, m_size, skipFirst);
		}

		iterator createEndIterator() const {
			return iterator();
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

		void insert(size_type pos, const K& key, const V& value) {

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
				setLast(bucket->node.next, node);		


				if (bucket->count >= MAX_ITEMS_IN_BUCKET) {
					m_bucketFull = true;
				}
			}			
			
		}


		void insertMove(size_type pos,const K& key, V &&value) {

			auto *bucket = m_buckets[pos];
			if (!bucket->active || bucket->count == 0) {

				bucket->node.key = key;
				bucket->node.value = std::move(value);
				bucket->count++;
				bucket->active = true;
			}
			else {
				bucket->count++;
				auto *node = getNode();

				node->key = key;
				node->value = std::move(value);
				node->next = nullptr;
				if (bucket->node.next == nullptr) {
					bucket->node.next = node;
					return;
				}
				setLast(bucket->node.next, node);


				if (bucket->count >= MAX_ITEMS_IN_BUCKET) {
					m_bucketFull = true;
				}
			}

		}

		void insert(std::pair<node_type*, hash_type> &node) {

			assert(node.first->next == nullptr);

			size_type pos = calcPos(node.second);

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
				setLast(bucket->node.next, node.first);
			

			}
		}


		bool updateIfExist(hash_type hash, const K &key, const V &value, size_type &pos) {
			pos = calcPos(hash);
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

		bool updateIfExistMove(hash_type hash, const K &key, V &&value, size_type &pos) {

			pos = calcPos(hash);

			auto *bucket = m_buckets[pos];

			if (bucket->active && bucket->node.key == key) {
				bucket->node.value = std::move(value);
				return true;
			}
			auto *node = bucket->node.next;

			while (node != nullptr) {
				if (node->key == key) {
					node->value = std::move(value);
					return true;
				}
				node = node->next;
			}

			return false;
		}



		template< typename K >
		typename std::enable_if< !std::is_integral< K >::value &&  !std::is_pointer< K >::value, hash_type>::type
			getHash(const K &key) const {
			hash_type h = key.getHash();
			return h;
		}

		template< typename K >
		typename std::enable_if< !std::is_integral< K >::value && std::is_pointer< K >::value, hash_type>::type
			getHash(const K &key) const {
			hash_type h = key->getHash();
			return h;
		}

		
		template< typename K >
		typename std::enable_if< std::is_integral< K >::value, hash_type >::type
			getHash(const K &key) const {
			return key;
		}
		//string overloading
		hash_type getHash(const std::string &key) const {
			return m_stringHasher(key);
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

		void moveToFront(Bucket<K, V> *bucket) {

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
			
			size_type newsize = 0;;
			if (m_currentBucketSizeIndex == m_bucketSizes.size() -1) {
				newsize = m_bucketSize * 2;			//TODO : add more to m_bucketSizes
			}
			else {
				newsize = m_bucketSizes[++m_currentBucketSizeIndex];
			}
			
			m_buckets.resize(newsize);

			for (size_type i = m_bucketSize; i < newsize; i++) {
				m_buckets[i] = new Bucket<K, V>();
			}
			size_type old = m_bucketSize;
			m_bucketSize = newsize;

			std::vector< std::pair<node_type*, hash_type> > moveNodes;
			moveNodes.reserve(old);

			for (size_type i = 0; i < old; i++) {
				auto *bucket = m_buckets[i];
				if (bucket->count == 0) continue;

				if (bucket->active) {
					hash_type hash = getHash(bucket->node.key);
					size_type pos = calcPos(hash);
					if (pos != i) {
						auto *node = getNode();
						node->key = std::move(bucket->node.key);
						node->value = std::move(bucket->node.value);
						node->next = nullptr;
						bucket->active = false;
						bucket->count--;
						assert(bucket->count >= 0);
						moveNodes.emplace_back(node, hash);
					}
				}

				auto *HOME = bucket->node.next;  	//points to bucket->node.next
				auto *node = bucket->node.next;
				auto *prev = bucket->node.next;		//points to previos active node

				while (node) {

					hash_type hash = getHash(node->key);
					size_type pos = calcPos(hash);
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
						moveNodes.emplace_back(node, hash);

					}
					else {
						prev = node;
					}

					node = next_node;

				}

				moveToFront(bucket);

			}
			m_bucketFull = false;

			for (auto &n : moveNodes) {
				insert(n);
			}

		}

		const_reference	at(K const &key) const {
			hash_type hash = getHash(key);
			size_type pos = calcPos(hash);

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
			size_type pos = calcPos(hash);
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


		node_allocator & get_allocator() {
			return allocator;
		}

		

		iterator begin() const {
			return createIterator(nullptr, 0, false);
		}
		iterator end() const {
			return m_end;
		}

		explicit HashTable(size_type bucketSize = 16) {
			if (bucketSize != 16) {
				if (bucketSize <= m_bucketSizes[0]) {
					bucketSize = m_bucketSizes[0];
					m_currentBucketSizeIndex = 0;
				}
				else {
					unsigned int i = 1;
					for (unsigned int p : m_bucketSizes) {
						if (bucketSize <= m_bucketSizes[i]) {
							bucketSize = m_bucketSizes[i];
							m_currentBucketSizeIndex = i;
							break;
						}
						i++;
					}
				}

			}
		
			m_bucketFull = false;
			m_pool.reserve(20);
			m_bucketSize = bucketSize;
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

		bool check() const {
			size_type tot = 0;
			for (size_type i = 0; i < m_bucketSize; i++) {
				if (!check(m_buckets[i])) return false;
				tot += m_buckets[i]->count;
			}

			return tot == m_size;
		}

		std::tuple<size_type, size_type, size_type> bucketInfo() const {
			size_type empty = 0;
			size_type one = 0;
			size_type more = 0;
			for (size_type i = 0; i < m_bucketSize; i++) {
				if (m_buckets[i]->count == 0) {
					empty++;
				}
				else if (m_buckets[i]->count == 1) {
					one++;
				}
				else {
					more++;
				}
				
			}
			return { empty,one,more };
			
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
				allocator.deallocate(m_pool.back(), 1);
				m_pool.pop_back();
			}

		}

		HashTable(HashTable &&other)
		{
			m_pool = std::move(other.m_pool);
			m_bucketSize = other.m_bucketSize;
			m_size = other.m_size;
			m_buckets = std::move(other.m_buckets);
			m_currentBucketSizeIndex = other.m_currentBucketSizeIndex;
			other.m_bucketSize = 0;
			other.m_size = 0;
		}

		HashTable& operator=(HashTable &&other) {

			destroy();

			m_pool = std::move(other.m_pool);
			m_bucketSize = other.m_bucketSize;
			m_size = other.m_size;
			m_buckets = std::move(other.m_buckets);
			m_currentBucketSizeIndex = other.m_currentBucketSizeIndex;
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
			m_currentBucketSizeIndex = other.m_currentBucketSizeIndex;

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
			m_currentBucketSizeIndex = other.m_currentBucketSizeIndex;
			for (size_type i = 0; i < m_bucketSize; i++) {
				auto *b = other.m_buckets[i]->copy(this);
				m_buckets.push_back(b);
			}

		}

		reference operator[](const K &key) {

			if (exist(key)) {
				return get(key);
			}
			else {
				V v{};
				add(key, v);
				return get(key);
			}
		}

		const_reference operator[](const K &key) const {
			return get(key);
		}


		void add(const K& key, const V& value) {

			hash_type hash = getHash(key);
			size_type pos;

			if (updateIfExist(hash,key,value,pos)) {
				return;
			}

			insert(pos, key, value);

			m_size++;

			if (m_size > 20  && m_bucketFull) {
			
				float f = (1.0f*m_size) / m_bucketSize;
				if (f > LOAD_FACTOR) {

					//rehash when loadfactor is over 85 % and atleast one bucket has x elements
					rehash();
				}
			}
		}
		
		void add(const K& key, V &&value) {

			hash_type hash = getHash(key);

			size_type pos;
			if (updateIfExistMove(hash, key, std::move(value), pos)) {
				return;
			}

			insertMove(pos, key, std::move(value));

			m_size++;

			if (m_size > 20 && m_bucketFull) {

				float f = (1.0f*m_size) / m_bucketSize;
				if (f > LOAD_FACTOR) {

					//rehash when loadfactor is over 85 % and atleast one bucket has x elements
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
			hash_type hash = getHash(key);
			size_type pos = calcPos(hash);

			auto *bucket = m_buckets[pos];
			if (bucket->active && bucket->node.key == key) {
				return createIterator(&bucket->node, pos);
			}
			auto *node = bucket->node.next;
			while (node != nullptr) {
				if (node->key == key) {
					return createIterator(node, pos);
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
			hash_type hash = getHash(key);
			removeAtEx(hash, key);
			return temp > m_size;
		}



		iterator remove(const iterator &it) {
			if (it == end()) return end();
			hash_type hash = getHash(it.m_node->key);
			return removeAtEx(hash, it.m_node->key);
		}


		bool exist(K const &key) const {
			hash_type hash = getHash(key);
			size_type pos = calcPos(hash);
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

		//clears and trims down the internal structure 
		void clearAndTrim() {

			clear();
			
		
			m_currentBucketSizeIndex = m_currentBucketSizeIndex / 2;
						
			size_type bucketSize = m_bucketSizes[m_currentBucketSizeIndex];

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

		bucket_vector m_buckets;
		iterator	m_end;		//end() iterator 
		size_type m_size;
		size_type m_bucketSize;
		bool	  m_bucketFull;
	};


}

