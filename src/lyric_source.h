#pragma once
#include <string>
#include <vector>

struct LyricLine {
    int timestamp;
    std::string text;
};

class LyricSource {
public:
    virtual ~LyricSource() = default;
    virtual std::string getName() const = 0;
    virtual std::vector<LyricLine> search(const std::string& song, const std::string& artist) = 0;
};