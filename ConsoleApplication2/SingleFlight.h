#pragma once

#include <string>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

class Group;

class Result {
	void *val;
	bool shared;
	int  dups;
public:
	Result(void *val, bool shared) {
		this->val = val;
		this->shared = shared;
	}
};

class SingleFlight
{
	Group *group;

public:
	SingleFlight();
	~SingleFlight();

public:
	Result Do(const std::string& key, const std::function<Result* ()> fn);
};

