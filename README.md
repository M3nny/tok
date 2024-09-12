# 🍫 tok

tok is a [Byte Pair Encoding](https://en.wikipedia.org/wiki/Byte_pair_encoding) tokenizer used for splitting text into tokens which can then be encoded into ids.

## Features
- Custom string normalization
- Easy to use API for interacting with text tokenization
- Serializable vocabulary and merge rules

## Installation
tok uses [utf8proc](https://github.com/JuliaStrings/utf8proc) for normalizing strings and [cereal](https://github.com/USCiLab/cereal) for serializing, you can also install them through your package manager.

Using a debian based distro:
```bash
apt install libutf8proc-dev libcereal-dev
```

## Usage
1. Clone the repository with `git clone https://github.com/M3nny/tok`
2. Run `make` inside the cloned repository, it will create a `build` directory with the static library
3. Include it in you project (you also have to link **utf8proc**)

```bash
g++ -std=c++11 -c program.cpp -o program.o
g++ -std=c++11 program.o -o program -L path_to/tok/build -l tok -l utf8proc
```

### Brief example
```c++
#include <vector>
#include "tok.hpp"

int main() {
    tok tokenizer;
    tokenizer.load("pretrained/eng_adjectives_adverbs_30k.bin");
    std::string str = "i've just bought a melon!";

    std::vector<std::string> tokenized_str = tokenizer.tokenize(str);
    // ["i", "'", "ve", "Ķjust", "Ķbought", "Ķa", "Ķmel", "on", "!", "<|eot|>"]

    return 0;
}
```

> [!IMPORTANT]
> The API documentation can be found in `tok.hpp` and some examples are listed inside the `examples` folder.
> 
> You can find pretrained vocabularies inside `pretrained`.
