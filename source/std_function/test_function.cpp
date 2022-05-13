#include <iostream>
#include <thread>
#include <functional>

class MyClass
{
public:

	typedef std::function<void(bool)> callback;

	MyClass(const std::string& name) {
		name_ = name;
		t = std::thread([this]() {work(); });
		std::cout << name << " " << this << "\n";
	}
	~MyClass() {
		if (t.joinable())t.join();
		cb_ = nullptr;
		std::cout << name_ << "  " << __FUNCTION__ << std::endl;
	}

	void bind_callback(callback cb) {
		cb_ = std::move(cb);
	}

	void test_function(bool  val) {
		std::cout << name_ << " " << std::boolalpha << val << ", this: " << this << std::endl;
	}

	void stop() {
		loop = false;
	}

private:

	void work() {
		bool val = true;
		for (; loop;) {
			if (cb_ != nullptr)cb_(val);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			val = !val;
		}
	}

private:
	callback cb_;
	std::thread t;
	bool loop{ true };

	std::string name_{"..."};
};

void test1() {
	MyClass a("a"), b("b");
	a.bind_callback([&b](bool val) {b.test_function(val); });

}

void test2() {
	MyClass a("a");
	{
		MyClass b("b");
		//a.bind_callback([&b](bool val) {b.test_function(val); });
		a.bind_callback(std::bind(&MyClass::test_function,&b,std::placeholders::_1));
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		b.stop();
	}

	std::cout << __FUNCTION__ << std::endl;
}

int main() {

	test2();

	return 0;
}