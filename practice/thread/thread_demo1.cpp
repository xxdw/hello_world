//
// Created by 79203 on 2022/6/1.
//
#include<iostream>
#include<thread>
#include<chrono>
#include <vector>
#include <mutex>
#include <atomic>

void func1() {
    std::cout << "executing func1" << std::endl;
    std::cout << "threadid: " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

class Func2 {
public:
    void operator()() {
        std::cout << "executing func2" << std::endl;
        std::cout << "threadid: " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
};

template <typename T>
class ParamTest {
public:
    ParamTest():t(T()){
        std::cout << "ParamTest constructor" <<  std::endl;
    }
    ParamTest(const ParamTest& paramTest) {
        std::cout << "ParamTest copy constructor" << t << std::endl;
    }
    ParamTest& operator=(const ParamTest& paramTest) {
        std::cout << "ParamTest = constructor" << t << std::endl;
    }
    ~ParamTest() {
        std::cout << "ParamTest destructor" << t << std::endl;
    }
public:
    T t;
};

void func3(int x, double y, ParamTest<std::string>& paramTest) {
    std::cout << "executing func3" << std::endl;
    std::cout << "threadid: " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

class Wallet {
public:
    Wallet():money_(0),money2_(0),money3_(0) {}
    std::string money() {
        return "money_: " + std::to_string(money_) + "\n" +
                "money2_: " + std::to_string(money2_) + "\n" +
                "money3_: " + std::to_string(money3_) + "\n";
    }
    void AddMoney(int add_money) {
        for (int i=0;i<add_money;++i) {
            money3_++;
            money2_++;
            std::lock_guard<std::mutex> guard(mutex_);
            money_++;
        }
    }
private:
    int money_;
    std::atomic<int> money2_;
    int money3_;
    std::mutex mutex_;
};

int main() {
    {
        std::thread th1(func1);
        th1.join();
        std::cout << "exit func1" << std::endl;
    }
    {
        Func2 func2;
        std::thread th2(func2);
        th2.join();
        std::cout << "exit func2" << std::endl;
    }
    {
        int x = 1;
        double y = 2.0;
        ParamTest<std::string> param_test;
        std::thread th3(func3, x, y, std::ref(param_test));
        th3.join();
        std::cout << "exit func3" << std::endl;
    }

    {
        Wallet wallet;
        std::vector<std::thread> my_theads;
        for (int i=0; i< 10; ++i) {
            my_theads.push_back(std::thread(&Wallet::AddMoney, &wallet, 100000));
        }
        for (auto& th : my_theads) {
            th.join();
        }
        std::cout << wallet.money() << std::endl;
    }


    return 0;
}