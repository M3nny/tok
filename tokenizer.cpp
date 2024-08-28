#include <string>
#include "tokenizer.hpp"

tokenizer::tokenizer() : normalize_opts(UTF8PROC_NULLTERM | UTF8PROC_STABLE | UTF8PROC_COMPOSE | UTF8PROC_STRIPMARK | UTF8PROC_CASEFOLD) {}

std::string tokenizer::normalize(std::string str) {
    utf8proc_uint8_t* fold_str;

    utf8proc_map(
        reinterpret_cast<const unsigned char*>(str.c_str()),
        0, // won't be used if UTF8PROC_NULLTERM is specified
        &fold_str,
        utf8proc_option_t(this->normalize_opts)
    );

    std::string normalized_string(reinterpret_cast<const char*>(fold_str));
    free(fold_str);
    return normalized_string;
}
