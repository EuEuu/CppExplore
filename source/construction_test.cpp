#include <iostream>

class A {
public:
    A(int m_x) : x_(m_x) {
        count_++;
        std::cout << "construction\n";
    }

    A(const A& obj) : x_(obj.x_) {
        count_++;
        std::cout << "copy construction\n";
    }

    A& operator=(const A& obj) {
        x_ = obj.x_;
        count_++;
        std::cout << "copy assignment operator construction\n";
        return *this;
    }

    A(const A&& obj) : x_(obj.x_) {
        count_++;
        std::cout << "move construction\n";
    }

    A& operator==(const A&& obj) {
        x_ = obj.x_;
        count_++;
        std::cout << "move assignment operator construction\n";
        return *this;
    }

    ~A() {
        count_ = 0;
        std::cout << "deconstruction\n";
    }

    void print_count() {
        std::cout << "count: " << count_ << "\n";
    }

private:

    int count_{ 0 };

    int x_;

};

A getA() {
    return A(1);
}

A&& moveA(A& a) {
    return std::move(a);
}

void test_construction() {

    std::cout << "---------------1---------------\n";
    A a(1);//调用构造函数
    a.print_count();

    std::cout << "---------------2---------------\n";
    A b = a;//创建新的对象b,使用a初始化b,因此调用的是拷贝构造函数
    b.print_count();

    std::cout << "---------------3---------------\n";
    A c(a);//创建对象c,使用a初始化b,因此调用的是拷贝构造函数
    c.print_count();

    std::cout << "---------------4---------------\n";
    b = a;//使用a更新b的值, 不需要创建新的对象，因此调用的是拷贝赋值运算符
    b.print_count();

    std::cout << "---------------5---------------\n";
    A d = A(1);//创建新的对象d, 使用临时对象A(1)初始化，为啥只调用了构造函数?
    d.print_count();

    std::cout << "---------------6---------------\n";
    A e = std::move(a);//创建对象e，使用a初始化e，但是调用了std::move(a)将左值转化为了右值，因此调用的是移动构造函数
    e.print_count();


    std::cout << "---------------7---------------\n";
    A f = getA();//创建对象f，使用getA()返回的临时对象初始化f，为啥只调用了构造函数?
    f.print_count();

    std::cout << "---------------8---------------\n";
    A&& g = moveA(f);//没有创建新的对象，也没有更新任何对象，只是将moveA的返回值绑定到了右值引用g,因此不调用任何构造函数
    g.print_count();

    std::cout << "---------------9---------------\n";
    d = A(1);//使用临时对象A(1)更新d,因为不需要创建新的对象，因此调用的是拷贝赋值运算符
    // 有个问题，如果不加形参不加const编译不过 error: cannot bind non-const lvalue reference of type ‘A&’ to an rvalue of type ‘A’
    d.print_count();

}

int main(int argc, char argv) {

    test_construction();

	std::cout << "hello world\n";
	return 0;
}