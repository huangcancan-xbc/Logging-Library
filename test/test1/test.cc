// 测试util.hpp工具类

#include "../../source/util.hpp"

int main()
{
    std::cout << mylog::util::Date::now() << std::endl;
    std::string pathname = "./abc/def/test.txt";
    mylog::util::File::createDirectory(mylog::util::File::getPath(pathname));

    return 0;
}