#include <iostream>
#include <thread>

void do_something(){
    std::cout<<"something"<<std::endl;
}

class Task{
public:
    void operator()(){
        do_something();
    }
};

int main(){
    std::thread t1(do_something);
    std::thread t2{Task()};
    // 下面这种语法会被解析成函数声明，所以必要的时候还是使用花括号构造
    // std::thread t2(Task());
    // 使用lambda表达式避免以上问题
    std::thread t3([](){
        do_something();
    });
    t1.join();
    t2.join();
    t3.join();
    return 0;
}