#include <iostream>
#include <string>
#include "tokenizer.hpp"


int main() {
    tokenizer tokenizer;
    std::string str = "HèLlO HòW Arè YòU dOìng?";
    std::string str2 = "hello how are you?";
    std::string str_norm = tokenizer.normalize(str);


    std::cout << "original:\t" << str << std::endl;
    std::cout << "normalized:\t" << str_norm << std::endl;
}
