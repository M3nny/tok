#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.hpp"

int main() {
    tokenizer tokenizer;
    tokenizer.train_bpe("corpus.txt", 20, true);
    tokenizer.save("pretrained/eng_adjectives_adverbs.bin");
}
