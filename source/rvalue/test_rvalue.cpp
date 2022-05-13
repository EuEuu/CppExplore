#include <iostream>

template<typename T>
class Business {
public:
	Business() {
		data_ = new T[size_];
		std::cout << "���캯��: " << " " << &data_ << std::endl;
	}
	~Business() {
		if (data_) {
			delete data_;
			std::cout << "data: " << &data_ << std::endl;
		}
		std::cout << "��������\n";
	}

	Business(const Business& obj) {
		size_ = obj.size_;
		data_ = new T[obj.size_];
		memcpy(data_, obj.data_, sizeof(T) * obj.size_);
		std::cout << "�������캯��\n";
	}

	Business(Business&& obj) noexcept {
		size_ = obj.size_;
		data_ = obj.data_;
		obj.data_ = nullptr;
		std::cout << "�ƶ��������캯��\n";
	}

	Business& operator=(const Business& obj) {
		if (this != &obj) {
			size_ = obj.size_;
			data_ = new T[obj.size_];
			memcpy(data_, obj.data_, sizeof(T) * obj.size_);
		}
		std::cout << "������ֵ���캯��\n";
	}

	Business& operator=(Business&& obj) noexcept {
		if (this != &obj) {
			size_ = obj.size_;
			data_ = obj.data_;
			obj.data_ = nullptr;
		}
		std::cout << "�ƶ�������ֵ���캯��\n";
	}

private:
	int size_{ 10 };
	T* data_{ nullptr };
};

template<typename T>
Business<T> CreatBusiness() {
	Business<T> temp;//���캯��
	return temp;
}

template<typename T>
void SaveBusiness(Business<T> obj) {
	std::cout << __FUNCTION__ << std::endl;
}

//void SaveToInt(int&& obj) {
//	//obj��һ����ʱֵ������
//}
//
//template<typename T>
//void Save(T&& obj) {
//
//	SaveToInt(std::forward<T>(obj));//����ת�� �� �����۵�
//	std::cout << __FUNCTION__ << std::endl;
//}

int main() {

	Business<char> b1;//���캯��

	Business<char> b2 = b1;//�������캯��

	Business<char> b3(b1);//�������캯��

	Business<char> b4 = CreatBusiness<char>();//�ƶ��������캯�� CreatBusiness<char>()��������һ����ʱ��������һ����ֵ

	Business<char> b5(CreatBusiness<char>());//�ƶ��������캯��

	std::cout << "SaveBusiness(b5)" << std::endl;
	SaveBusiness(b5);//��������

	std::cout << "SaveBusiness<char>(CreatBusiness<char>())" << std::endl;
	SaveBusiness<char>(CreatBusiness<char>());//�ƶ��������캯��

	std::cout << "SaveBusiness<char>(std::move(b1))" << std::endl;
	SaveBusiness<char>(std::move(b1));//�ƶ��������캯��

	std::cout << "SaveBusiness<char>(static_cast<Business<char>&&>(b2))" << std::endl;
	SaveBusiness<char>(static_cast<Business<char>&&>(b2));//�ƶ��������캯��

	Business<char>&& r = CreatBusiness<char>();
	SaveBusiness(r);//�������� r����ֵ���ã�������ֵ��������һ������

	return 0;
}