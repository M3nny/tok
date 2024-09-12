#include <iostream>
#include <vector>
#include "../tok.hpp"

int main() {
    tok tokenizer;
    tokenizer.load("../pretrained/eng_adjectives_adverbs_30k.bin");

    std::vector<std::string> tokenizer_str = tokenizer.tokenize("i've finished tokenizing this string!");
    std::vector<size_t> ids = tokenizer.tokens_to_ids(tokenizer_str);
    std::vector<std::string> tokens = tokenizer.ids_to_tokens(ids);

    for (size_t i = 0; i < tokenizer_str.size(); i++) {
        std::cout << tokenizer_str.at(i) << "\t" << ids.at(i) << "\t" << tokens.at(i) << std::endl;
    }

    return 0;
}
