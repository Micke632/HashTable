#include "pch.h"
#include "..\hashtable\hashtable.h"
#include <memory>
#include <stdlib.h> 
#include <chrono>
#include <iostream>
#include <ctime>
#include <iterator>
#include <functional>
#include <numeric>

using namespace stml;
class XXX
{
	int hash;
public:

	explicit XXX(int hash) {
		this->hash = hash;
	}
	int getHash() const {
		return this->hash;
	}
	XXX(const XXX &other) {
		this->hash = other.hash;
	}
	XXX(XXX &&other) = default;
	XXX& operator=(XXX&&other) = default;
	XXX& operator=(const XXX&other) {
		this->hash = other.hash;
		return *this;
	}
	
	bool operator==(const XXX &v) {
		return this->hash == v.hash;
	}
};

TEST(HashMap, Basic) {
	HashTable<XXX, int> h(1);

	EXPECT_EQ(3, h.getBucketSize());

	EXPECT_TRUE(h.check());

	EXPECT_EQ(0, h.size());
	h.clear();
	EXPECT_EQ(0, h.size());
	h.add(XXX(1), 1);
	h.add(XXX(2), 2);
	h.add(XXX(3), 3);
	h.add(XXX(4), 4);
	h.add(XXX(5), 5);
	h.add(XXX(6), 6);

	EXPECT_EQ(6, h.size());
	EXPECT_TRUE(h.exist(XXX(1)));
	EXPECT_TRUE(h.exist(XXX(2)));
	EXPECT_TRUE(h.exist(XXX(3)));
	EXPECT_TRUE(h.exist(XXX(4)));
	EXPECT_TRUE(h.exist(XXX(5)));

	EXPECT_TRUE(h.check());

	bool p = h.remove(XXX(3));
	EXPECT_EQ(true, p);
	EXPECT_EQ(5, h.size());
	EXPECT_TRUE(h.exist(XXX(1)));
	EXPECT_TRUE(h.exist(XXX(2)));
	EXPECT_TRUE(h.exist(XXX(4)));
	EXPECT_TRUE(h.exist(XXX(5)));
	EXPECT_FALSE(h.exist(XXX(3)));

	p = h.remove(XXX(5));
	EXPECT_EQ(true, p);
	EXPECT_EQ(4, h.size());
	EXPECT_TRUE(h.exist(XXX(1)));
	EXPECT_TRUE(h.exist(XXX(2)));
	EXPECT_TRUE(h.exist(XXX(4)));
	EXPECT_FALSE(h.exist(XXX(5)));
	EXPECT_FALSE(h.exist(XXX(3)));

	p = h.remove(XXX(6));
	EXPECT_EQ(true, p);
	EXPECT_EQ(3, h.size());
	EXPECT_TRUE(h.exist(XXX(1)));
	EXPECT_TRUE(h.exist(XXX(2)));
	EXPECT_TRUE(h.exist(XXX(4)));
	EXPECT_FALSE(h.exist(XXX(5)));
	EXPECT_FALSE(h.exist(XXX(3)));

	EXPECT_TRUE(h.check());

	p = h.remove(XXX(1));
	EXPECT_EQ(true, p);
	EXPECT_EQ(2, h.size());
	EXPECT_FALSE(h.exist(XXX(1)));
	EXPECT_TRUE(h.exist(XXX(2)));
	EXPECT_TRUE(h.exist(XXX(4)));
	EXPECT_FALSE(h.exist(XXX(5)));
	EXPECT_FALSE(h.exist(XXX(3)));

	p = h.remove(XXX(2));
	EXPECT_EQ(true, p);
	EXPECT_EQ(1, h.size());
	EXPECT_FALSE(h.exist(XXX(1)));
	EXPECT_FALSE(h.exist(XXX(2)));
	EXPECT_TRUE(h.exist(XXX(4)));
	EXPECT_FALSE(h.exist(XXX(5)));
	EXPECT_FALSE(h.exist(XXX(3)));

	EXPECT_TRUE(h.check());

	h.add(XXX(5), 5);
	EXPECT_EQ(2, h.size());
	EXPECT_TRUE(h.exist(XXX(5)));
	
	h.add(XXX(5), 6);
	EXPECT_EQ(2, h.size());
	EXPECT_TRUE(h.exist(XXX(5)));
	EXPECT_EQ(6, h.get(XXX(5)));

	h.add(XXX(6), 5);
	EXPECT_EQ(3, h.size());
	EXPECT_TRUE(h.exist(XXX(6)));
	EXPECT_EQ(5, h.get(XXX(6)));

	p = h.remove(XXX(6));
	EXPECT_EQ(true, p);

	h.add(XXX(2), 2);
	EXPECT_TRUE(h.check());
	h.clear();
	EXPECT_EQ(0, h.size());
	EXPECT_TRUE(h.check());

	h.add(XXX(6), 6);
	h.add(XXX(4), 4);
	h.add(XXX(4), 5);
	h.add(XXX(8), 8);
	EXPECT_EQ(3, h.size());
	p = h.remove(XXX(6));
	EXPECT_EQ(2, h.size());
	EXPECT_EQ(true, p);
	EXPECT_EQ(5, h.get(XXX(4)));
	p = h.remove(XXX(4));
	EXPECT_EQ(true, p);
	p = h.remove(XXX(8));
	EXPECT_EQ(true, p);
	EXPECT_EQ(0, h.size());
	EXPECT_TRUE(h.check());

	{
		HashTable<XXX, int> h1(3);

		EXPECT_EQ(3, h1.getBucketSize());
	}

	{
		HashTable<XXX, int> h1(7);

		EXPECT_EQ(13, h1.getBucketSize());
	}

	{
		HashTable<XXX, int> h1;

		EXPECT_EQ(13, h1.getBucketSize());
	}

	{
		HashTable<XXX, int> h1(20);

		EXPECT_EQ(97, h1.getBucketSize());
	}

}

