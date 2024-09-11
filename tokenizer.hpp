#include <utf8proc.h>

#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <vector>
#include <list>
#include <unordered_map>
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
    tokenizer(std::string spec_char, size_t opts);
    std::string normalize(const std::string& str, bool strip_whitespaces = true) const;
    std::vector<token> pre_tokenize(const std::string& str, bool is_file_path = false) const;

    void train_bpe(const std::string& corpus, size_t n_merges, bool is_file_path = false);
    const std::unordered_map<std::string, size_t>& get_vocab() const;
    const std::vector<byte_pair>& get_merges() const;
    std::vector<std::string> tokenize(const std::string& str) const;
    void save(const std::string& filename) const;
    void load(const std::string& filename);

    template <class Archive>
    void serialize(Archive& archive);

private:
    friend class cereal::access; // allow cereal to access private fields

    std::list<std::string> string2list(const std::string& str) const;
    void parallel_splits_func(std::vector<std::list<std::string>>& splits, const std::function<void(size_t, size_t)>& func) const;
    size_t encode(const std::string & str) const;
    std::string decode(size_t id) const;

    std::string special_char;
    size_t normalize_opts;
    std::unordered_map<std::string, size_t> vocab;
    std::vector<byte_pair> merges;
};
