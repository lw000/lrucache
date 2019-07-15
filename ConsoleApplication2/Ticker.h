#pragma once

#include <functional>
#include <thread>

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