TEST(HashMap, Basic2) {
	HashTable<XXX, int> h(2);
	EXPECT_EQ(0, h.size());
	h.clear();
	EXPECT_EQ(0, h.size());
	h.add(XXX(1), 1);
	h.add(XXX(2), 2);
	h.add(XXX(3), 3);
	h.add(XXX(4), 4);
	h.add(XXX(5), 5);


	int p = h.get(XXX(1));
	EXPECT_EQ(1, p);

	h.remove(XXX(4));
	p = h.get(XXX(2));
	EXPECT_EQ(2, p);

	EXPECT_EQ(false,h.remove(XXX(10)));
	EXPECT_THROW(h.get(XXX(10)), std::runtime_error);
	
	EXPECT_TRUE(h.check());
}



TEST(HashMap, Rehash1) {
	HashTable<XXX, int> h;
	for (int i = 0; i < 10;i++) {
		h.add(XXX(i), i);
	}
		
	for (int i = 100; i < 110; i++) {
		h.add(XXX(i), i);
	}
	EXPECT_TRUE(h.check());
	EXPECT_EQ(20, h.size());


	for (int i = 20; i < 100; i++) {
		h.add(XXX(i), i);
	}

	EXPECT_EQ(100, h.size());
	EXPECT_TRUE(h.check());
}

TEST(HashMap, Large) {

	HashTable<int, int> h;

	srand(0);
	std::set<int> ok;
	for (int i = 0; i < 10000; i++)
	{
		int x = rand() % 200000;
		ok.insert(x);
		h.add(x, i);
	}
	EXPECT_TRUE(h.check());
	EXPECT_EQ(ok.size(), h.size());

}


TEST(HashMap, Integer) {
	HashTable<int, int> h;
	bool b = h.exist(2);
	EXPECT_EQ(b, false);
	h.add(2, 2);

	int v = h.get(2);
	EXPECT_EQ(2, v);
	b= h.exist(2);
	EXPECT_EQ(true, b);

	h.remove(2);
	b= h.exist(2);
	EXPECT_EQ(false, b);

}

