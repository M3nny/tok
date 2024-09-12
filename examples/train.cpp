#include "../tok.hpp"

int main() {
    tok tokenizer;
    tokenizer.train_bpe("corpus.txt", 100, true);
    tokenizer.save("../pretrained/mytokenizer.bin");

    // ---

    tokenizer.load("../pretrained/mytokenizer.bin");
    return 0;
}
