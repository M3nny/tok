#include <iostream>
#include <string>
#include "tokenizer.h"

int main() {
    utf8proc_uint8_t str[] = "HèLlO HòW Arè YòU";

    tokenizer tokenizer;
    printf("%s\n", tokenizer.normalize(str));
}
