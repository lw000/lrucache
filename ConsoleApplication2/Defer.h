#pragma once

#include <functional>
#include <vector>
#include <stack>

typedef std::function<void()> DeferCall;

class Defer {
	DeferCall f;
	std::stack<DeferCall>fs;

public:
	Defer(const DeferCall& f) {
		this->f = f;
	}

	Defer(const std::initializer_list<DeferCall> &fs) {
		for (auto f : fs) {
			this->fs.push(f);
		}
	}

	~Defer() {
		if (f != nullptr) {
			this->f();
		}

		while (!this->fs.empty())
		{
			auto f = this->fs.top();
			this->fs.pop();
			f();
		}
	}
};
