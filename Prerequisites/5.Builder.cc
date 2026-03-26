#include <iostream>
#include <memory>


// 产品类
class Computer
{
public:
    void setBoard(const std::string& board)
    {
        _board = board;
    }

    void setDisplay(const std::string& display)
    {
        _display = display;
    }

    void showParamaters()
    {
        std::string param = "Computer Paramaters:\n";
        param += "\tBoard: " + _board + "\n";
        param += "\tDisplay " + _display + "\n";
        param += "\tOS: " + _os + "\n";
        std::cout << param << std::endl;
    }

    virtual void setOS() = 0;   // 不同电脑的操作系统不同，由子类决定（纯虚）

protected:
    std::string _board;     // 主板
    std::string _display;   // 显示器
    std::string _os;        // 操作系统
};


// 具体产品
class MacBook : public Computer
{
public:
    void setOS() override
    {
        _os = "Mac OS";     // MacBook固定使用Mac OS
    }
};


// 抽象建造者
class Builder
{
public:
    virtual void buildBoard(const std::string &board) = 0;
    virtual void buildDisplay(const std::string &board) = 0;
    virtual void buildOS() = 0;
    virtual std::shared_ptr<Computer> build() = 0;
};


// 具体建造者
class MacBookBuilder : public Builder
{
public:
    // 构造时创建具体产品对象
    MacBookBuilder()
        : _computer(new MacBook())
    {
        
    }

    void buildBoard(const std::string &board) override
    {
        _computer->setBoard(board);
    }

    void buildDisplay(const std::string &display) override
    {
        _computer->setDisplay(display);
    }

    void buildOS() override
    {
        _computer->setOS();
    }

    std::shared_ptr<Computer> build()
    {
        return _computer;       // 返回构建好的电脑产品
    }

private:
    std::shared_ptr<Computer> _computer;
};


// 指挥者
class Director
{
public:
    // 接收一个建造者
    Director(Builder* builder)
        : _builder(builder)
    {
        
    }

    // 统一构建流程，按步骤展开
    void construct(const std::string& board, const std::string& display)
    {
        _builder->buildBoard(board);        // 1.装主板
        _builder->buildDisplay(display);    // 2.装显示器
        _builder->buildOS();                // 3.装OS
    }

private:
    std::shared_ptr<Builder> _builder;      // 使用建造者来完成具体构建
};


int main()
{
    // 1.创建具体的建造者
    Builder *builder = new MacBookBuilder();

    // 2.创建指挥者并传入建造者
    std::unique_ptr<Director> director(new Director(builder));

    // 3.指挥者控制构建流程的步骤
    director->construct("mac主板", "三星显示器");

    // 4.从建造者获取最终产品
    std::shared_ptr<Computer> computer = builder->build();

    // 5.使用产品（现实产品相关信息）
    computer->showParamaters();

    return 0;
}