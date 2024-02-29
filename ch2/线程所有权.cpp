#include <iostream>
#include <thread>

void some_function(){
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void some_other_function(){
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(){
    // 线程的所有权是可以通过std::move()进行转交的
    std::thread t1(some_function);
    std::thread t2=std::move(t1);
    t1=std::thread(some_other_function);
    std::thread t3;
    t3=std::move(t2);
    // 因为t1已经绑定了一个线程，所以这里把t3的所有权再交给t1会报错
    // ：不能通过赋一个新值给std::thread对象的方式来"丢弃"一个线程
    t1=std::move(t3);
    /*
        因为thread可以被拷贝，所以它可以作为函数中的局部变量被返回
        因为函数返回时会优先调用拷贝构造函数，如果没有拷贝构造，就会调用移动构造
    */
}