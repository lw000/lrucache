// ConsoleApplication2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <future>
#include <csignal>

#include "Cache.h"
#include "TaskQueue.h"
#include "Ticker.h"
#include "Defer.h"

const int TestThreadCount = 20;

class HelloFunctionObject {
public:
	std::string operator()(const std::string& s) const {
		return "Hello C++11 from" + s + ".";
	}
};

class PrintTask : public Task {
private:
	std::string name;

public:
	PrintTask(const std::string& name) {
		this->name = name;
	}
public:
	virtual void run() override {
		//std::cout << " thread_id " << std::this_thread::get_id() << " printTask " << std::endl;
		LOG(INFO) << " thread_id " << std::this_thread::get_id() << " printTask " << std::endl;
	}
};

DEFINE_bool(is, true, "");
DEFINE_string(str, "levi", "");
DEFINE_int32(port, 6379, "port");
DEFINE_string(ip, "127.0.0.1", "ip adress");


bool ValidatePort(const char*flagname, int32_t val) {
	if (val > 0 && val < 32768) {
		return true;
	}
	return false;
}

void handleUserInterrupt(int signo) {
	if (signo == SIGINT) {
		LOG(INFO) << "SIGINT trapped ..." << std::endl;
	}
}

int main(int argc, char * argv[])
{
	::signal(SIGINT, handleUserInterrupt);

	Defer defer([]() {
		google::ShutdownGoogleLogging();
	});
	
	google::ParseCommandLineFlags(&argc, &argv, true);

	bool isPort = google::RegisterFlagValidator(&FLAGS_port, &ValidatePort);
	if (isPort) {

	}

	LOG(INFO) << "port " << FLAGS_port << std::endl;
	LOG(INFO) << "ip " << FLAGS_ip << std::endl;

	google::InitGoogleLogging(argv[0]);
	
	google::SetLogDestination(google::GLOG_INFO, "./log");

	if (FLAGS_is) {
		LOG(INFO) << FLAGS_is;
	}

	if (!FLAGS_str.empty()) {
		LOG(INFO) << FLAGS_str;
	}

	Cache cache(100);
	for (int i = 0; i < 30; i++) {
		cache.Put(std::to_string(i), Item(i));
	}

	std::vector<cached*> s = cache.Get({ });
	std::vector<cached*> s1 = cache.Get({ "1", "2", "3", "5", "9", "15" });
	std::vector<cached*> s2 = cache.Get({ "6", "7", "8" });
	for (auto v : s) {
		if (v != nullptr) {
			LOG(INFO) << v->e.a << std::endl;
		}
	}
	
	for (auto v : s1) {
		if (v != nullptr) {
			LOG(INFO) << v->e.a << std::endl;
		}
	}

	cache.Remove({ "1", "2", "3" });

	std::async([](const std::string &s) -> std::string {
		return "Hello C++11 from" + s + ".";
	}, "function");

	HelloFunctionObject fobject;
 	auto futureObject = std::async(fobject, "function object");
 	std::cout << futureObject.get() << std::endl;

	TaskQueue queue;
	if (queue.Start()) {
		queue.addTask(new PrintTask("name_1"));
		queue.addTask(new PrintTask("name_2"));
		queue.addTask(new PrintTask("name_3"));
		queue.addTask(new PrintTask("name_4"));
	}

	std::thread t[TestThreadCount];
	for (int i = 0; i < TestThreadCount; i++) {
		t[i] = std::thread([&queue]() {
			for (int i = 0; i < 1000; i++) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				queue.addTask(new PrintTask(std::to_string(i)));
			}
		});
	}

	for (int i = 0; i < TestThreadCount; i++) {
		t[i].detach();
	}

	Ticker tk(TickerT::S);
	tk.AfterFunc(2, []() {
		LOG(INFO) << "AfterFunc" << std::endl;
	});

	system("pause");
}