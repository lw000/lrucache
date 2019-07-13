#pragma once

#include <unordered_map>
#include <list>
#include <string>
#include <mutex>

typedef struct _Item
{
	int a;
public:
	_Item(int a) {
		this->a = a;
	}
public:
	bool operator >(const _Item &o) {

	}
}Item;

typedef struct _cached
{
public:
	Item e;
	std::string element;

public:
	_cached(const Item &e): e(e) {
	}
public:
	bool operator >(const _cached &o){

	}

	void setElementIfNotNil(const std::string& element) {
		if (this->element.empty()) {
			this->element = element;
		}
	}
}cached;

class Cache
{
private:
	std::mutex m;
	std::unordered_map<std::string, cached*> items;
	std::list<std::string> keyList;

public:
	Cache();
	~Cache();

public:
	int Put(const std::string key, const Item &e);
	std::vector<cached*> Get(const std::initializer_list< std::string>& ls);
	void Remove(const std::initializer_list<std::string>& ls);
	uint64_t Size();

private:
	void remove(const std::string& key);
	std::string recordAccess(const std::string& key);
};

