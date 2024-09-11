#include <iostream>
#include <string>
#include <vector>
#include "tok.hpp"

int main() {
    tok tokenizer;
    tokenizer.train_bpe("corpus.txt", 20, true);
    tokenizer.save("pretrained/eng_adjectives_adverbs.bin");
}
