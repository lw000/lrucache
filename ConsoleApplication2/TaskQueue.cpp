#include "pch.h"
#include "TaskQueue.h"
#include "Defer.h"

TaskQueue::TaskQueue()
{

}

TaskQueue::~TaskQueue()
{
	this->quit = true;
	this->cond.notify_all();
	for (int i = 0; i < 10; i++) {
		this->t[i].join();
	}
}

bool TaskQueue::Start() {
	for (int i = 0; i < 10; i++) {
		this->t[i] = std::thread([this](int workerId) {
			this->run(workerId);
		}, i);
	}
	return true;
}

void TaskQueue::Stop() {

}

void TaskQueue::addTask(Task * task) {
	std::unique_lock <std::mutex> lck(this->m);
	this->tasks.push_back(task);
	this->cond.notify_all();
}

void TaskQueue::removeTask(Task * task) {
	std::unique_lock <std::mutex> lck(this->m);
	this->tasks.remove(task);
}

void TaskQueue::run(int workerId) {
	Defer defer({ [workerId]() {
		printf("%d exit\n", workerId);
	} });

	do
	{
		Task * task = nullptr;
		{
			std::unique_lock<std::mutex> lck(this->m);
			while (this->tasks.empty() && !this->quit)
			{
				this->cond.wait(lck);
			}

			if (!this->tasks.empty()) {
				task = this->tasks.front();
				this->tasks.pop_front();
			}
		}

		if (task != nullptr) {
			task->run();
			delete task;
		}

	} while (!this->quit);
}