#pragma once
#include "lyric_source.h"
#include "lrc_parser.h"
#include <vector>
#include <string>

class LyricManager {
public:
    static LyricManager& get() { static LyricManager m; return m; }
    void addSource(LyricSource* s) { srcs_.push_back(s); }
    std::vector<LyricLine> search(const std::string& s, const std::string& a) {
        for (auto* x : srcs_) {
            auto r = x->search(s, a);
            if (!r.empty()) return r;
        }
        return {};
    }
private:
    LyricManager() {
        srcs_.push_back(new LocalSource());
        srcs_.push_back(new NeteaseSource());
        srcs_.push_back(new QQMusicSource());
    }
    std::vector<LyricSource*> srcs_;
};