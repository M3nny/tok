#include "tokenizer.h"

tokenizer::tokenizer() : normalize_opts(UTF8PROC_NULLTERM | UTF8PROC_STABLE | UTF8PROC_COMPOSE | UTF8PROC_STRIPMARK | UTF8PROC_CASEFOLD){}

utf8proc_uint8_t* tokenizer::normalize(utf8proc_uint8_t str[]) {
    utf8proc_uint8_t *fold_str;

    utf8proc_map(
        str,
        0,
        &fold_str,
        utf8proc_option_t(this->normalize_opts)
    );
    return fold_str;
    // TODO: free(fold_str);
}
