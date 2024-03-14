#include <iostream>
#include <thread>

int *ptr;

void f(int &i){
    ptr=&i;
}

int main(){
    int a=42;
    f(a);
    std::cout<<*ptr<<std::endl;
    return 0;
}