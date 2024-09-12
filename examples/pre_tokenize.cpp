#include <iostream>
#include <vector>
#include "../tok.hpp"

int main() {
    tok tokenizer;
    std::string str = "HèlLò,   HoW   aRè yoU dOing?";
    std::cout << str << std::endl;
    std::cout << tokenizer.normalize(str) << std::endl;

    // automatically normalizes
    std::vector<tok::word> pre_tokenized_str = tokenizer.pre_tokenize(str);

    for (const auto& piece : pre_tokenized_str)
        std::cout << piece << std::endl;

    return 0;
}
