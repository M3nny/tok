#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.hpp"

int main() {
    tokenizer tokenizer;
    tokenizer.load("pretrained/eng_adjectives_adverbs.bin");
    // for (auto rule : tokenizer.get_merge_rules()) {
    //     std::cout << rule.first << " + " << rule.second << std::endl;
    // }
    std::vector<std::string> res = tokenizer.tokenize("hi, how are you doing?");
    for (auto a : res) {
        std::cout << a << " | ";
    }
    std::cout << std::endl;
}
