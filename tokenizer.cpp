#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <functional>

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
tokenizer::byte_pair::byte_pair() : first(""), second("") {}

tokenizer::byte_pair::byte_pair(std::string fst, std::string snd) : first(fst), second(snd) {}

size_t tokenizer::byte_pair::hash::operator()(const byte_pair& bp) const {
    return std::hash<std::string>()(std::string(bp.first + bp.second));
}

bool tokenizer::byte_pair::operator==(const byte_pair &that) const {
    return this->first == that.first && this->second == that.second;
}

template <class Archive>
void tokenizer::byte_pair::serialize(Archive& archive) {
    archive(first, second);
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

std::vector<tokenizer::token> tokenizer::pre_tokenize(const std::string& str, bool file_path) const {
    std::vector<token> tokens;
    std::string norm_str;

    if (file_path) {
        std::ifstream ifs(str);
        if (!ifs.is_open()) throw std::runtime_error("Unable to read file: " + str);

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string file_content = buffer.str();
        norm_str = this->normalize(file_content);
    } else {
        norm_str = this->normalize(str);
    }

    // matches an alphanumeric word, preceded or not by a whitespace
    // or a punctuation character
    std::regex token_regex(R"(\s?\w+|[.,:;?!-_'"<>()[\]{}])");

    for (std::sregex_iterator iter(norm_str.begin(), norm_str.end(), token_regex); iter != std::sregex_iterator(); iter++) {
        std::smatch match = *iter;
        std::string token = match.str();
        if (token.at(0) == ' ') token.replace(0, 1, "Ķ");
        int start = match.position();
        int end = start + match.length();
        tokens.push_back(tokenizer::token(token, start, end));
    }

    return tokens;
}

std::list<std::string> tokenizer::string2vec(const std::string& str) const {
    std::list<std::string> vec_string;
    auto it = str.begin();

    while (it != str.end()) {
        unsigned char current_char = *it;
        int char_len = 0;

        // find the number of bytes for the current char (max 4 in utf-8)
        if ((current_char >> 7) == 0) {              // 1-byte char: 0xxxxxxx
            char_len = 1;
        } else if ((current_char >> 5) == 0b110) {   // 2-bytes char: 110xxxxx 10xxxxxx
            char_len = 2;
        } else if ((current_char >> 4) == 0b1110) {  // 3-bytes char: 1110xxxx 10xxxxxx 10xxxxxx
            char_len = 3;
        } else if ((current_char >> 3) == 0b11110) { // 4-bytes char: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            char_len = 4;
        }

        vec_string.emplace_back(it, it+char_len); // constructs the object in place
        std::advance(it, char_len);
    }

    return vec_string;
}

void tokenizer::parallel_splits_func(std::vector<std::list<std::string>>& splits, const std::function<void(size_t, size_t)>& func) {
    size_t n_cores = std::thread::hardware_concurrency();
    if (n_cores == 0) n_cores = 4;

    // there will be n_cores chunks (and threads) to maximize parallelism
    std::vector<std::thread> thread_pool;
    thread_pool.reserve(n_cores);
    size_t chunk_size = splits.size() / n_cores;

    if (chunk_size == 0) { // the corpus is too short, creating threads will be useless
        func(0, splits.size());
    } else {
        for (size_t chunk = 0; chunk < n_cores; chunk++) {
            size_t start = chunk * chunk_size;
            size_t end = (chunk == n_cores-1) ? splits.size() : start + chunk_size;
            thread_pool.emplace_back(func, start, end);
        }
        for (auto& th : thread_pool) th.join();
    }
}

void tokenizer::train_bpe(const std::vector<tokenizer::token>& tokens, size_t n_merges) {
    std::vector<std::list<std::string>> splits;
    std::unordered_map<byte_pair, size_t, byte_pair::hash> pairs_freqs;
    std::unordered_set<std::string> seen_tokens; // avoids duplicate tokens thus making the corpus smaller
    std::mutex freq_mutex;

    splits.reserve(tokens.size());
    seen_tokens.reserve(tokens.size()/4);
    this->merge_rules.reserve(n_merges);

    // initialize splits (e.g. "hi" -> "h", "i")
    for (const auto& token : tokens) {
        if (seen_tokens.count(token.value) == 0) {
            seen_tokens.insert(token.value);
            splits.push_back(string2vec(token.value));
        }
    }

    for (size_t i = 0; i < n_merges; i++) {
        pairs_freqs.reserve(tokens.size());
        std::cout << "--- Merge " << i+1 << "/" << n_merges << " ---" << std::endl;
        size_t hi_freq = 0;
        tokenizer::byte_pair new_rule;


        // lambda used for counting the frequencies of byte-pairs on each chunk
        const std::function<void(size_t, size_t)> count_bp_frequency = [&](size_t start, size_t end) {
            std::unordered_map<byte_pair, size_t, byte_pair::hash> chunk_freqs;
            for (size_t j = start; j < end; j++) {
                const auto& split = splits.at(j);
                for (auto it = split.begin(); std::next(it) != split.end(); it++) {
                    tokenizer::byte_pair current_pair(*it, *std::next(it));
                    chunk_freqs[current_pair]++;
                }
            }

            std::lock_guard<std::mutex> lock(freq_mutex); // releases lock when out of scope
            for (const auto& pair_freq : chunk_freqs)
                pairs_freqs[pair_freq.first] += pair_freq.second;
        };

        parallel_splits_func(splits, count_bp_frequency);

        // after computing each chunk's byte-pairs frequencies, we look for the highest one
        for (const auto& pair_freq : pairs_freqs) {
            if (pair_freq.second > hi_freq) {
                hi_freq = pair_freq.second;
                new_rule = pair_freq.first;
            }
        }

        // lambda used for applying the new merge rule on each chunk
        const std::function<void(size_t, size_t)> apply_merge_rule = [&](size_t start, size_t end) {
            for (size_t j = start; j < end; j++) {
                auto& split = splits.at(j);
                for (auto it = split.begin(); std::next(it) != split.end();) {
                    if (*it == new_rule.first and *std::next(it) == new_rule.second) {
                        *it = new_rule.first + new_rule.second;
                        it = split.erase(std::next(it));
                    } else {
                        it++;
                    }
                }
            }
        };

        parallel_splits_func(splits, apply_merge_rule);

        // the new merge rule is stored and the byte-pair frequencies are reset
        if (!new_rule.first.empty() and !new_rule.second.empty())
            this->merge_rules.push_back(new_rule);
        pairs_freqs.clear();
    }
}

template <class Archive>
void tokenizer::serialize(Archive& archive) {
    archive(normalize_opts, merge_rules);
}

void tokenizer::save(const std::string& filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) throw std::runtime_error("Unable to write file: " + filename);

    // tracks the endianness
    cereal::PortableBinaryOutputArchive oarchive(ofs);
    oarchive(*this);
}

void tokenizer::load(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) throw std::runtime_error("Unable to read file: " + filename);

    // tracks the endianness
    cereal::PortableBinaryInputArchive iarchive(ifs);
    iarchive(*this);
}
