#include <iostream>
#include <thread>
#include <vector>

void f(int i){}

int main(){
    std::vector<std::thread> threads;
    for(int i=0;i<10;i++){
        /*
            在vector中存储线程不能调用push_back，
            因为push_back会调用容器内对象的拷贝构造
            而thread没有拷贝构造函数
            所以我们要使用emplace_back直接移动
        */
        // threads.push_back(f,i);
        threads.emplace_back(f,i);
    }
    for(auto &entry:threads){
        entry.join();
    }
}