TEST(HashMap, ObjectAsValue) {
	HashTable<int, XXX> h;
	
	h.add(1, XXX(2));

	const XXX &v = h.get(1);
	EXPECT_EQ(2, v.getHash());

	h.remove(1);
	bool b = h.exist(1);
	EXPECT_EQ(false, b);

}

TEST(HashMap, Range) {
	HashTable<int, int> h;

	h.add(1,1);
	h.add(10, 2);
	h.add(2, 3);
	h.add(20, 4);
	h.add(21, 5);
	h.add(41, 10);
	h.add(30, 7);

	int i = 0;
	int sum = 0;
	for (auto &x : h)
	{
		sum += x.second;
		i++;		
	}
	EXPECT_EQ(32, sum);
	EXPECT_EQ(7, i);

	sum = 0;
	HashTable<int, int>::iterator it = h.find(21);
	EXPECT_TRUE(it != h.end());
	while (it != h.end()) {
		sum += (*it).second;
		++it;
	}
	

	HashTable<int, XXX> hh;

	hh.add(1, XXX(1));
	hh.add(2, XXX(2));
	sum=0;
	for (auto &x : hh)
	{
		sum += x.second.getHash();	
	}

	EXPECT_EQ(3, sum);
	sum = 0;
	HashTable<int, XXX>::iterator itx = hh.begin();
	for (itx; itx != hh.end(); ++itx) {	
		sum += (*itx).first;				
	}

	EXPECT_EQ(3, sum);


	auto itt = h.find(1);
	EXPECT_TRUE(itt != h.end());
	EXPECT_EQ(1, 1);

	itt= h.remove(itt);

	EXPECT_FALSE(h.exist(1));


	HashTable<int, int> ::iterator it2 = h.find(66);
	EXPECT_EQ(it2, h.end());

	++it2;

	EXPECT_EQ(it2, h.end());
}

//OBS this test is using key as hash and 13 as bucketsize
TEST(HashMap, Range2) {
	HashTable<int, int> h;

	h.add(1, 1);
	h.add(10, 2);
	h.add(2, 3);
	h.add(20, 4);
	h.add(21, 5);
	h.add(41, 10);
	h.add(30, 7);
	h.add(28, 7);

	//key : 2, 41, 28 at bucket 2

	//post
	HashTable<int, int>::iterator it = h.find(2);
	it++;
	EXPECT_EQ(41, (*it).first);

	//pre
	it = h.find(2);
	++it;
	EXPECT_EQ(41, (*it).first);

	//test post
	it = h.find(2);
	HashTable<int, int>::iterator iz = it++;
	EXPECT_EQ(2, (*iz).first);
	EXPECT_EQ(41, (*it).first);

	//test pre
	it = h.find(2);
	iz = ++it;
	EXPECT_EQ(41, (*iz).first);
	EXPECT_EQ(41, (*it).first);



	it = h.find(41);
	++it;
	EXPECT_EQ(28, (*it).first);

	it = h.find(2);
	++it;
	EXPECT_EQ(41, (*it).first);

	//continue to bucket 4, where key 30 is 
	it = h.find(28);
	++it;
	EXPECT_EQ(30, (*it).first);

	it = h.find(41);
	it = h.remove(it);
	EXPECT_FALSE(h.exist(41));
	EXPECT_EQ(28, (*it).first);

	it = h.find(2);
	it = h.remove(it);
	EXPECT_FALSE(h.exist(2));
	EXPECT_EQ(28, (*it).first);

	//remove 28
	it = h.remove(it);
	EXPECT_FALSE(h.exist(28));
	EXPECT_EQ(30, (*it).first);

	it = h.begin();
	HashTable<int, int>::iterator itEnd = h.end();

	for (it; it != itEnd;)
	{
		if ((*it).first == 2) {
			it = h.remove(it);
		}
		if ((*it).first == 20) {
			it = h.remove(it);
		}

		else {
			++it;
		}
	}
	EXPECT_FALSE(h.exist(2));
	EXPECT_FALSE(h.exist(20));



	h.add(1, 1);
	h.add(10, 2);
	h.add(2, 3);
	h.add(20, 4);
	h.add(21, 5);
	h.add(41, 10);
	h.add(30, 7);
	h.add(28, 7);

	it = h.begin();
	itEnd = h.end();
	auto tot = h.size();
	for (it; it != itEnd; tot--)
	{
		it = h.remove(it);
	}

	EXPECT_EQ(0, tot);
	EXPECT_EQ(tot, h.size());
}

