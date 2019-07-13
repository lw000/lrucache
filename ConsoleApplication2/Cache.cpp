#include "pch.h"
#include "Cache.h"

enum class ALG {
	// LeastRecentlyAdded indicates a least-recently-added eviction policy.
	LeastRecentlyAdded = 0,
	// LeastRecentlyUsed indicates a least-recently-used eviction policy.
	LeastRecentlyUsed = 1,
};

Cache::Cache()
{
}

Cache::~Cache()
{
	std::lock_guard<std::mutex> l(this->m);
	for (auto k : this->items) {
		delete k.second;
	}
}

std::vector<cached*> Cache::Get(const std::initializer_list<std::string>& keys) {
	if (keys.size() == 0) {
		return std::vector<cached*>();
	}

	std::vector<cached*> ret;
	ret.reserve(keys.size());

	std::lock_guard<std::mutex> l(this->m);
	for (auto k : keys) {
		cached* p = this->items[k];
		if (p != nullptr) {
			this->recordAccess(k);
			ret.push_back(p);
		}
		else
		{
			ret.push_back(nullptr);
		}
	}
	return ret;
}

int Cache::Put(const std::string key, const Item &e) {
	if (key.size() == 0) {
		return -1;
	}

	{
		std::lock_guard<std::mutex> l(this->m);
		this->items.erase(key);
		cached *c = new cached(e);
		c->setElementIfNotNil(this->recordAccess(key));
		this->items[key] = c;
	}

	return 0;
}

void Cache::Remove(const std::initializer_list<std::string>& keys) {
	if (keys.size() == 0) {
		return;
	}

	std::lock_guard<std::mutex> l(this->m);
	for (auto k : keys) {
		this->remove(k);
	}
}

void Cache::remove(const std::string& key) {
	if (key.empty()) {
		return;
	}

	cached *p = this->items[key];
	if (p != nullptr) {
		this->items.erase(key);
		this->keyList.remove(key);
	}
}

uint64_t Cache::Size() {
	std::lock_guard<std::mutex> l(this->m);
	uint64_t c = this->items.size();
	return c;
}

std::string Cache::recordAccess(const std::string& key) {
	if (key.empty()) {
		return std::string();
	}
	cached* p = this->items[key];
	if (p != nullptr) {
		this->keyList.remove(p->element);
		this->keyList.push_front(p->element);
		return p->element;
	}
	else
	{
		this->keyList.push_front(key);
		return key;
	}
}