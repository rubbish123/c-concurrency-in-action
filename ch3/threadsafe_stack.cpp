#include <iostream>
#include <thread>
#include <stack>
#include <mutex>

template<typename T>
class threadsafe_stack1{
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack1(){}
    threadsafe_stack1(const threadsafe_stack1 &other){
        // 因为这个对象传过来的时候可能有别的线程在修改，所以先把它锁起来
        std::lock_guard<std::mutex> lock(other.m);
        data=other.data;
    }
    threadsafe_stack1 &operator=(const threadsafe_stack1&)=delete;
    void push(T new_value){
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value));
    }
    // 问题代码
    T pop(){
        std::lock_guard<std::mutex> lock(m);
        auto element=data.top();
        data.pop();
        return element;
    }
    // 危险
    bool empty()const{
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

template<typename T>
class threadsafe_stack{
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack(){}
    threadsafe_stack(const threadsafe_stack &other){
        // 因为这个对象传过来的时候可能有别的线程在修改，所以先把它锁起来
        std::lock_guard<std::mutex> lock(other.m);
        data=other.data;
    }
    threadsafe_stack &operator=(const threadsafe_stack&)=delete;
    void push(T new_value){
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value));
    }
    std::shared_ptr<T> pop(){
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()){
            return nullptr;
        }
        std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
        data.pop();
        return res;
    }
    bool empty()const{
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

void test_threadsafe_stack1(){
    threadsafe_stack1<int> safe_stack;
    safe_stack.push(1);
    std::thread t1([&safe_stack](){
        /*
            这里让t1睡一会儿，整个程序的执行就会出问题
            原因在于我们使用empty的返回值来判断栈是否为空，
            这个返回值不具有时效性
            举个例子假如empty返回栈不为空，但此时刚好系统中断发生，
            然后另一个线程被调度了，它先把栈pop了，此时栈就为空了
            但是我们的消息没有及时更新，还以为栈不空，此时再pop就会出问题
        */
        if(!safe_stack.empty()){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            safe_stack.pop();
        }
    });
    std::thread t2([&safe_stack](){ 
        if(!safe_stack.empty()){
            safe_stack.pop();
        }
    });
    t1.join();
    t2.join();
}

int main(){
    test_threadsafe_stack1();
    return 0;
}