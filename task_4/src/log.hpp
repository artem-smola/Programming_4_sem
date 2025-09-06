#pragma once
#include <iostream>

class Log{
public:
    explicit Log(std::string text);
    std::string GetText();
    void Show();
private:
    std::string text_;
};