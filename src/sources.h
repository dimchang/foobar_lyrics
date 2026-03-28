#pragma once
#include "lyric_source.h"
#include <string>
#include <vector>

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