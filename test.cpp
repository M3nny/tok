#include <iostream>
#include <string>
#include "tokenizer.hpp"


int main() {
    tokenizer tokenizer;
    auto tokens = tokenizer.pre_tokenize("corpus.txt", true);
    tokenizer.train_bpe(tokens, 20);
}
