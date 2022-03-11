#include <iostream>
#include <vector>

void test_vector() {
	std::cout << __FUNCTION__ << std::endl;

	std::vector<int> vec = { 1,2,3,4 };
	auto it = vec.begin();
	while (it != vec.end()) {

		if (*it == 3) {
			it = vec.erase(it);//������һ��Ԫ�صĵ�����
			std::cout << "erase 3, return *it: " << *it << std::endl;
		}
		else {
			++it;
		}
	}

}

int main(int argc, char** argv) {

	test_vector();

	return EXIT_SUCCESS;
}