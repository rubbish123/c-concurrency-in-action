#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx1;
int shared_data=100;

void use_lock(){
    while(true){
        mtx1.lock();
        shared_data++;
        std::cout<<"current thread is "<<std::this_thread::get_id()<<std::endl;
        std::cout<<"shared data is "<<shared_data<<std::endl;
        mtx1.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(10000));
    }
}

void test_lock(){
    std::thread t1(use_lock);
    std::thread t2([](){
        while(true){
            mtx1.lock();
            shared_data--;
            std::cout<<"current thread is "<<std::this_thread::get_id()<<std::endl;
            std::cout<<"shared data is "<<shared_data<<std::endl;
            mtx1.unlock();
            std::this_thread::sleep_for(std::chrono::microseconds(10000));
        }
    });
    t1.join();
    t2.join();
}

int main(){
    test_lock();
    return 0;
}