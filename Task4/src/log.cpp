#include "log.hpp"

Log::Log(std::string text) : text_(text){}

std::string Log::GetText() { return text_; }

void Log::Show() {
    std::cout << text_ << std::endl;
}