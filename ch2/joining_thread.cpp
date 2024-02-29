#include <iostream>
#include <thread>

class joining_thread{
private:
    std::thread _t;
public:
    joining_thread() noexcept=default;
    template<typename Callable,typename ... Args>
    explicit joining_thread(Callable &&func,Args&& ...args):
        _t(std::forward<Callable>(func),std::forward<Args>(args)...){}
    explicit joining_thread(std::thread t)noexcept:_t(std::move(t)){}
    joining_thread(joining_thread &&other)noexcept:_t(std::move(other._t)){}
    joining_thread &operator=(joining_thread &&other)noexcept{
        if(joinable()){
            join();
        }
        _t=std::move(other._t);
        return *this;
    }
    joining_thread &operator=(joining_thread other)noexcept{
        if(joinable()){
            join();
        }
        _t=std::move(other._t);
        return *this;
    }
    ~joining_thread()noexcept{
        if(joinable()){
            join();
        }
    }
    void swap(joining_thread &other)noexcept{
        _t.swap(other._t);
    }
    std::thread::id   get_id() const noexcept {
        return _t.get_id();
    }

    bool joinable() const noexcept {
        return _t.joinable();
    }

    void join() {
        _t.join();
    }

    void detach() {
        _t.detach();
    }

    std::thread& as_thread() noexcept {
        return _t;
    }

    const std::thread& as_thread() const noexcept {
        return _t;
    }
};