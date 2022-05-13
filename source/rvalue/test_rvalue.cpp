#include <iostream>

template<typename T>
class Business {
public:
	Business() {
		data_ = new T[size_];
		std::cout << "构造函数: " << " " << &data_ << std::endl;
	}
	~Business() {
		if (data_) {
			delete data_;
			std::cout << "data: " << &data_ << std::endl;
		}
		std::cout << "析构函数\n";
	}

	Business(const Business& obj) {
		size_ = obj.size_;
		data_ = new T[obj.size_];
		memcpy(data_, obj.data_, sizeof(T) * obj.size_);
		std::cout << "拷贝构造函数\n";
	}

	Business(Business&& obj) noexcept {
		size_ = obj.size_;
		data_ = obj.data_;
		obj.data_ = nullptr;
		std::cout << "移动拷贝构造函数\n";
	}

	Business& operator=(const Business& obj) {
		if (this != &obj) {
			size_ = obj.size_;
			data_ = new T[obj.size_];
			memcpy(data_, obj.data_, sizeof(T) * obj.size_);
		}
		std::cout << "拷贝赋值构造函数\n";
	}

	Business& operator=(Business&& obj) noexcept {
		if (this != &obj) {
			size_ = obj.size_;
			data_ = obj.data_;
			obj.data_ = nullptr;
		}
		std::cout << "移动拷贝赋值构造函数\n";
	}

private:
	int size_{ 10 };
	T* data_{ nullptr };
};

template<typename T>
Business<T> CreatBusiness() {
	Business<T> temp;//构造函数
	return temp;
}

template<typename T>
void SaveBusiness(Business<T> obj) {
	std::cout << __FUNCTION__ << std::endl;
}

//void SaveToInt(int&& obj) {
//	//obj是一个临时值的引用
//}
//
//template<typename T>
//void Save(T&& obj) {
//
//	SaveToInt(std::forward<T>(obj));//完美转发 和 引用折叠
//	std::cout << __FUNCTION__ << std::endl;
//}

int main() {

	Business<char> b1;//构造函数

	Business<char> b2 = b1;//拷贝构造函数

	Business<char> b3(b1);//拷贝构造函数

	Business<char> b4 = CreatBusiness<char>();//移动拷贝构造函数 CreatBusiness<char>()产生的是一个临时变量，是一个右值

	Business<char> b5(CreatBusiness<char>());//移动拷贝构造函数

	std::cout << "SaveBusiness(b5)" << std::endl;
	SaveBusiness(b5);//拷贝构造

	std::cout << "SaveBusiness<char>(CreatBusiness<char>())" << std::endl;
	SaveBusiness<char>(CreatBusiness<char>());//移动拷贝构造函数

	std::cout << "SaveBusiness<char>(std::move(b1))" << std::endl;
	SaveBusiness<char>(std::move(b1));//移动拷贝构造函数

	std::cout << "SaveBusiness<char>(static_cast<Business<char>&&>(b2))" << std::endl;
	SaveBusiness<char>(static_cast<Business<char>&&>(b2));//移动拷贝构造函数

	Business<char>&& r = CreatBusiness<char>();
	SaveBusiness(r);//拷贝构造 r是右值引用，不是右值，本质是一个变量

	return 0;
}