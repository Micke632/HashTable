#include "pch.h"
#include "..\hashtable\hashtable.h"
#include <memory>
#include <stdlib.h> 

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
	XXX(const XXX &other) = default;
	XXX(XXX &&other) = default;
	XXX& operator=(XXX&&other) = default;
	XXX& operator=(const XXX&other) = default;
	
	bool operator==(const XXX &v) {
		return this->hash == v.hash;
	}
};

TEST(HashMap, Basic) {
	HashTable<XXX, int> h(2);

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


	for (int i = 50; i < 100; i++) {
		h.add(XXX(i), i);
	}

	EXPECT_EQ(70, h.size());
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

	XXX v = h.get(1);
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

	int i = 0;
	int sum = 0;
	for (auto &x : h)
	{
		sum += x.second;
		i++;		
	}
	EXPECT_EQ(15, sum);
	EXPECT_EQ(5, i);


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


	HashTable<int, int> ::iterator it = h.find(1);
	EXPECT_TRUE(it != h.end());
	EXPECT_EQ(1, 1);

	HashTable<int, int> ::iterator it2 = h.find(66);
	EXPECT_EQ(it2, h.end());

	++it2;

	EXPECT_EQ(it2, h.end());
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
	int deallocs = 0;

	pointer   allocate(size_type n, const void * = 0) {
		T* t = (T*)malloc(n * sizeof(T));
		allocs++;
		return t;
	}

	void      deallocate(void* p, size_type) {
		if (p) {
			free(p);		
		}
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
	h.add(21, 21);
	h.add(22, 22);
	h.remove(3);
	h.remove(1);
	h.clear();
	//check allocationa == deallocations
	EXPECT_EQ(h.getAllocator().deallocs,h.getAllocator().allocs);

	
}


TEST(HashMap, Pointer) {

	HashTable<int, XXX*> h;
	h.add(1, new XXX(1));

	XXX *p = h.get(1);
	EXPECT_TRUE(p!=nullptr);

	delete p;
	h.clear();

}



TEST(HashMap, Large2) {

	HashTable<int, int> h;

	srand(0);
	std::set<int> ok;
	for (int i = 0; i < 1000; i++)
	{
		int x = rand() % 200000;
		ok.insert(x);
		h.add(x, i);
	}
	EXPECT_TRUE(h.check());
	EXPECT_EQ(ok.size(), h.size());

	HashTable<int, int> hh = std::move(h);
	EXPECT_TRUE(h.check());
	EXPECT_TRUE(hh.check());
	EXPECT_EQ(ok.size(), hh.size());

	for (int i = 0; i < 200; i++) {	
		int t = (*ok.begin());
		hh.remove(t);
		ok.erase(t);
	}


	EXPECT_TRUE(hh.check());
	EXPECT_EQ(ok.size(), hh.size());

	hh.add(12, 12);
}