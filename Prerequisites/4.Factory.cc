#include <iostream>
#include <memory>
#include <string>

// 简单工厂的优点：
// 代码简单，容易实现
// 客户端不需要知道具体产品类，只依赖工厂和抽象接口
// 集中管理对象的创建，方便统一控制

// 开闭原则：对扩展开放，对修改关闭，即加新功能尽量不修改旧代码
// 简单工厂里，工厂类用 if-else 或 switch 决定创建什么样的对象
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


// class Abstract_Factory
// {
// public:
//     static std::shared_ptr<abstract> create(const std::string& name)
//     {
//         if (name == "咕咕嘎嘎")
//         {
//             return std::make_shared<penguin>();
//         }
//         else
//         {
//             return std::make_shared<Little_Whitey>();
//         }
//     }
// };
//
// int main()
// {
//     std::shared_ptr<abstract> cx = Abstract_Factory::create("咕咕嘎嘎");
//     cx->name();

//     cx = Abstract_Factory::create("小白子");
//     cx->name();

//     return 0;
// }





// 工厂方法
// 简单工厂：一个工厂类，用 if-else 根据参数创建不同对象，加新产品需修改工厂类，违背开闭原则
// 工厂方法：每个产品对应一个工厂类（penguin_Factory、Little_Whitey_Factory），通过不同工厂实例创建对象
// 加新产品只需新增工厂类，不改原有工厂，符合开闭原则
class abstract_Factory
{
public:
    virtual std::shared_ptr<abstract> create() = 0;
};

class penguin_Factory : public abstract_Factory
{
public:
    std::shared_ptr<abstract> create() override
    {
        return std::make_shared<penguin>();
    }
};

class Little_Whitey_Factory : public abstract_Factory
{
public:
    std::shared_ptr<abstract> create() override
    {
        return std::make_shared<Little_Whitey>();
    }
};

int main()
{
    std::shared_ptr<abstract_Factory> ptr(new penguin_Factory());   // 只创建工厂指针，不创建产品，ptr现在指向一个专门生产penguin的工厂
    std::shared_ptr<abstract> abstract = ptr->create();             // 创建penguin对象，并用基类指针abstract指向它（实例化、隐式上转）
    abstract->name();

    ptr.reset(new Little_Whitey_Factory());                         // 重置指向Little_Whitey_Factory
    abstract = ptr->create();
    abstract->name();

    return 0;
}