#include <utf8proc.h>
#include <string>

class tokenizer {
public:
    tokenizer();
    utf8proc_uint8_t* normalize(utf8proc_uint8_t str[]);
private:
    int normalize_opts;
};
