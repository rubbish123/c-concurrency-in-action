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

class thread_guard{
private:
    // 注意thread对象不能被拷贝，所以这里要声明为引用
    std::thread &t;
public:
    explicit thread_guard(std::thread &t_):t(t_){
        std::cout<<"in thread_guard()"<<std::endl;
    }
    // RAII思想，thread_guard对象销毁时一定会等t执行结束
    ~thread_guard(){
        if(t.joinable()){
            std::cout<<"in ~thread_guard()"<<std::endl;
            t.join();
        }
    }
    thread_guard(thread_guard const&)=delete;
    thread_guard &operator=(const thread_guard&)=delete;
};

int main(){
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    try{
        // 对象g被销毁之前一定会等my_thread执行结束
        thread_guard g(my_thread);
        // 即使这里发生了异常，因为g在栈上，
        // 发生异常会跳出这个代码块，所以g一定会被销毁
        throw 100;
    }
    catch(int i){
        std::cout<<"catch exception: "<<i<<std::endl;
    }
    return 0;
}