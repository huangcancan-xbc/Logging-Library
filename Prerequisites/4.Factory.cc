#include <iostream>
#include <memory>
#include <string>

// 简单工厂的优点：
// 代码简单，容易实现
// 客户端不需要知道具体产品类，只依赖工厂和抽象接口
// 集中管理对象的创建，方便统一控制

// 开闭原则：对扩展开放，对修改关闭，即加新功能尽量不修改旧代码
// 简单工厂里，工厂类用 if-else 或 witch 决定创建什么样的对象
// 要加新产品，就得改工厂类，加判断，这就动了旧代码，改错了容易把原来好的功能搞坏
// 所以简单工厂违背了开闭原则

class abstract
{
public:
    virtual void name() = 0;
    virtual ~abstract() = default;
};


class penguin : public abstract
{
public:
    void name() override
    {
        std::cout << "我是咕咕嘎嘎！" << std::endl;
    }
};


class Little_Whitey : public abstract
{
public:
    void name() override
    {
        std::cout << "我是小白子！" << std::endl;
    }
};


class Abstract_Factory
{
public:
    static std::shared_ptr<abstract> create(const std::string& name)
    {
        if (name == "咕咕嘎嘎")
        {
            return std::make_shared<penguin>();
        }
        else
        {
            return std::make_shared<Little_Whitey>();
        }
    }
};


int main()
{
    std::shared_ptr<abstract> cx = Abstract_Factory::create("咕咕嘎嘎");
    cx->name();

    cx = Abstract_Factory::create("小白子");
    cx->name();

    return 0;
}