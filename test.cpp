#include "pch.h"
#include "..\hashtable\hashtable.h"

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

	bool operator==(const XXX &v) {
		return this->hash == v.hash;
	}
};

TEST(TestCaseName, TestNameHash1) {
	HashTable<XXX, int> h(2);
	EXPECT_EQ(0, h.size());
	h.clear();
	EXPECT_EQ(0, h.size());
	h.add(XXX(1), 1);
	h.add(XXX(2), 2);
	h.add(XXX(3), 3);
	h.add(XXX(4), 4);
	h.add(XXX(5), 5);
	EXPECT_EQ(5, h.size());
	EXPECT_TRUE(h.exist(XXX(1)));
	EXPECT_TRUE(h.exist(XXX(2)));
	EXPECT_TRUE(h.exist(XXX(3)));
	EXPECT_TRUE(h.exist(XXX(4)));
	EXPECT_TRUE(h.exist(XXX(5)));
	bool p = h.remove(XXX(3));
	EXPECT_EQ(true, p);
	EXPECT_EQ(4, h.size());
	EXPECT_TRUE(h.exist(XXX(1)));
	EXPECT_TRUE(h.exist(XXX(2)));
	EXPECT_TRUE(h.exist(XXX(4)));
	EXPECT_TRUE(h.exist(XXX(5)));
	EXPECT_FALSE(h.exist(XXX(3)));

	p = h.remove(XXX(5));
	EXPECT_EQ(true, p);
	EXPECT_EQ(3, h.size());
	EXPECT_TRUE(h.exist(XXX(1)));
	EXPECT_TRUE(h.exist(XXX(2)));
	EXPECT_TRUE(h.exist(XXX(4)));
	EXPECT_FALSE(h.exist(XXX(5)));
	EXPECT_FALSE(h.exist(XXX(3)));

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

	h.add(XXX(5), 5);
	EXPECT_EQ(2, h.size());
	EXPECT_TRUE(h.exist(XXX(5)));
	
	h.add(XXX(5), 5);
	EXPECT_EQ(2, h.size());
	EXPECT_TRUE(h.exist(XXX(5)));

	h.clear();
	EXPECT_EQ(0, h.size());
}

TEST(TestCaseName, TestNameHash3) {
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
	EXPECT_THROW(h.get(XXX(10)), std::exception);
	
}



TEST(TestCaseName, TestNamerehash) {
	HashTable<XXX, int> h;
	for (int i = 0; i < 10;i++) {
		h.add(XXX(i), i);
	}
		
	for (int i = 100; i < 110; i++) {
		h.add(XXX(i), i);
	}

	EXPECT_EQ(20, h.size());


	for (int i = 50; i < 100; i++) {
		h.add(XXX(i), i);
	}

	EXPECT_EQ(70, h.size());

}


TEST(TestCaseName, TestNameInt) {
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

TEST(TestCaseName, TestNameX) {
	HashTable<int, XXX> h;
	
	h.add(1, XXX(2));

	XXX v = h.get(1);
	EXPECT_EQ(2, v.getHash());

	h.remove(1);
	bool b = h.exist(1);
	EXPECT_EQ(false, b);

}

TEST(TestCaseName, TestNameRange) {
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
}