#include <iostream>
#include <string>
#include <vector>
#include "tok.hpp"

int main() {
    tok tokenizer;
    tokenizer.load("pretrained/eng_adjectives_adverbs.bin");
    tokenizer.tokenize("hi, how are you doing?");
}
