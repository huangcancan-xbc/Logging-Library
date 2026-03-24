#include <iostream>

// 递归终止函数，没有参数时，只输出一个换行
void xprintf()
{
    std::cout << std::endl;
}

// 先输出当前第一个参数v，再把剩下的参数 args... 递归传给下一次xprintf，当没有剩余参数时输出换行
// 这里的Args是一个模板参数包，表示有零个或多个类型参数
// Args... 表示一组类型。
// args... 表示一组函数参数。
// Args&&... args 表示对参数包 Args 中的每个类型，都声明一个对应的右值引用参数
template <typename T, typename... Args>
void xprintf(const T &v, Args &&...args)
{
    std::cout << v;

    // sizeof... 是一个独立运算符：sizeof... (参数包)。后面必须跟一个参数包
    // 普通sizeof返回的是字节数，这里返回的是参数个数！这有点想main函数的第二个参数：char *argv[]
    if ((sizeof...(args)) > 0)
    {
        xprintf(std::forward<Args>(args)...);
    }
    else
    {
        xprintf();
    }
}

int main()
{
    xprintf("小米里的大麦");
    xprintf("小米里的大麦", " minbit");
    xprintf("小米里的大麦", " minbit", " debug");

    return 0;
}