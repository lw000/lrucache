// ConsoleApplication2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "Cache.h"

enum class TickerT
{
	S = 0,
	MS = 1,
	NS = 2,
};

class Ticker {
public:	
	TickerT t;

public:
	Ticker(TickerT c) {
		t = c;
	}

public:
	void AfterFunc(const long long& ts, std::function<void()> f) {
		switch (t)
		{
		case TickerT::S:;
			std::this_thread::sleep_for(std::chrono::seconds(ts));
			break;
		case TickerT::MS:
			std::this_thread::sleep_for(std::chrono::milliseconds(ts));
			break;
		case TickerT::NS:
			std::this_thread::sleep_for(std::chrono::microseconds(ts));
			break;
		default:
			break;
		}
		f();
	}
};

int main()
{
	Cache cache(100);
	for (int i = 0; i < 30; i++) {
		cache.Put(std::to_string(i), Item(i));
	}
	//cache.Put("1", Item(1));
	//cache.Put("2", Item(2));
	//cache.Put("3", Item(3));
	//cache.Put("4", Item(4));
	//cache.Put("5", Item(5));
	std::vector<cached*> s = cache.Get({ });
	std::vector<cached*> s1 = cache.Get({ "1", "2", "3", "5", "9", "15" });
	std::vector<cached*> s2 = cache.Get({ "6", "7", "8" });
	for (auto v : s) {
		if (v != nullptr) {
			std::cout << v->e.a << std::endl;
		}
	}
	
	for (auto v : s1) {
		if (v != nullptr) {
			std::cout << v->e.a << std::endl;
		}
	}

	cache.Remove({ "1", "2", "3" });

	Ticker tk(TickerT::S);
	tk.AfterFunc(2, []() {
		std::cout << "AfterFunc" << std::endl;
	});

	system("pause");
}