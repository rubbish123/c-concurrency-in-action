#include <iostream>

using namespace std;

class Test {
public:
  Test()
    {
        cout << "构造 Test 类对象 " << endl;
    }
    ~Test()
    {
        cout << "析构 Test 类对象 " << endl;
    }
};

void f1()
{
    cout << "\nf1() 开始 ";
    Test t;
    throw 100;
    cout << "\nf1() 结束 ";
}

// f2() 函数的作用是调用 f1()
void f2()
{
    cout << "\nf2() 开始 ";
    f1();
    cout << "\nf2() 结束 ";
}

void f3()
{
    cout << "f3() 开始 ";
    try {
        f2();
    }
    catch (int i) {
        cout << "\n捕获异常：" << i;
    }
    cout << "\nf3() 结束";
}

int main()
{
    f3();
    return 0;
}

