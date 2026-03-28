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

class LRCParser {
public:
    static std::vector<LyricLine> parse(const std::string& txt);
    static int getCurrentIndex(const std::vector<LyricLine>& v, double secs);
};

// 在线歌词源
class NeteaseSource : public LyricSource {
public:
    std::string getName() const override { return "Netease"; }
    std::vector<LyricLine> search(const std::string& song, const std::string& artist) override;
};

class QQMusicSource : public LyricSource {
public:
    std::string getName() const override { return "QQMusic"; }
    std::vector<LyricLine> search(const std::string& song, const std::string& artist) override;
};

class LocalSource : public LyricSource {
public:
    std::string getName() const override { return "Local"; }
    std::vector<LyricLine> search(const std::string& song, const std::string& artist) override;
};

// 歌词管理器
class LyricManager {
public:
    static LyricManager& get();
    std::vector<LyricLine> search(const std::string& s, const std::string& a);
};