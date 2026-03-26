// 代理模式：假设房东要把一个房子通过中介租出去
// 不直接访问真实对象，而是通过一个代理对象间接访问，并在中间做增强控制
#include <iostream>
#include <string>


// 抽象租房
class RentHouse
{
public:
    virtual void rentHouse() = 0;
};


// 真实对象：房东才是真正要把房子租出去的人
class Landlord : public RentHouse
{
public:
    void rentHouse() override
    {
        std::cout << "把房子租出去\n" << std::endl;
    }
};


// 代理对象：中介：不直接租房，而是帮房东处理前后流程
class Agent : public RentHouse
{
public:
    void rentHouse() override
    {
        // 前置增强
        std::cout << "发布招租启示\n";
        std::cout << "带人看房\n";

        // 调用真实对象
        _landlord.rentHouse();

        // 后置增强
        std::cout << "负责租后维修\n";
    }

private:
    Landlord _landlord;     // 持有真实对象
};


int main()
{
    Agent agent;
    agent.rentHouse();

    return 0;
}