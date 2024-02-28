#include <iostream>
#include <thread>
#include <string>
#include <memory>

void f(int i,std::string s){
    std::cout<<s<<std::endl;
}

class A{
public:
    int num=5;
    // A():num(5){}
};

void k(A &a){
    a.num=20;
}

class X{
public:
    // 这个函数一定要有定义，不然把它传经thread的时候会找不到引用报错
    void do_lengthy_work(){
        std::cout<<"do_length_work()"<<std::endl;
    };
};

class big_data{
public:
    int prepare_data;
};

void process_big_data(std::unique_ptr<big_data>){}

int main(){
    std::thread t1(f,3,"hello");
    t1.join();
    char buf[32];
    /*
        给线程传递参数的时候它只是简单地把参数复制一份
        至于类型匹不匹配，能不能转换成功它是不管的
    */
    sprintf(buf,"%s","hello world");
    // std::thread t2(f,3,buf);
    // t2.join();
    // 所以最保险的办法就是在传参的时候手动转换
    std::thread t3(f,3,std::string(buf));
    t3.join();
    A a;
    std::cout<<"a.num="<<a.num<<std::endl;
    /*
        注意k()需要一个非常量引用作为参数，但是thread传参是不管你参数类型的，只是无脑复制
        而且这个参数还是以右值地方式进行拷贝传递的，这是过不了编译的
        因此当k()需要一个引用作为参数的时候，在给thread
        传参的时候就不能直接传，而是要用std::ref(xxx)，就是强制获取xxx的引用
    */
    // std::thread t4{k,a};
    std::thread t4{k,std::ref(a)};
    t4.join();
    std::cout<<"a.num="<<a.num<<std::endl;

    X my_x;
    /*
        如果传的参数是普通函数时，&加不加都无所谓，
        如果传的是类中的方法时，一定要加&，
        参数&my_x是因为类的成员方法都有一个隐藏参数，是一个顶const的该类类型指针，
        用来指向调用这个方法的对象
    */
    std::thread t5(&X::do_lengthy_work,&my_x);
    t5.join();

    /*
        std::unique_ptr类型不能被拷贝但是可以被移动
        因此在传递这类参数时使用std::move(xxx)来把这个对象的所有权
        转交给子线程
    */
    std::unique_ptr<big_data> p(new big_data);
    p->prepare_data=42;
    std::thread t6(process_big_data,std::move(p));
    t6.join();
    return 0;
}