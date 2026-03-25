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



class Animal
{
public:
    virtual void name() = 0;
};

class Cat : public Animal
{
public:
    void name() override
    {
        std::cout << "我是小猫老弟！" << std::endl;
    }
};

class Dog : public Animal
{
    void name() override
    {
        std::cout << "我是土狗！" << std::endl;
    }
};








// 简单工厂
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









// // 工厂方法：牺牲了简单，换来了更好的扩展性
// // 工厂方法优点：符合开闭原则（加新产品只需加新工厂类，不改原有代码），客户端只需依赖抽象工厂和抽象产品
// // 工厂方法缺点：类数量多（每个产品都要配一个工厂类），代码更复杂，比简单工厂麻烦

// // 简单工厂：一个工厂类，用 if-else 根据参数创建不同对象，加新产品需修改工厂类，违背开闭原则
// // 工厂方法：每个产品对应一个工厂类（penguin_Factory、Little_Whitey_Factory），通过不同工厂实例创建对象
// // 加新产品只需新增工厂类，不改原有工厂，符合开闭原则
// class abstract_Factory
// {
// public:
//     virtual std::shared_ptr<abstract> create() = 0;
// };

// class penguin_Factory : public abstract_Factory
// {
// public:
//     std::shared_ptr<abstract> create() override
//     {
//         return std::make_shared<penguin>();
//     }
// };

// class Little_Whitey_Factory : public abstract_Factory
// {
// public:
//     std::shared_ptr<abstract> create() override
//     {
//         return std::make_shared<Little_Whitey>();
//     }
// };

// int main()
// {
//     std::shared_ptr<abstract_Factory> ptr(new penguin_Factory());   // 只创建工厂指针，不创建产品，ptr现在指向一个专门生产penguin的工厂
//     std::shared_ptr<abstract> abstract = ptr->create();             // 创建penguin对象，并用基类指针abstract指向它（实例化、隐式上转）
//     abstract->name();

//     ptr.reset(new Little_Whitey_Factory());                         // 重置指向Little_Whitey_Factory
//     abstract = ptr->create();
//     abstract->name();

//     return 0;
// }










// 抽象工厂：围绕⼀个超级工厂创建其他⼯⼚（一个工厂生产一整套相关产品）。
// 每个⽣成的⼯⼚按照⼯⼚模式提供对象将⼯⼚抽象成两层，抽象⼯⼚&具体⼯⼚⼦类， 在⼯⼚⼦类种⽣产不同类型的⼦产品
// 优点：符合开闭原则（加新“主题”只需新增一个工厂，不改旧代码），能保证同一套产品是一起的（一致性强）
// 缺点：类非常多（产品族越多，工厂和产品类爆炸），系统复杂，扩展新产品种类非常麻烦（要改所有工厂）
class Factory
{
public:
    virtual std::shared_ptr<abstract> getAbstract(const std::string& name) = 0;
    virtual std::shared_ptr<Animal> getAnimal(const std::string &name) = 0;
};


class abstract_Factory : public Factory
{
public:
    std::shared_ptr<Animal> getAnimal(const std::string& name) override
    {
        return std::shared_ptr<Animal>();
    }

    std::shared_ptr<abstract> getAbstract(const std::string& name) override
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


class Animal_Factory : public Factory
{
public:
    std::shared_ptr<abstract> getAbstract(const std::string& name) override
    {
        return std::shared_ptr<abstract>();
    }

    std::shared_ptr<Animal> getAnimal(const std::string& name) override
    {
        if(name == "小猫")
        {
            return std::make_shared<Cat>();
        }
        else
        {
            return std::make_shared<Dog>();
        }
    }
};

class FactoryProducer
{
public:
    static std::shared_ptr<Factory> create(const std::string& name)
    {
        if(name == "抽象")
        {
            return std::make_shared<abstract_Factory>();
        }
        else
        {
            return std::make_shared<Animal_Factory>();
        }
    }
};

int main()
{
    std::shared_ptr<Factory> ptr = FactoryProducer::create("抽象");
    std::shared_ptr<abstract> p = ptr->getAbstract("咕咕嘎嘎");
    p->name();

    p = ptr->getAbstract("小白子");
    p->name();



    std::shared_ptr<Factory> ptr2 = FactoryProducer::create("动物");
    std::shared_ptr<Animal> animal = ptr2->getAnimal("小猫");
    animal->name();

    animal = ptr2->getAnimal("小狗");
    animal->name();

    return 0;
}