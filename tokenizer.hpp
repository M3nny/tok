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
    /*
    * This class is used to store and lookup the result of tokenizer::pre_tokenize
    * a word represent a string isolted by space and/or punctuation marks by printing a word
    * it will be shown its value followed by its starting and ending indexes in the normalized corpus
    */
    class word {
    public:
        word();
        word(std::string str, size_t start, size_t end);

        friend std::ostream& operator<<(std::ostream& out, const tokenizer::word& t);

        const std::string value;
        const size_t start, end;
    };

    /*
    * A byte_pair is just a pair of strings with an arbitrary length
    * it has an hash function and an equality operator
    * so it can be used inside data structures which requires these functions (e.g. std::unordered_map)
    */
    class byte_pair {
    public:
        struct hash {
            size_t operator()(const byte_pair& bp) const;
        };

        byte_pair();
        byte_pair(std::string fst, std::string snd);

        bool operator==(const byte_pair &other) const;

        std::string first, second;
    private:
        friend class cereal::access; // Allow cereal to access private fields
        template <class Archive>
        void serialize(Archive& archive);
    };

    tokenizer();
    tokenizer(std::string lead_ws, std::string eot, size_t opts);

    // Normalizes a string by lower-casing it, removing accents, and stripping excessive white spaces
    std::string normalize(const std::string& str, bool strip_whitespaces = true) const;

    /*
    * Normalizes the corpus given in input, it can be a string or a path to a file,
    * then it matches every alphanumeric word/punctuation mark and stores them into a vector
    * if the word/punctuation was preceded by a white space, it will be replaced by leading_white_space
    */
    std::vector<word> pre_tokenize(const std::string& str, bool is_file_path = false) const;

    /*
    * Pre tokenizes the corpus, it can be a string or a path to a file,
    * then for n_merges times it uses up to <number of your cpu cores> threads to:
    * 1. Count the byte_pair with highest frequency
    * 2. Merge every occurrence of that byte_pair
    * 3. Push into vocab the new merge rule and merged byte_pair
    */
    void train_bpe(const std::string& corpus, size_t n_merges, bool is_file_path = false);

    /*
    * Uses up to <number of your cpu cores> threads to
    * apply every merge rule stored in merges to the input string
    */
    std::vector<std::string> tokenize(const std::string& str) const;


    /*
    * For every string in input (tokens in this case) it will return the matching encoding (id)
    * corresponding to the specific token
    */
    std::vector<size_t> tokens_to_ids(const std::vector<std::string>& tokens) const;

    /*
    * For every id in input it will decode it and then return them,
    * in this case it won't look them up in a data structure, but instead calculate them in place
    * vocab should have been a bi-directional map, or there should have been another inverse map (thus taking up more space)
    */
    std::vector<std::string> ids_to_tokens(const std::vector<size_t>& ids) const;

    // Returns the vocab with basic chars, special chars and applied merges
    const std::unordered_map<std::string, size_t>& get_vocab() const;

    // Returns the merges learned during training
    const std::vector<byte_pair>& get_merges() const;

    /*
    * Uses cereal to save special chars, vocab and merges
    * into a portable binary format that keeps track of the endianness of the system
    */
    void save(const std::string& filename) const;

    // Uses cereal to load special chars, vocab and merges from a portable binary format
    void load(const std::string& filename);

private:
    friend class cereal::access; // Allow cereal to access private fields
    template <class Archive>
    void serialize(Archive& archive);

    /*
    * Converts a string into a list of utf-8 characters,
    * each character can occupy up to 4 bytes, so a string will be used to represent them
    */
    std::list<std::string> string2list(const std::string& str) const;

    // Computes a function along a vector containing lists of strings using up to <number of your cpu cores> threads.
    void parallel_splits_func(std::vector<std::list<std::string>>& splits, const std::function<void(size_t, size_t)>& func) const;

    // Creates an integer by attaching each byte of the string
    size_t encode(const std::string & str) const;

    // Creates a string by detaching every byte starting from the last one, casting it into a char and then preprending it to the string
    std::string decode(size_t id) const;

    // Fills vocab with ascii characters ranging from 32 to 126 and adds the special chars
    void fill_basic_vocab();

    /*
    * These are the special chars:
    * - leading_white_space by default will be 'Ķ'
    * - endoftext by default will be "<|eot|>"
    */
    std::string leading_white_space, endoftext;

    /*
    * Stores the normalization options (check out https://juliastrings.github.io/utf8proc/doc/utf8proc_8h.html#a0a18a541ba5bedeb5c3e150024063c2d)
    */
    size_t normalize_opts;

    // Maps the token to its encoded representation
    std::unordered_map<std::string, size_t> vocab;

    // Stores the merge rules learned during training
    std::vector<byte_pair> merges;
};
