// 不定参函数的使用


// #include <iostream>
// using namespace std;
// int main()
// {
//     // FILE：当前源文件名（字符串字面量）
//     // LINE：当前代码所在的行号（整数）
//     cout << "文件: " << __FILE__ << "\n";
//     cout << "行号: " << __LINE__ << "\n"; // 输出这一行的行号
//     // 常用于日志、断言、调试信息
//     cout << "位置: " << __FILE__ << ":" << __LINE__ << "\n";


//     printf("[%s:%d] %s-%d\n", __FILE__, __LINE__, "小米里的大麦", 666666);

//     return 0;
// }





// #include <iostream>
// using namespace std;

// C/C++ 里相邻的字符串字面量会自动拼接：
// printf("[%s:%d]" "错误码=%d\n", __FILE__, __LINE__, err);
// 等价于：printf("[%s:%d]错误码=%d\n", __FILE__, __LINE__, err);

// format 是第一个参数，一般是字符串，...表示可变参数，__VA_ARGS__是...的占位符
// #define LOG(format, ...) printf("[%s:%d]" format, __FILE__, __LINE__, __VA_ARGS__);

// int main()
// {
//     LOG("%s-%d\n", "小米里的大麦", 6666666);
//     // 等价于：printf("[%s:%d]%s-%d\n", __FILE__, __LINE__, "小米里的大麦", 6666666);

//     return 0;
// }







// #include <iostream>
// using namespace std;

// // #define LOG(format, ...) printf("[%s:%d]" format, __FILE__, __LINE__, __VA_ARGS__);
// // 这会导致一个问题：但使用：LOG("小米里的大麦"); 会报错，原因是没有可变参数时，格式化中保留的“__LINE__,”这个逗号报错

// // 解决办法：这里的 ##__VA_ARGS__ 会在 __VA_ARGS__ 为空时自动去掉多余的逗号
// #define LOG(format, ...) printf("[%s:%d]" format, __FILE__, __LINE__, ##__VA_ARGS__);

// int main()
// {
//     LOG("小米里的大麦\n");
//     // 等价于：printf("[%s:%d]小米里的大麦\n", __FILE__, __LINE__);

//     return 0;
// }









// man va_arg: C语言中处理可变参数函数的四个宏/函数
// #include <stdarg.h>                          // 头文件
// void va_start(va_list ap, last);             // 初始化va_list，开始读取变参（必须第一个调用），参数：va_list变量、最后一个固定参数的名字
// type va_arg(va_list ap, type);               // 取出下一个参数，并指定其类型（循环调用取参数），参数：va_list变量、期望的参数类型
// void va_end(va_list ap);                     // 清理 va_list，结束变参读取（必须最后一个调用），参数：va_list变量
// void va_copy(va_list dest, va_list src);     // 拷贝一份 va_list（C99+，需要“备份”参数列表时用），参数：目标va_list、源va_list

// #define _GNU_SOURCE /* See feature_test_macros(7) */
#include <iostream>
#include <stdarg.h>
using namespace std;

#define LOG(format, ...) printf("[%s:%d]" format, __FILE__, __LINE__, ##__VA_ARGS__);

// C语言中不定参数的使用，不定参数据的访问
// count表示后面有多少个可变参数，然后逐个把这些参数按 int 取出来并打印
void printNum(int count, ...)
{
    va_list ap;             // 定义一个变量，用来访问可变参数（我的理解是指针/句柄，官方解释叫游标）
    va_start(ap, count);    // 表示找到可变参数的位置，而这里的count表示最后一个非可变参数，要从的他的后面开始读可变参数

    for (int i = 0; i < count; i++)
    {
        // 从当前可变参数位置取出一个参数，按照 int 类型解释它，然后把 ap 移动到下一个参数，可以传不同的类型，但是要保证按正确类型读取
        int num = va_arg(ap, int);

        printf("参数：[%d]:%d\n", i, num);
    }

    va_end(ap);
}

// man vasprintf
// vasprintf：自动分配足够大的内存，把格式化结果存到 res 中，返回字符数，-1表示失败
// 第1个参数：char **strp —— 传 &res（char* 的地址），vasprintf 会自动分配内存并把指针写到这里
// 第2个参数：const char *fmt —— 自己写的格式字符串（如 "%d %s\n"）
// 第3个参数：va_list ap —— 固定传已初始化的 va_list（用 va_start 拿到的参数列表）
void myprintf(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    char *res;
    int ret = vasprintf(&res, format, ap);
    if(ret != -1)
    {
        printf("%s", res);
        free(res);
    }

    va_end(ap);
}

int main()
{
    // LOG("小米里的大麦\n");
    // printNum(2, 123, 123456);
    // printNum(5, 10, 20, 30, 40, 50);

    myprintf("%s-%d\n", "小米里的大麦", 666666);
    myprintf("小米里的大麦\n");

    return 0;
}