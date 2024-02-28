#include <iostream>
#include <thread>

void do_something(int i){}

struct func{
    int &i;
    func(int &i_):i(i_){}
    void operator()(){
        for(unsigned j=0;j<1000000;++j){
            do_something(i);
        }
    }
};

void oops(){
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    /*
        这里已经把my_thread从主线程中分离了，
        所以主线程结束时，my_thread可能还在运行
        但是这时some_local_state变量已经被销毁了，
        所以my_thread线程中会存在悬空引用
    */
    my_thread.detach();
}