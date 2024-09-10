#include <iostream>
#include <string>
#include "tokenizer.hpp"

int main() {
    tokenizer tokenizer;
    tokenizer.load("pretrained/eng_adjectives_adverbs.bin");
    for (auto rule : tokenizer.get_merge_rules()) {
        std::cout << rule.first << " + " << rule.second << std::endl;
    }
}
