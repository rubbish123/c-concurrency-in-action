#include <iostream>
#include <thread>
#include <mutex>

std::mutex m1,m2;
int a1=0,a2=1;

void dead_lock1(){
    while(true){
        std::cout<<"dead_lock1 begin"<<std::endl;
        m1.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        a1=1024;
        m2.lock();
        a2=2048;
        m2.unlock();
        m1.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        std::cout<<"dead_lock1 end"<<std::endl;
    }
}

void dead_lock2(){
    while(true){
        std::cout<<"dead_lock2 begin"<<std::endl;
        m2.lock();
        // 手动sleep一下造成死锁
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        a2=2048;
        m1.lock();
        a1=1024;
        m1.unlock();
        m2.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        std::cout<<"dead_lock2 end"<<std::endl;
    }
}

// 把针对同一个锁的加锁和解锁操作合并到一个函数中编程原子操作
// 那么当该线程请求m2的时候一定已经释放m1了，就不会造成死锁
void atomic_lock1(){
    std::cout<<"lock1 begin lock"<<std::endl;
    m1.lock();
    a1=1024;
    m1.unlock();
    std::cout<<"lock1 end lock"<<std::endl;
}

void atomic_lock2(){
    std::cout<<"lock2 begin lock"<<std::endl;
    m2.lock();
    a2=2048;
    m2.unlock();
    std::cout<<"lock2 end lock"<<std::endl;
}

void safe_lock1(){
    while(true){
        atomic_lock1();
        atomic_lock2();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void safe_lock2(){
    while(true){
        atomic_lock2();
        atomic_lock1();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void test_dead_lock(){
    std::thread t1(dead_lock1);
    std::thread t2(dead_lock2);
    t1.join();
    t2.join();
}

void test_safe_lock(){
    std::thread t1(safe_lock1);
    std::thread t2(safe_lock2);
    t1.join();
    t2.join();
}

class some_big_object{
private:
    int _data;
public:
    some_big_object(int data):_data(data){}
    some_big_object(const some_big_object &b2):
        _data(b2._data){}
    some_big_object(some_big_object &&b2):_data(std::move(b2._data)){}
    friend std::ostream &operator<<(std::ostream &os,const some_big_object &big_obj){
        os<<big_obj._data;
        return os;
    }
    some_big_object &operator=(const some_big_object &b2){
        if(this==&b2){
            return *this;
        }
        _data=b2._data;
        return *this;
    }

    some_big_object &operator=(const some_big_object &&b2){
        _data=std::move(b2._data);
        return *this;
    }
    // 交换数据
    friend void swap(some_big_object &b1,some_big_object &b2){
        some_big_object temp=std::move(b1);
        b1=std::move(b2);
        b2=std::move(temp);
    }
};

// 这是一个结构，包含成员对象和一个互斥量
class big_object_mgr{
private:
    std::mutex _mtx;
    some_big_object _obj;
public:
    big_object_mgr(int data = 0) :_obj(data) {}
    void printinfo() {
        std::cout << "current obj data is " << _obj << std::endl;
    }
    friend void danger_swap(big_object_mgr &objm1,big_object_mgr &objm2);
    friend void safe_swap(big_object_mgr &objm1,big_object_mgr &objm2);
    friend void safe_swap_scope(big_object_mgr &objm1,big_object_mgr &objm2);
};

void danger_swap(big_object_mgr &objm1,big_object_mgr &objm2){
    std::cout<<"thread["<<std::this_thread::get_id()<<"] begin"<<std::endl;
    if(&objm1==&objm2){
        return;
    }
    std::lock_guard<std::mutex> guard1(objm1._mtx);
    // 延迟一秒让死锁一定出现
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::lock_guard<std::mutex> guard2(objm2._mtx);
    swap(objm1._obj,objm2._obj);
    std::cout<<"thread["<<std::this_thread::get_id()<<"] end"<<std::endl;
}

// 解决方法：同时获取两把锁
void safe_swap(big_object_mgr &objm1,big_object_mgr &objm2){
    std::cout<<"thread["<<std::this_thread::get_id()<<"] begin"<<std::endl;
    if(&objm1==&objm2){
        return;
    }
    // 使用lock方法同时取得两把锁
    std::lock(objm1._mtx,objm2._mtx);
    // 使用lock_guard管理锁的生命周期，但是使用std::adopt_lock声明这把锁是领养的
    // 意思就是不用管加锁，管解锁就行了
    std::lock_guard<std::mutex> guard1(objm1._mtx,std::adopt_lock);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::lock_guard<std::mutex> guard2(objm2._mtx,std::adopt_lock);
    swap(objm1._obj,objm2._obj);
    std::cout<<"thread["<<std::this_thread::get_id()<<"] end"<<std::endl;
}

void safe_swap_scope(big_object_mgr &objm1,big_object_mgr &objm2){
    std::cout<<"thread["<<std::this_thread::get_id()<<"] begin"<<std::endl;
    if(&objm1==&objm2){
        return;
    }
    // c++17提供，可以同时获取多把锁并管理生命周期
    std::scoped_lock(objm1._mtx,objm2._mtx);
    swap(objm1._obj,objm2._obj);
    std::cout<<"thread["<<std::this_thread::get_id()<<"] end"<<std::endl;
}

void test_danger_swap(){
    big_object_mgr objm1(5);
    big_object_mgr objm2(100);
    std::thread t1(danger_swap,std::ref(objm1),std::ref(objm2));
    std::thread t2(danger_swap,std::ref(objm2),std::ref(objm1));
    t1.join();
    t2.join();
    objm1.printinfo();
    objm2.printinfo();
}

void test_safe_swap(){
    big_object_mgr objm1(5);
    big_object_mgr objm2(100);
    std::thread t1(safe_swap,std::ref(objm1),std::ref(objm2));
    std::thread t2(safe_swap,std::ref(objm2),std::ref(objm1));
    t1.join();
    t2.join();
    objm1.printinfo();
    objm2.printinfo();
}

void test_safe_swap_scope(){
    big_object_mgr objm1(5);
    big_object_mgr objm2(100);
    std::thread t1(safe_swap,std::ref(objm1),std::ref(objm2));
    std::thread t2(safe_swap,std::ref(objm2),std::ref(objm1));
    t1.join();
    t2.join();
    objm1.printinfo();
    objm2.printinfo();
}

int main(){
    // test_dead_lock();
    // test_safe_lock();
    // test_danger_swap();
    // test_safe_swap();
    test_safe_swap_scope();
    return 0;
}