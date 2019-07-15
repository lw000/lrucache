#pragma once

#include <stdint.h>
#include <list>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>

class Task {
public:
	uint64_t Id;

public:
	virtual void run() = 0;
};

class TaskQueue
{
private:
	bool quit;
	std::mutex m;
	std::condition_variable cond;
	std::list<Task*> tasks;
	std::thread t[10];
public:
	TaskQueue();
	~TaskQueue();

public:
	bool Start();
	void Stop();
public:
	void addTask(Task * task);
	void removeTask(Task * task);

private:
	void run(int workerId);

	TaskQueue(TaskQueue const &) = delete;
	TaskQueue& operator=(TaskQueue const &) = delete;
};