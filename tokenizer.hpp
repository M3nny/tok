#include <utf8proc.h>
#include <string>
#include <iostream>
#include <vector>

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

    tokenizer();
    std::string normalize(const std::string& str, bool strip_whitespaces = true) const;
    std::vector<token> pre_tokenize(const std::string& str) const;

private:
    size_t normalize_opts;
};
