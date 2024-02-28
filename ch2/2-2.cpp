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

int main(){
    int some_local_state=0;
    func my_func{some_local_state};
    std::thread t(my_func);
    try{
        /*
            就是说虽然本意是想等线程t结束的，但是如果主线程
            抛了异常，那么它就会执行catch{}中的内容然后结束了，
            所以为了保险，需要在catch中再t.join()一次
        */
        throw "catch my exception";
    }
    catch(const char *e_msg){
        std::cout<<e_msg<<std::endl;
        std::cout<<"in catch"<<std::endl;
        t.join();
        throw;
    }
    t.join();
}