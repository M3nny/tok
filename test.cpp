#include <iostream>
#include <string>
#include "tokenizer.hpp"


int main() {
    tokenizer tokenizer;
    std::string str = "HèLlO,     HòW Arè YòU dOìng?";
    std::string str_norm = tokenizer.normalize(str);
    auto tokens = tokenizer.pre_tokenize(str);
    auto merge_rules = tokenizer.train_bpe(tokens, 20);


    std::cout << "original:\t" << str << std::endl;
    std::cout << "normalized:\t" << str_norm << std::endl;
    std::cout << "tokens: " << std::endl;
    for (auto& token : tokens) {
        std::cout << token << std::endl;
    }

}