TEST(HashMap, Range3) {
	HashTable<int, int> h;

	h.add(1, 1);
	h.add(10, 2);
	h.add(2, 3);
	h.add(20, 4);
	h.add(21, 5);
	h.add(41, 10);
	h.add(30, 7);
	h.add(28, 7);

	auto tot = std::distance(h.begin(), h.end());
	EXPECT_EQ(8, tot);

	h.clear();

	tot = std::distance(h.begin(), h.end());
	EXPECT_EQ(0, tot);

	h.add(1, 1);
	h.add(10, 2);
	h.add(2, 3);
	h.add(20, 4);
	h.add(21, 5);
	h.add(41, 10);
	h.add(30, 7);	
	h.add(28, 7);

	auto func = [](auto acc,const auto &pair) {
		return acc + pair.second;
	};

	auto sum = std::accumulate(h.begin(), h.end(), 0, func );
	EXPECT_EQ(39, sum);

		
	
	for (int i = 0; i < 10000; i++)
	{	
		h.add(42+i, 1);
	}

	sum = std::accumulate(h.begin(), h.end(), 0, func);

	EXPECT_EQ(10039, sum);

}

TEST(HashMap, CopyMove) {
	HashTable<int, int> h;

	h.add(1, 1);
	h.add(10, 2);
	h.add(2, 3);
	h.add(20, 4);
	h.add(21, 5);


	HashTable<int, int> hh(h);
	EXPECT_EQ(5,hh.size());
	EXPECT_TRUE(hh.exist(1));
	EXPECT_TRUE(hh.exist(10));
	EXPECT_TRUE(hh.exist(2));
	EXPECT_TRUE(hh.exist(20));
	EXPECT_TRUE(hh.exist(21));

	HashTable<int, int> x; 
	x.add(99, 99);
	x = h;
	EXPECT_EQ(5, x.size());
	EXPECT_FALSE(x.exist(99));
	EXPECT_TRUE(x.exist(1));
	EXPECT_TRUE(x.exist(10));
	EXPECT_TRUE(x.exist(2));
	EXPECT_TRUE(x.exist(20));
	EXPECT_TRUE(x.exist(21));


	HashTable<int, int> hhh(std::move(hh));
	EXPECT_EQ(5, hhh.size());
	EXPECT_TRUE(hhh.exist(1));
	EXPECT_TRUE(hhh.exist(10));
	EXPECT_TRUE(hhh.exist(2));
	EXPECT_TRUE(hhh.exist(20));
	EXPECT_TRUE(hhh.exist(21));

	EXPECT_EQ(0, hh.size());


	HashTable<int, int> xx;
	xx.add(99, 99);
	xx = std::move(hhh);

	EXPECT_EQ(5, xx.size());
	EXPECT_FALSE(xx.exist(99));
	EXPECT_TRUE(xx.exist(1));
	EXPECT_TRUE(xx.exist(10));
	EXPECT_TRUE(xx.exist(2));
	EXPECT_TRUE(xx.exist(20));
	EXPECT_TRUE(xx.exist(21));

	EXPECT_EQ(0, hhh.size());


}


template <class T>
class my_allocator
{
public:
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;

	my_allocator() {}
	my_allocator(const my_allocator&) {}

	int allocs = 0;
	int deallocs=0;
	size_type deallocs_byte = 0;
	pointer   allocate(size_type n, const void * = 0) {
		T* t = (T*)malloc(n * sizeof(T));
		allocs++;
		return t;
	}

	void      deallocate(void* p, size_type s) {
		if (p) {
			free(p);		
		}
		deallocs_byte += s;
		deallocs++;
	}

	pointer           address(reference x) const { return &x; }
	const_pointer     address(const_reference x) const { return &x; }
	my_allocator<T>&  operator=(const my_allocator&) { return *this; }
	void              construct(pointer p)
	{
		new ((T*)p) T();
	}
	void              destroy(pointer p) { p->~T(); }

