#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
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

// --- tokenizer::byte_pair ---
tokenizer::byte_pair::byte_pair(std::string fst, std::string snd) : first(fst), second(snd) {}

size_t tokenizer::byte_pair::hash::operator()(const byte_pair& bp) const {
    return std::hash<std::string>()(std::string(bp.first + bp.second));
}

bool tokenizer::byte_pair::operator==(const byte_pair &that) const {
    return this->first == that.first && this->second == that.second;
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
        std::regex whitespace_regex(R"(\s+)");
        norm_str = std::regex_replace(norm_str, whitespace_regex, " ");
    }
    return norm_str;
}

std::vector<tokenizer::token> tokenizer::pre_tokenize(const std::string& str) const {
    std::vector<token> tokens;
    std::string norm_str = this->normalize(str);

    // matches an alphanumeric word, preceded or not by a whitespace
    // or a punctuation character
    std::regex token_regex(R"(\s?\w+|[.,:;?!-_'"<>()[\]{}])");

    for (std::sregex_iterator iter(norm_str.begin(), norm_str.end(), token_regex); iter != std::sregex_iterator(); iter++) {
        std::smatch match = *iter;
        std::string token = match.str();
        if (token.at(0) == ' ') token.replace(0, 1, "K"); // TODO: select a better special character
        int start = match.position();
        int end = start + match.length();
        tokens.push_back(tokenizer::token(token, start, end));
    }

    return tokens;
}

std::vector<std::string> tokenizer::string2vec(const std::string& str) const {
    std::vector<std::string> vec_string;
    vec_string.reserve(str.size());

    for (char c : str)
        vec_string.emplace_back(1, c); // constructs the object in place

    return vec_string;
}

std::vector<std::pair<std::string, std::string>> tokenizer::train_bpe(const std::vector<tokenizer::token>& tokens, size_t n_merges) const {
    std::vector<std::vector<std::string>> splits;
    std::vector<std::pair<std::string, std::string>> merge_rules;
    std::unordered_map<byte_pair, size_t, byte_pair::hash> pairs_freqs;


    // initialize splits (e.g. "hi" -> "h", "i")
    for (const auto& token : tokens) {
        splits.push_back(string2vec(token.value));
    }

    for (const auto& split : splits)
        for (size_t i = 0; i < split.size() - 1; i++)
            pairs_freqs[tokenizer::byte_pair(split.at(i), split.at(i+1))]++;

    return merge_rules;
}
