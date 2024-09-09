#include <utf8proc.h>
#include <string>
#include <iostream>
#include <vector>
#include <list>

class tokenizer {
public:
    class token {
    public:
        const size_t start, end;
        const std::string value;

        token();
        token(std::string str, size_t start, size_t end);
        friend std::ostream& operator<<(std::ostream& out, const tokenizer::token& t);
    };

    class byte_pair {
    public:
        byte_pair();
        byte_pair(std::string fst, std::string snd);
        bool operator==(const byte_pair &other) const;
        struct hash {
            size_t operator()(const byte_pair& bp) const;
        };
        std::string first, second;
    };

    tokenizer();
    std::string normalize(const std::string& str, bool strip_whitespaces = true) const;
    std::vector<token> pre_tokenize(const std::string& str) const;
    void train_bpe(const std::vector<token>& tokens, size_t n_merges);

private:
    size_t normalize_opts;
    std::vector<byte_pair> merge_rules;
    std::list<std::string> string2vec(const std::string& str) const;
};