	size_type         max_size() const { return size_t(-1); }

	template <class U>
	struct rebind { typedef my_allocator<U> other; };

	template <class U>
	my_allocator(const my_allocator<U>&) {}

	template <class U>
	my_allocator& operator=(const my_allocator<U>&) { return *this; }
};


TEST(HashMap, Memory) {

	HashTable<int, int, my_allocator<int>> h;
	h.add(1, 1);
	h.add(2, 2);
	h.add(3, 3);
	h.add(11, 11);
	h.add(12, 12);
	h.add(13, 13);
	h.add(21, 21);
	h.add(22, 22);
	h.add(23, 23);
	h.add(24, 24);
	h.add(25, 25);
	h.add(31, 31);
	h.add(41, 41);
	h.add(43, 43);
	h.add(44, 44);
	h.add(51, 51);
	h.add(210, 210);
	h.add(230, 310);


	h.remove(3);
	h.remove(1);
	h.add(3, 3);
	h.add(1, 1);
	h.destroy();
	//check allocationa == deallocations
	EXPECT_EQ(h.get_allocator().deallocs,h.get_allocator().allocs);

}


TEST(HashMap, Pointer) {

	HashTable<int, XXX*> h;
	h.add(1, new XXX(1));

	XXX *p = h.get(1);
	EXPECT_TRUE(p!=nullptr);

	delete p;
	h.clear();

	XXX *ppp = new XXX(1);
	HashTable<XXX*, int> hh;
	hh.add(ppp,1);

	int t = hh.get(ppp);
	EXPECT_EQ(1,t);
}

TEST(HashMap, string) {

	HashTable<std::string, XXX> h;
	h.add("hej", XXX(1));
	h.add("a", XXX(2));
	h.add("humla", XXX(3));
	h.add("Pollasdsd", XXX(3));

	EXPECT_EQ(4,h.size());
	XXX &p = h.get("hej");
	EXPECT_EQ(1, p.getHash());


}

TEST(HashMap, LargeAndTimer) {

	HashTable<int, int> h;

	srand(std::time(0));
	std::set<int> ok;
	for (int i = 0; i < 1000000; i++)
	{
		int x = (rand() << 16) | rand();
		ok.insert(x);
		h.add(x, i);
	}
	EXPECT_TRUE(h.check());
	EXPECT_EQ(ok.size(), h.size());

	HashTable<int, int> hh = std::move(h);
	EXPECT_TRUE(h.check());
	EXPECT_TRUE(hh.check());
	EXPECT_EQ(ok.size(), hh.size());

	//erase some
	for (int i = 0; i < 5000; i++) {	
		int t = (*ok.begin());
		hh.remove(t);
		ok.erase(t);
	}


	EXPECT_TRUE(hh.check());
	EXPECT_EQ(ok.size(), hh.size());

	hh.add(12, 12);	
	ok.insert(12);
	
	//insert x numbers to test get() against
	std::vector<int> data;

	for (int i = 0; i < 250000; i++) {
		int t = (*ok.begin());
		ok.erase(t);
		data.push_back(t);
		if (ok.empty()) break;
	}
	int sum = 0;
	auto start = std::chrono::high_resolution_clock::now();

	std::for_each(data.begin(), data.end() , [&](auto x) 
	{
		sum+= hh.get(x);

	});
	
	std::for_each(data.rbegin(), data.rend(), [&](auto x)
	{
		sum -= hh.get(x);

	});

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	EXPECT_LE(0, sum);
	EXPECT_GT(0.04, diff.count());

	int s= hh.getBucketSize();
	hh.clearAndTrim();
	EXPECT_GT(s, hh.getBucketSize());

}

TEST(HashMap, Operatorindex) {
	HashTable<int, int> h;
	h[1] = 4;

	int t = h[1];
	EXPECT_EQ(4, t);

	h[1] = 5;
	EXPECT_EQ(5, h[1]);

	t = h[2];
	EXPECT_EQ(0, t);


}
