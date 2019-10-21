// ConsoleApplication2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <forward_list>
#include <array>
#include <any>
#include <list>
#include <vector>

#include <future>
#include <csignal>
#include <ctime>

#include "Cache.h"
#include "TaskQueue.h"
#include "Ticker.h"
#include "Defer.h"

#ifdef __cplusplus
	extern "C" {
#endif

	#include "cJSON.h"
	#include "cJSON_Utils.h"

#ifdef __cplusplus
}
#endif


#pragma warning(disable:4996)

const int TestThreadCount = 20;

DEFINE_bool(is, true, "");
DEFINE_string(str, "levi", "");
DEFINE_int32(port, 6379, "port");
DEFINE_string(ip, "127.0.0.1", "ip adress");

std::once_flag flag;

class HelloFunctionObject {
public:
	std::string operator()(const std::string& s) const {
		return "Hello C++11 from " + s + ".";
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
		/*LOG(INFO) << " printTask ";

		std::call_once(flag, []() {
			LOG(INFO) << "called once";
		});*/
	}
};

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

class Greet {
	const char *owner = "Greet";
public:
	void SayHello(const char *name) {
		LOG(INFO) << "Hello" << " from " << this->owner;
	}
};

struct MyStruct
{
	int a;
	int b;

	bool operator < (const MyStruct& other) const {
		return this->a < other.a && this->b < other.b;
	}

	bool operator > (const MyStruct& other) const {
		return this->a > other.a && this->b > other.b;
	}
};


struct MyStructA
{
	int a;
	int b;

	bool operator < (const MyStructA& other) const {
		return this->a < other.a && this->b < other.b;
	}

	bool operator > (const MyStructA& other) const {
		return this->a > other.a && this->b > other.b;
	}
};


std::ostream& operator << (std::ostream& o, const MyStruct& other) {
	o << "{a=" << other.a << ", b=" << other.b << "}";
	return o;
}

std::ostream& operator << (std::ostream& o, const MyStructA& other) {
	o << "{a=" << other.a << ", b=" << other.b << "}";
	return o;
}


void readfile() {
	{
		std::ifstream fs("conf.json", std::ifstream::in);
		//std::string context1((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());

		std::string context;
#if 1
		context.append((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
#else
		std::stringstream s;
		s << fs.rdbuf();
		context.append(s.str());
#endif // 1


		std::cout << "context: " << context << std::endl;

		cJSON* object = cJSON_Parse(context.c_str());
		std::string json(cJSON_Print(object));
		std::cout << "json: " << json << std::endl;
		cJSON_free(object);
	}
}



void test() {
	std::list<int> lst = { 1,2,3,4,5,6,7,8,9,0 };
	std::list<int>::iterator p = lst.begin();
	std::advance(p, 2);
	std::cout << *p << std::endl;
}

int main(int argc, char * argv[])
{
	//::signal(SIGINT, handleUserInterrupt);

	Defer defer([]() {
		google::ShutdownGoogleLogging();
	});
	
	google::ParseCommandLineFlags(&argc, &argv, true);

	bool isPort = google::RegisterFlagValidator(&FLAGS_port, &ValidatePort);
	if (isPort) {

	}

	DLOG(INFO) << "port " << FLAGS_port;
	DLOG(INFO) << "ip " << FLAGS_ip;

	LOG_IF(INFO, 20 > 10) << "log if test";

	google::InstallFailureSignalHandler();
	google::InitGoogleLogging(argv[0]);
	
	FLAGS_max_log_size = 10;

	google::SetLogDestination(google::GLOG_INFO, "log/log");
	google::SetStderrLogging(google::GLOG_INFO);

	//CHECK(1 == 2) << "fail";
	LOG(ERROR) << "error info";
	//LOG(FATAL) << "fatal info";

	if (FLAGS_is) {
		LOG(INFO) << FLAGS_is;
	}

	if (!FLAGS_str.empty()) {
		LOG(INFO) << FLAGS_str;
	}

	test();
	readfile();
	
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

	for (auto v : s2) {
		if (v != nullptr) {
			LOG(INFO) << v->e.a << std::endl;
		}
	}

	cache.Remove({ "1", "2", "3" });

	std::future<std::string>res = std::async([](const std::string &s) -> std::string {
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
			for (int i = 0; i < 10000; i++) {
				//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				
				//std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::microseconds(1000));

				//using std::chrono::system_clock;
				//std::time_t t = system_clock::to_time_t(system_clock::now());
				//struct std::tm *ptm = std::localtime(&t);
				//ptm->tm_min++;
				//ptm->tm_sec = 0;
				//std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
				
				queue.addTask(new PrintTask(std::to_string(i)));
			}
		});
	}

	for (int i = 0; i < TestThreadCount; i++) {
		t[i].detach();
	}

	Greet greet;
	std::thread ttt(&Greet::SayHello, &greet, "c++11");
	void* handle = ttt.native_handle();
	LOG(INFO) << handle;
	ttt.detach();

	Ticker tk(TickerT::S);
	tk.AfterFunc(2, []() {
		LOG(INFO) << "AfterFunc" << std::endl;
	});

	{
		std::vector<int> s{ 1,2,3,4,5,6,7,8 };
	}

	{
		MyStruct a{ 1 };
		std::pair<int, MyStruct> ss(10, MyStruct{1, 2});
		LOG(INFO) << ss.first << ", " << ss.second;
	}

	{
		std::tuple<int, int, int, int, std::string> tu(10, 20, 20, 20, "tuple");
		std::tuple<int, int, int, int, std::string> tu1 = std::make_tuple(10, 20, 20, 20, "tuple");
		if (tu == tu1) {
			LOG(INFO) << "equal";
		}
		int v1 = std::get<0>(tu);
		int v2 = std::get<1>(tu);
		int v3 = std::get<2>(tu);
		int v4 = std::get<3>(tu);
		std::string v5 = std::get<4>(tu);
		LOG(INFO) << v1 << "-" << v2 << "-" << v3 << "-" << v4 << "-" << v5;
	}

	{
		std::priority_queue<MyStruct, std::deque<MyStruct>, std::greater<MyStruct>> q;
		for (auto& v : { MyStruct{3,4}, MyStruct{7,8}, MyStruct{5,6}, MyStruct{1,2} }) {
			q.push(v);
		}
		LOG(INFO) << q.size();
	}

	{
		std::forward_list<MyStruct> fl{ MyStruct{3,4}, MyStruct{7,8}, MyStruct{5,6}, MyStruct{1,2} };
		for (auto v : fl) {
			LOG(INFO) << v;
		}
	}

	{
		std::array<MyStruct, 10> ar{ MyStruct{3,4}, MyStruct{7,8}, MyStruct{5,6}, MyStruct{1,2} };
		for (auto v : ar) {
			LOG(INFO) << v;
		}
		if (std::any_of(ar.begin(), ar.end(), [](const MyStruct & ohter) {
			return ohter.a == 5 && ohter.b == 6;
		})) {
			LOG(INFO) << "ok";
		}
	}

	{
		std::any a = 1;
		if (a.has_value()) {
			LOG(INFO) << std::any_cast<int>(a);
		}
		
		/*std::any b = "ssfdsfsdfsdf";
		if (b.has_value()) {
			try {
				LOG(INFO) << std::any_cast<char*>(b);
			}
			catch (const std::bad_any_cast& e) {
				LOG(INFO) << e.what();
			}
		}*/
	}

	int c = getchar();
}