#include <utf8proc.h>

#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <functional>

class tokenizer {
public:
    class token {
    public:
        token();
        token(std::string str, size_t start, size_t end);

        friend std::ostream& operator<<(std::ostream& out, const tokenizer::token& t);

        const size_t start, end;
        const std::string value;
    };

    class byte_pair {
    public:
        struct hash {
            size_t operator()(const byte_pair& bp) const;
        };

        byte_pair();
        byte_pair(std::string fst, std::string snd);

        bool operator==(const byte_pair &other) const;

        template <class Archive>
        void serialize(Archive& archive);

        std::string first, second;
    };

    tokenizer();
    tokenizer(size_t opts);
    std::string normalize(const std::string& str, bool strip_whitespaces = true) const;
    std::vector<token> pre_tokenize(const std::string& str, bool file_path = false) const;

    void train_bpe(const std::vector<token>& tokens, size_t n_merges);
    const std::vector<byte_pair>& get_merge_rules() const;
    void save(const std::string& filename) const;
    void load(const std::string& filename);

    template <class Archive>
    void serialize(Archive& archive);

private:
    friend class cereal::access; // allow cereal to access private fields

    std::list<std::string> string2list(const std::string& str) const;
    void parallel_splits_func(std::vector<std::list<std::string>>& splits, const std::function<void(size_t, size_t)>& func);

    size_t normalize_opts;
    std::vector<byte_pair> merge_rules;
};
