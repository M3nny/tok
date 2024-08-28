#include <utf8proc.h>
#include <string>

class tokenizer {
public:
    tokenizer();
    std::string normalize(std::string str);
private:
    int normalize_opts;
};
