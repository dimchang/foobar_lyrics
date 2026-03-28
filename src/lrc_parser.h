#pragma once
#include <string>
#include <vector>
#include "lyric_source.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <numeric>

class LRCParser {
public:
    static std::vector<LyricLine> parse(const std::string& txt) {
        std::vector<LyricLine> out;
        std::regex r(R"((\[(\d+):(\d+)(?:\.(\d+))?\]\s*)(.*)$)");
        std::smatch m;
        std::string line;
        std::istringstream ss(txt);
        while (std::getline(ss, line)) {
            if (std::regex_search(line, m, r) && m.size() >= 6) {
                int ms = std::stoi(m[2].str()) * 60000 + std::stoi(m[3].str()) * 1000;
                if (m[4].matched) {
                    std::string msStr = m[4].str();
                    if (msStr.size() == 2) ms += std::stoi(msStr) * 10;
                    else if (msStr.size() == 3) ms += std::stoi(msStr);
                }
                out.push_back({ms, m[5].str()});
            }
        }
        std::sort(out.begin(), out.end(), [](auto& a, auto& b){ return a.timestamp < b.timestamp; });
        return out;
    }

    static int getCurrentIndex(const std::vector<LyricLine>& v, double secs) {
        int ms = (int)(secs * 1000);
        for (int i = (int)v.size() - 1; i >= 0; --i)
            if (v[i].timestamp <= ms) return i;
        return -1;
    }
};