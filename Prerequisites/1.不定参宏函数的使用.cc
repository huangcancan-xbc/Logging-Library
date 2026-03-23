// 不定参函数的使用

#include <iostream>
using namespace std;

int main()
{
    // FILE：当前源文件名（字符串字面量）
    // LINE：当前代码所在的行号（整数）
    cout << "文件: " << __FILE__ << "\n";
    cout << "行号: " << __LINE__ << "\n"; // 输出这一行的行号
    // 常用于日志、断言、调试信息
    cout << "位置: " << __FILE__ << ":" << __LINE__ << "\n";


    printf("[%s:%d] %s-%d\n", __FILE__, __LINE__, "小米里的大麦", 666666);

    return 0;
}