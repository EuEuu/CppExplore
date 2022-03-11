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
    A a(1);//���ù��캯��
    a.print_count();

    std::cout << "---------------2---------------\n";
    A b = a;//�����µĶ���b,ʹ��a��ʼ��b,��˵��õ��ǿ������캯��
    b.print_count();

    std::cout << "---------------3---------------\n";
    A c(a);//��������c,ʹ��a��ʼ��b,��˵��õ��ǿ������캯��
    c.print_count();

    std::cout << "---------------4---------------\n";
    b = a;//ʹ��a����b��ֵ, ����Ҫ�����µĶ�����˵��õ��ǿ�����ֵ�����
    b.print_count();

    std::cout << "---------------5---------------\n";
    A d = A(1);//�����µĶ���d, ʹ����ʱ����A(1)��ʼ����Ϊɶֻ�����˹��캯��?
    d.print_count();

    std::cout << "---------------6---------------\n";
    A e = std::move(a);//��������e��ʹ��a��ʼ��e�����ǵ�����std::move(a)����ֵת��Ϊ����ֵ����˵��õ����ƶ����캯��
    e.print_count();


    std::cout << "---------------7---------------\n";
    A f = getA();//��������f��ʹ��getA()���ص���ʱ�����ʼ��f��Ϊɶֻ�����˹��캯��?
    f.print_count();

    std::cout << "---------------8---------------\n";
    A&& g = moveA(f);//û�д����µĶ���Ҳû�и����κζ���ֻ�ǽ�moveA�ķ���ֵ�󶨵�����ֵ����g,��˲������κι��캯��
    g.print_count();

    std::cout << "---------------9---------------\n";
    d = A(1);//ʹ����ʱ����A(1)����d,��Ϊ����Ҫ�����µĶ�����˵��õ��ǿ�����ֵ�����
    // �и����⣬��������ββ���const���벻�� error: cannot bind non-const lvalue reference of type ��A&�� to an rvalue of type ��A��
    d.print_count();

}

int main(int argc, char argv) {

    test_construction();

	std::cout << "hello world\n";
	return 0;
}