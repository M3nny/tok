#include <string>
#include <vector>
#include <regex>
#include "tokenizer.hpp"

// --- tokenizer::token ---
tokenizer::token::token(std::string str, size_t start, size_t end) : value(str), start(start), end(end) {}

std::ostream& operator<<(std::ostream& out, const tokenizer::token& t) {
    out << "("
        << "\"" << t.value << "\", "
        << "("  << t.start << ", " << t.end << ")"
        << ")";
    return out;
}

// --- tokenizer ---
tokenizer::tokenizer() : normalize_opts(UTF8PROC_NULLTERM | UTF8PROC_STABLE | UTF8PROC_COMPOSE | UTF8PROC_STRIPMARK | UTF8PROC_CASEFOLD) {}

std::string tokenizer::normalize(const std::string& str, bool strip_whitespaces) const {
    utf8proc_uint8_t* fold_str;

    utf8proc_map(
        reinterpret_cast<const unsigned char*>(str.c_str()),
        0, // length (won't be used if UTF8PROC_NULLTERM is specified)
        &fold_str,
        utf8proc_option_t(this->normalize_opts)
    );

    std::string norm_str(reinterpret_cast<const char*>(fold_str));
    free(fold_str);

    if (strip_whitespaces) {
        std::regex space_regex(R"(\s+)");
        norm_str = std::regex_replace(norm_str, space_regex, " ");
    }
    return norm_str;
}

std::vector<tokenizer::token> tokenizer::pre_tokenize(const std::string& str) const {
    std::vector<token> tokens;
    std::string norm_str = this->normalize(str);
    std::regex token_regex(R"(\w+|[.,:;?!])");

    for (std::sregex_iterator iter(norm_str.begin(), norm_str.end(), token_regex); iter != std::sregex_iterator(); iter++) {
        std::smatch match = *iter;
        std::string token = match.str();
        int start = match.position();
        int end = start + match.length();
        tokens.push_back(tokenizer::token(token, start, end));
    }

    return tokens;
}
