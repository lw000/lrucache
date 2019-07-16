#include "pch.h"
#include "SingleFlight.h"

class call {
public:
	void *val;
	int  dups;
	bool forgotten;
	std::mutex mu;
	std::condition_variable cond;

public:
	call() {
		this->val = nullptr;
		this->dups = 0;
	}

public:
	void* Val() const {
		return this->val;
	}

	void wait() {
		std::unique_lock<std::mutex> lck(this->mu);
		this->cond.wait(lck);
	}

	void notify() {
		this->cond.notify_one();
	}
};


class Group {
private:
	std::mutex mu;
	std::unordered_map<std::string, call*> store;

public:
	Group() {

	}

public:
	Result Do(const std::string& key, const std::function<Result* ()> fn){
		this->mu.lock();
		call *tc = this->store[key];
		if (tc != nullptr) {
			tc->dups++;
			this->mu.unlock();
			tc->wait();
			return Result(tc->val, true);
		}
		call *c = new call();
		this->store[key] = c;
		this->mu.unlock();

		this->doCall(c, key, fn);

		Result r(c->val, c->dups > 0);
		delete c;
		return r;
	}

	void Forget(const std::string& key) {
		std::lock_guard<std::mutex> l(this->mu);
		call *c = this->store[key];
		if (c != nullptr) {
			c->forgotten = true;
		}
		this->store.erase(key);
	}

private:
	void doCall(call *c, const std::string& key, const std::function<Result* ()> fn) {
		Result* r = fn();
		std::lock_guard<std::mutex> lck(this->mu);
		if (!c->forgotten) {
			this->store.erase(key);
		}
		c->notify();
	}
};

SingleFlight::SingleFlight()
{
	this->group = new Group();
}


SingleFlight::~SingleFlight()
{
	delete this->group;
}

Result SingleFlight::Do(const std::string& key, const std::function<Result* ()> fn) {
	return this->group->Do(key, fn);
}
