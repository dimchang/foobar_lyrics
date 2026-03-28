#pragma once
#include "lyric_source.h"
#include "lrc_parser.h"
#include "sources.h"
#include <vector>
#include <string>

class LyricManager {
public:
    static LyricManager& get() { 
        static LyricManager m; 
        return m; 
    }
    
    std::vector<LyricLine> search(const std::string& s, const std::string& a) {
        // Local source first
        LocalSource local;
        auto r = local.search(s, a);
        if (!r.empty()) return r;
        
        // Then online sources
        NeteaseSource netease;
        r = netease.search(s, a);
        if (!r.empty()) return r;
        
        QQMusicSource qq;
        r = qq.search(s, a);
        return r;
    }
};