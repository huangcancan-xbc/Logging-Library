#include <iostream>

// // 经典饿汉：以空间换时间
// // 程序一开始就把单例对象创建好，不用等第一次调用
// // 不管用不用，程序启动时就创建
// // 如果对象很重，或者依赖别的初始化顺序，可能不合适
// class Singleton
// {
// public:
//     static Singleton& getInstance()
//     {
//         return _eton;
//     }

//     int getData()
//     {
//         return _data;
//     }

// private: 

//     // 私有构造
//     Singleton()
//         : _data(666)
//     {
//         std::cout << "单例对象构造！" << std::endl;
//     }

//     ~Singleton()
//     {

//     }

//     // 禁用拷贝
//     Singleton(const Singleton &) = delete;
//     Singleton& operator=(const Singleton &) = delete;

//     static Singleton _eton;         // 声明唯一实例，程序开始就创建
//     int _data;
// };

// Singleton Singleton::_eton;         // 类内声明，类外定义








// 懒汉单例（Meyer's Singleton）—— 延迟加载，一个对象到用的时候才进行实例化
//懒加载：第一次调用时创建
//C++11 起线程安全
//不需要手动 new
//不用手动释放内存
class Singleton
{
public:
    static Singleton &getInstance()
    {
        static Singleton _eton;         // 把单例对象放到函数内部作为 局部 static 变量
        return _eton;
    }

    int getData()
    {
        return _data;
    }

private:
    // 私有构造
    Singleton()
        : _data(666)
    {
        std::cout << "单例对象构造！" << std::endl;
    }

    ~Singleton()
    {
    }

    // 禁用拷贝
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    int _data;
};

int main()
{
    std::cout << Singleton::getInstance().getData() << std::endl;

    return 0;
}