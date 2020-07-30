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
		using hash_type = std::size_t;
	private:
		template <typename K, typename V >
		struct Node
		{			
			K key = {};
			V value = {};
			hash_type hash = {};
			Node *next = {};
		};

		using node_type = Node<K, V>;
		using node_allocator = typename Alloc::template rebind<node_type>::other;
		node_allocator m_allocator;
		using reference = V & ;
		using const_reference = const V&;

		std::vector<node_type*, Alloc> m_pool;
	
		const double LOAD_FACTOR = 0.90;
		const int TRY_MOVE_NODE_COUNT = 3;		
	
		std::hash<std::string> m_stringHasher;

		static int setLast(node_type *node_next , node_type *node) {					

			int i = 1;
			auto *n = node_next;
			while (n != nullptr) {
				node_next = n;
				n = n->next;
				i++;
			}
			node_next->next = node;
			return i;
		}

		//copied from flat_hash_map by  Malte Skarupke 
		//faster modulo..
		struct prime_number_hash_policy
		{
			static size_t mod0(size_t) { return 0llu; }
			static size_t mod2(size_t hash) { return hash % 2llu; }
			static size_t mod3(size_t hash) { return hash % 3llu; }
			static size_t mod5(size_t hash) { return hash % 5llu; }
			static size_t mod7(size_t hash) { return hash % 7llu; }
			static size_t mod11(size_t hash) { return hash % 11llu; }
			static size_t mod13(size_t hash) { return hash % 13llu; }
			static size_t mod17(size_t hash) { return hash % 17llu; }
			static size_t mod23(size_t hash) { return hash % 23llu; }
			static size_t mod29(size_t hash) { return hash % 29llu; }
			static size_t mod37(size_t hash) { return hash % 37llu; }
			static size_t mod47(size_t hash) { return hash % 47llu; }
			static size_t mod59(size_t hash) { return hash % 59llu; }
			static size_t mod73(size_t hash) { return hash % 73llu; }
			static size_t mod97(size_t hash) { return hash % 97llu; }
			static size_t mod127(size_t hash) { return hash % 127llu; }
			static size_t mod151(size_t hash) { return hash % 151llu; }
			static size_t mod197(size_t hash) { return hash % 197llu; }
			static size_t mod251(size_t hash) { return hash % 251llu; }
			static size_t mod313(size_t hash) { return hash % 313llu; }
			static size_t mod397(size_t hash) { return hash % 397llu; }
			static size_t mod499(size_t hash) { return hash % 499llu; }
			static size_t mod631(size_t hash) { return hash % 631llu; }
			static size_t mod797(size_t hash) { return hash % 797llu; }
			static size_t mod1009(size_t hash) { return hash % 1009llu; }
			static size_t mod1259(size_t hash) { return hash % 1259llu; }
			static size_t mod1597(size_t hash) { return hash % 1597llu; }
			static size_t mod2011(size_t hash) { return hash % 2011llu; }
			static size_t mod2539(size_t hash) { return hash % 2539llu; }
			static size_t mod3203(size_t hash) { return hash % 3203llu; }
			static size_t mod4027(size_t hash) { return hash % 4027llu; }
			static size_t mod5087(size_t hash) { return hash % 5087llu; }
			static size_t mod6421(size_t hash) { return hash % 6421llu; }
			static size_t mod8089(size_t hash) { return hash % 8089llu; }
			static size_t mod10193(size_t hash) { return hash % 10193llu; }
			static size_t mod12853(size_t hash) { return hash % 12853llu; }
			static size_t mod16193(size_t hash) { return hash % 16193llu; }
			static size_t mod20399(size_t hash) { return hash % 20399llu; }
			static size_t mod25717(size_t hash) { return hash % 25717llu; }
			static size_t mod32401(size_t hash) { return hash % 32401llu; }
			static size_t mod40823(size_t hash) { return hash % 40823llu; }
			static size_t mod51437(size_t hash) { return hash % 51437llu; }
			static size_t mod64811(size_t hash) { return hash % 64811llu; }
			static size_t mod81649(size_t hash) { return hash % 81649llu; }
			static size_t mod102877(size_t hash) { return hash % 102877llu; }
			static size_t mod129607(size_t hash) { return hash % 129607llu; }
			static size_t mod163307(size_t hash) { return hash % 163307llu; }
			static size_t mod205759(size_t hash) { return hash % 205759llu; }
			static size_t mod259229(size_t hash) { return hash % 259229llu; }
			static size_t mod326617(size_t hash) { return hash % 326617llu; }
			static size_t mod411527(size_t hash) { return hash % 411527llu; }
			static size_t mod518509(size_t hash) { return hash % 518509llu; }
			static size_t mod653267(size_t hash) { return hash % 653267llu; }
			static size_t mod823117(size_t hash) { return hash % 823117llu; }
			static size_t mod1037059(size_t hash) { return hash % 1037059llu; }
			static size_t mod1306601(size_t hash) { return hash % 1306601llu; }
			static size_t mod1646237(size_t hash) { return hash % 1646237llu; }
			static size_t mod2074129(size_t hash) { return hash % 2074129llu; }
			static size_t mod2613229(size_t hash) { return hash % 2613229llu; }
			static size_t mod3292489(size_t hash) { return hash % 3292489llu; }
			static size_t mod4148279(size_t hash) { return hash % 4148279llu; }
			static size_t mod5226491(size_t hash) { return hash % 5226491llu; }
			static size_t mod6584983(size_t hash) { return hash % 6584983llu; }
			static size_t mod8296553(size_t hash) { return hash % 8296553llu; }
			static size_t mod10453007(size_t hash) { return hash % 10453007llu; }
			static size_t mod13169977(size_t hash) { return hash % 13169977llu; }
			static size_t mod16593127(size_t hash) { return hash % 16593127llu; }
			static size_t mod20906033(size_t hash) { return hash % 20906033llu; }
			static size_t mod26339969(size_t hash) { return hash % 26339969llu; }
			static size_t mod33186281(size_t hash) { return hash % 33186281llu; }
			static size_t mod41812097(size_t hash) { return hash % 41812097llu; }
			static size_t mod52679969(size_t hash) { return hash % 52679969llu; }
			static size_t mod66372617(size_t hash) { return hash % 66372617llu; }
			static size_t mod83624237(size_t hash) { return hash % 83624237llu; }
			static size_t mod105359939(size_t hash) { return hash % 105359939llu; }
			static size_t mod132745199(size_t hash) { return hash % 132745199llu; }
			static size_t mod167248483(size_t hash) { return hash % 167248483llu; }
			static size_t mod210719881(size_t hash) { return hash % 210719881llu; }
			static size_t mod265490441(size_t hash) { return hash % 265490441llu; }
			static size_t mod334496971(size_t hash) { return hash % 334496971llu; }
			static size_t mod421439783(size_t hash) { return hash % 421439783llu; }
			static size_t mod530980861(size_t hash) { return hash % 530980861llu; }
			static size_t mod668993977(size_t hash) { return hash % 668993977llu; }
			static size_t mod842879579(size_t hash) { return hash % 842879579llu; }
			static size_t mod1061961721(size_t hash) { return hash % 1061961721llu; }
			static size_t mod1337987929(size_t hash) { return hash % 1337987929llu; }
			static size_t mod1685759167(size_t hash) { return hash % 1685759167llu; }
			static size_t mod2123923447(size_t hash) { return hash % 2123923447llu; }
			static size_t mod2675975881(size_t hash) { return hash % 2675975881llu; }
			static size_t mod3371518343(size_t hash) { return hash % 3371518343llu; }
			static size_t mod4247846927(size_t hash) { return hash % 4247846927llu; }
			static size_t mod5351951779(size_t hash) { return hash % 5351951779llu; }
			static size_t mod6743036717(size_t hash) { return hash % 6743036717llu; }
			static size_t mod8495693897(size_t hash) { return hash % 8495693897llu; }
			static size_t mod10703903591(size_t hash) { return hash % 10703903591llu; }
			static size_t mod13486073473(size_t hash) { return hash % 13486073473llu; }
			static size_t mod16991387857(size_t hash) { return hash % 16991387857llu; }
			static size_t mod21407807219(size_t hash) { return hash % 21407807219llu; }
			static size_t mod26972146961(size_t hash) { return hash % 26972146961llu; }
			static size_t mod33982775741(size_t hash) { return hash % 33982775741llu; }
			static size_t mod42815614441(size_t hash) { return hash % 42815614441llu; }
			static size_t mod53944293929(size_t hash) { return hash % 53944293929llu; }
			using mod_function = size_t(*)(size_t);

			mod_function next_size_over(size_t & size) const
			{
				
				static constexpr const size_t prime_list[] =
				{
					2llu, 3llu, 5llu, 7llu, 11llu, 13llu, 17llu, 23llu, 29llu, 37llu, 47llu,
					59llu, 73llu, 97llu, 127llu, 151llu, 197llu, 251llu, 313llu, 397llu,
					499llu, 631llu, 797llu, 1009llu, 1259llu, 1597llu, 2011llu, 2539llu,
					3203llu, 4027llu, 5087llu, 6421llu, 8089llu, 10193llu, 12853llu, 16193llu,
					20399llu, 25717llu, 32401llu, 40823llu, 51437llu, 64811llu, 81649llu,
					102877llu, 129607llu, 163307llu, 205759llu, 259229llu, 326617llu,
					411527llu, 518509llu, 653267llu, 823117llu, 1037059llu, 1306601llu,
					1646237llu, 2074129llu, 2613229llu, 3292489llu, 4148279llu, 5226491llu,
					6584983llu, 8296553llu, 10453007llu, 13169977llu, 16593127llu, 20906033llu
				};

				static constexpr size_t(*const mod_functions[])(size_t) =
				{
					&mod0, &mod2, &mod3, &mod5, &mod7, &mod11, &mod13, &mod17, &mod23, &mod29, &mod37,
					&mod47, &mod59, &mod73, &mod97, &mod127, &mod151, &mod197, &mod251, &mod313, &mod397,
					&mod499, &mod631, &mod797, &mod1009, &mod1259, &mod1597, &mod2011, &mod2539, &mod3203,
					&mod4027, &mod5087, &mod6421, &mod8089, &mod10193, &mod12853, &mod16193, &mod20399,
					&mod25717, &mod32401, &mod40823, &mod51437, &mod64811, &mod81649, &mod102877,
					&mod129607, &mod163307, &mod205759, &mod259229, &mod326617, &mod411527, &mod518509,
					&mod653267, &mod823117, &mod1037059, &mod1306601, &mod1646237, &mod2074129,
					&mod2613229, &mod3292489, &mod4148279, &mod5226491, &mod6584983, &mod8296553,
					&mod10453007, &mod13169977, &mod16593127, &mod20906033, &mod26339969, &mod33186281,
					&mod41812097, &mod52679969, &mod66372617, &mod83624237, &mod105359939, &mod132745199,
					&mod167248483, &mod210719881, &mod265490441, &mod334496971, &mod421439783,
					&mod530980861, &mod668993977, &mod842879579, &mod1061961721, &mod1337987929,
					&mod1685759167, &mod2123923447, &mod2675975881, &mod3371518343, &mod4247846927,
					&mod5351951779, &mod6743036717, &mod8495693897, &mod10703903591, &mod13486073473,
					&mod16991387857, &mod21407807219, &mod26972146961, &mod33982775741, &mod42815614441,
					&mod53944293929
				};
				const size_t * found = std::lower_bound(std::begin(prime_list), std::end(prime_list) - 1, size);
				size = *found;
				return mod_functions[1 + found - prime_list];
			}
				void commit(mod_function new_mod_function)
				{
					current_mod_function = new_mod_function;
				}
				void reset()
				{
					current_mod_function = &mod0;
				}

				size_t index_for_hash(size_t hash) const
				{
					return current_mod_function(hash);
				}
				size_t keep_in_range(size_t index, size_t num_slots_minus_one) const
				{
					return index > num_slots_minus_one ? current_mod_function(index) : index;
				}

			private:
				mod_function current_mod_function = &mod0;
		};
	
		size_type calcPos(hash_type hash) const {

			return hash_policy.index_for_hash(hash);
			//return hash % m_bucketSize ;		
		}

		prime_number_hash_policy hash_policy;

		template <typename K, typename V >
		struct Bucket {

			Bucket() {			
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
			
			}


			Bucket *copy(HashTable *home) const {
				Bucket *bucket = new Bucket<K, V>();

				auto *this_bucket_next = this->node.next;

				bucket->active = this->active;
				if (bucket->active) {
					bucket->node.key = this->node.key;
					bucket->node.value = this->node.value;
					bucket->node.hash  = this->node.hash;
				}

				

				while (this_bucket_next != nullptr) {

					auto *node = home->getNode();
					node->key = this_bucket_next->key;
					node->value = this_bucket_next->value;
					node->hash = this_bucket_next->hash;
					node->next = nullptr;
				

					this_bucket_next = this_bucket_next->next;

					if (bucket->node.next == nullptr) {
						bucket->node.next = node;
					}
					else {
						//find end
						setLast(bucket->node.next, node);
					}

				}
		
				return bucket;
			}
					
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
					if (m_node == &bucket->node) 
					{
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

			auto *node = m_allocator.allocate(1);
			//allocator.construct(node);
			return node;
		}
		
	
		void  releaseNode(node_type* node) {
			m_pool.push_back(node);
		}

	

		template <typename V>
		int insertEmplace(size_type pos, hash_type hash, const K& key, V &&value) {

			auto *bucket = m_buckets[pos];
			if (!bucket->active) {

				bucket->node.key = key;
				bucket->node.value = std::move(value);
				bucket->node.hash = hash;
				bucket->active = true;	
			}
			else {			
				auto *node = getNode();				
				node->key = key;
				node->value = std::move(value);
				node->hash = hash;
				node->next = nullptr;
				if (bucket->node.next == nullptr) {
					bucket->node.next = node;
					return 1;
				}
				int t = setLast(bucket->node.next, node);

				if (t >= TRY_MOVE_NODE_COUNT) {		//many items in this bucket , see if next one is empty
		
					if (pos + 1 < m_bucketSize) {
						auto *bucketNext = m_buckets[pos + 1];
						if (!bucketNext->active) {			
							removeAtEx(hash, key);											
							m_size++;						
							bucketNext->node.key = key;
							bucketNext->node.value = std::move(value);
							bucketNext->node.hash = hash;
							bucketNext->active = true;
							return t - 1;
						}					
					}								
				}
				return t;
			}

			return 1;
		}

		void insert(std::pair<node_type*, size_type > &node) {

			assert(node.first->next == nullptr);
					

			auto *bucket = m_buckets[node.second];
			if (!bucket->active) {
				bucket->node.key = std::move(node.first->key);
				bucket->node.value = std::move(node.first->value);
				bucket->node.hash = node.first->hash;		
				bucket->active = true;
				releaseNode(node.first);			
			}
			else {
				if (bucket->node.next == nullptr) {
					bucket->node.next = node.first;
					return;
				}
				//find end
				setLast(bucket->node.next, node.first);
			

			}
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


	

		void moveToFront(Bucket<K, V> *bucket) {

			if (!bucket->active && bucket->node.next) {
				auto *node = bucket->node.next;
				bucket->node.key = std::move(node->key);
				bucket->node.value = std::move(node->value);
				bucket->node.hash = node->hash;
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
		
			size_type newsize = m_bucketSize * 2;

			size_t sizeFromPrim = newsize;
			

			//get a size 
			auto new_prime_index = hash_policy.next_size_over(sizeFromPrim);
			newsize = sizeFromPrim;
			
			hash_policy.commit(new_prime_index);

			m_buckets.resize(newsize);

			for (size_type i = m_bucketSize; i < newsize; i++) {
				m_buckets[i] = new Bucket<K, V>();
			}
			size_type old = m_bucketSize;
			m_bucketSize = newsize;

			std::vector< std::pair<node_type*, size_type> > moveNodes;
			moveNodes.reserve(old);

			for (size_type i = 0; i < old; i++) {
				auto *bucket = m_buckets[i];
			
				auto *node = bucket->node.next;

			

				if (bucket->active) {			
					size_type newPos = calcPos(bucket->node.hash);
					if (newPos != i) {
						auto *newnode = getNode();
						newnode->key = std::move(bucket->node.key);
						newnode->value = std::move(bucket->node.value);
						newnode->hash = bucket->node.hash;
						newnode->next = nullptr;
						bucket->active = false;
					
						moveNodes.emplace_back(newnode, newPos);
					}
				}

				auto *HOME = bucket->node.next;  	//points to bucket->node.next				
				auto *prev = bucket->node.next;		//points to previos active node
			
				while (node) {
								
					size_type newPos = calcPos(node->hash);
					auto *next_node = node->next;
					if (newPos != i) {
						//remove the node from this bucket
						if (node == HOME) {
							bucket->node.next = next_node;
							HOME = bucket->node.next;
						}
						else {
							prev->next = next_node;
						}
					
						node->next = nullptr;
						moveNodes.emplace_back(node, newPos);

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
			hash_type hash = getHash(key);
			size_type pos = calcPos(hash);

			auto *bucket = m_buckets[pos];
			if (bucket->active && bucket->node.hash == hash && bucket->node.key == key) {
				return bucket->node.value;
			}
			auto *node = bucket->node.next;

			while (node != nullptr) {
				if (node->hash == hash && node->key == key) {
					return node->value;
				}
				node = node->next;
			}

			if (pos + 1 < m_bucketSize) {
				auto *bucketNext = m_buckets[pos + 1];
				if (bucketNext->active && bucketNext->node.hash == hash && bucketNext->node.key == key) {
					return bucketNext->node.value;

				}

			}
				

			throw std::runtime_error("not found");

		}

		iterator removeAtEx(hash_type hash, const K& key) {
			size_type pos = calcPos(hash);
			auto *bucket = m_buckets[pos];

			if (bucket->active && bucket->node.hash == hash && bucket->node.key == key) {
				bucket->active = false;
				m_size--;				
				moveToFront(bucket);	//check if we can move the next to the front
				
			
				return bucket->active ? createIterator(&bucket->node, pos) : createIterator(bucket->node.next, pos);

			}

			if (bucket->node.next) {
				auto *node = bucket->node.next;
				if (node->hash == hash  && node->key==key) {
					auto *next_n = node->next;
					releaseNode(node);			
					bucket->node.next = next_n;
					m_size--;

					return createIterator(next_n, pos);
				}
				auto *prev = node;
				node = bucket->node.next->next;
				while (node != nullptr) {

					if (node->hash == hash && node->key == key) {

						auto *next_n = node->next;
						prev->next = next_n;					
						m_size--;
						releaseNode(node);

						return createIterator(next_n, pos);
					}

					prev = node;
					node = node->next;
				}
			}

			if (pos + 1 < m_bucketSize) {
				auto *bucketNext = m_buckets[pos + 1];
				if (bucketNext->active && bucketNext->node.hash == hash && bucketNext->node.key == key) {
					bucketNext->active = false;
					m_size--;
					moveToFront(bucketNext);	//check if we can move the next to the front

				
					return bucketNext->active ? createIterator(&bucketNext->node, pos) : createIterator(bucketNext->node.next, pos);

				}

			}

			return end();
		}


	public:


		node_allocator & get_allocator() {
			return m_allocator;
		}

		

		iterator begin() const {
			return createIterator(nullptr, 0, false);
		}
		iterator end() const {
			return m_end;
		}

		explicit HashTable(size_type bucketSize = 13) {
			
			size_t t = bucketSize;
			auto new_prime_index = hash_policy.next_size_over(t);
			hash_policy.commit(new_prime_index);

			bucketSize = t;
	

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
			//not used
			return true;
		}

		

		void destroy() {
			for (size_type i = 0; i < m_bucketSize; i++) {
				m_buckets[i]->removeNodes(m_allocator);
				delete m_buckets[i];
			}
			m_buckets.clear();
			m_bucketSize = 0;
			m_size = 0;
		
			while (!m_pool.empty()) {
				m_allocator.destroy(m_pool.back());
				m_allocator.deallocate(m_pool.back(), 1);
				m_pool.pop_back();
			}

		}

		HashTable(HashTable &&other)
		{
			m_pool = std::move(other.m_pool);
			m_bucketSize = std::move(other.m_bucketSize);
			m_size = std::move(other.m_size);
			m_buckets = std::move(other.m_buckets);
			
			hash_policy = std::move(other.hash_policy);
			other.m_bucketSize = 0;
			other.m_size = 0;
		}

		HashTable& operator=(HashTable &&other) {

			destroy();

			m_pool = std::move(other.m_pool);
			m_bucketSize = std::move(other.m_bucketSize);
			m_size = std::move(other.m_size);
			m_buckets = std::move(other.m_buckets);

			hash_policy = std::move(other.hash_policy);
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
		
			hash_policy = other.hash_policy;
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
			hash_policy = other.hash_policy;
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

		template <typename V>
		void emplace(const K& key, V&& value, bool update) {

			hash_type hash = getHash(key);
		
			size_type pos = calcPos(hash);

			if (update) {
				//check if exist
				auto *bucket = m_buckets[pos];

				//update if exist
				if (bucket->active && bucket->node.hash == hash && bucket->node.key == key) {
					bucket->node.value = std::move(value);
					return;
				}
				auto *node = bucket->node.next;

				while (node != nullptr) {
					if (node->hash == hash && node->key == key) {
						node->value = std::move(value);
						return;
					}
					node = node->next;
				}

				if (pos + 1 < m_bucketSize) {
					auto *bucketNext = m_buckets[pos + 1];
					if (bucketNext->active && bucketNext->node.hash == hash && bucketNext->node.key == key) {
						bucketNext->node.value = std::move(value);
						return;
					}

				}

			}

			int count = insertEmplace(pos, hash, key, std::move(value));

			m_size++;

			float f = (1.0f*m_size) / m_bucketSize;
			if (f > LOAD_FACTOR) {
			
				rehash();
				
			}
		
		}

		// will check if key exist before
		void add(const K& key, const V &value) {
			emplace(key, value, true);
		}
		
		void add(const K& key, V &&value) {
			emplace(key, std::move(value), true);		
		}

		//use if you know key is unique
		void insert(const K& key, const V &value) {
			emplace(key, value, false);
		}

		void insert(const K& key, V &&value) {
			emplace(key, std::move(value) , false);
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
			if (bucket->active && bucket->node.hash == hash && bucket->node.key == key) {
				return createIterator(&bucket->node, pos);
			}
			auto *node = bucket->node.next;
			while (node != nullptr) {
				if (node->hash == hash && node->key == key) {
					return createIterator(node, pos);
				}

				node = node->next;
			}


			if (pos + 1 < m_bucketSize) {
				auto *bucketNext = m_buckets[pos + 1];
				if (bucketNext->active && bucketNext->node.hash == hash && bucketNext->node.key == key) {
					return createIterator(node, pos);
				}

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
			if (bucket->active && bucket->node.hash == hash && bucket->node.key == key) {
				return true;
			}
			auto *node = bucket->node.next;
			while (node != nullptr) {
				if (node->hash == hash && node->key == key) {
					return true;
				}
				node = node->next;
			}


			if (pos + 1 < m_bucketSize) {
				auto *bucketNext = m_buckets[pos + 1];
				if (bucketNext->active && bucketNext->node.hash == hash && bucketNext->node.key == key) {
					return true;
				}

			}


			return false;
		}

		//clears and trims down the internal structure 
		void clearAndTrim() {

			clear();
						
			size_type bucketSize = m_bucketSize / 2;
			
			size_t sizeFromPrim = bucketSize;

			auto new_prime_index = hash_policy.next_size_over(sizeFromPrim);
		
			bucketSize = sizeFromPrim;
			hash_policy.commit(new_prime_index);

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
				bucket->removeNodes(m_allocator);
			}
			m_size = 0;

			while (!m_pool.empty()) {
				m_allocator.destroy(m_pool.back());
				m_allocator.deallocate(m_pool.back(), 1);
				m_pool.pop_back();
			}

		}



	private:

		bucket_vector m_buckets;
		iterator	m_end;		//end() iterator 
		size_type m_size;
		size_type m_bucketSize;
	
	};


}

