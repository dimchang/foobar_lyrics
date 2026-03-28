#include "lyric_source.h"
#include <windows.h>
#include <regex>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "winhttp.lib")

static std::string httpGet(const std::string& url) {
    HINTERNET hs = WinHttpOpen(L"lyric/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hs) return "";
    
    URL_COMPONENTS uc = {0}; uc.dwStructSize = sizeof(uc);
    wchar_t scheme[256], host[256], path[1024];
    uc.lpszScheme = scheme; uc.dwSchemeLength = 256;
    uc.lpszHostName = host; uc.dwHostNameLength = 256;
    uc.lpszUrlPath = path; uc.dwUrlPathLength = 1024;
    
    int len = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, NULL, 0);
    std::wstring wurl(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, &wurl[0], len);
    
    if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &uc)) { WinHttpCloseHandle(hs); return ""; }
    HINTERNET hc = WinHttpConnect(hs, host, uc.nPort, 0);
    if (!hc) { WinHttpCloseHandle(hs); return ""; }
    HINTERNET hr = WinHttpOpenRequest(hc, L"GET", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hr) { WinHttpCloseHandle(hc); WinHttpCloseHandle(hs); return ""; }
    WinHttpSendRequest(hr, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    WinHttpReceiveResponse(hr, NULL);
    std::string r; DWORD br = 0; char buf[4096];
    while (WinHttpReadData(hr, buf, sizeof(buf), &br) && br) r.append(buf, br);
    WinHttpCloseHandle(hr); WinHttpCloseHandle(hc); WinHttpCloseHandle(hs);
    return r;
}

static std::string httpPost(const std::string& url, const std::string& data) {
    HINTERNET hs = WinHttpOpen(L"lyric/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hs) return "";
    URL_COMPONENTS uc = {0}; uc.dwStructSize = sizeof(uc);
    wchar_t scheme[256], host[256], path[1024];
    uc.lpszScheme = scheme; uc.dwSchemeLength = 256;
    uc.lpszHostName = host; uc.dwHostNameLength = 256;
    uc.lpszUrlPath = path; uc.dwUrlPathLength = 1024;
    int len = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, NULL, 0);
    std::wstring wurl(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, &wurl[0], len);
    if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &uc)) { WinHttpCloseHandle(hs); return ""; }
    HINTERNET hc = WinHttpConnect(hs, host, uc.nPort, 0);
    HINTERNET hr = WinHttpOpenRequest(hc, L"POST", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    WinHttpAddRequestHeaders(hr, L"Content-Type: application/x-www-form-urlencoded", -1, WINHTTP_ADDREQ_FLAG_ADD);
    WinHttpSendRequest(hr, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID*)data.c_str(), data.length(), data.length(), 0);
    WinHttpReceiveResponse(hr, NULL);
    std::string r; DWORD br = 0; char buf[4096];
    while (WinHttpReadData(hr, buf, sizeof(buf), &br) && br) r.append(buf, br);
    WinHttpCloseHandle(hr); WinHttpCloseHandle(hc); WinHttpCloseHandle(hs);
    return r;
}

std::vector<LyricLine> NeteaseSource::search(const std::string& song, const std::string& artist) {
    std::string q = song + (artist.empty() ? "" : " " + artist);
    std::string r = httpPost("http://music.163.com/api/search/get/", "s=" + q + "&type=1&limit=5");
    std::regex id(R"("id":(\d+))"); std::smatch m;
    if (!std::regex_search(r, m, id)) return {};
    std::string lr = httpGet("http://music.163.com/api/song/lyric?lv=-1&id=" + m[1].str());
    std::regex ly(R"("lyric":"((?:[^"\\]|\\.)*)")");
    if (std::regex_search(lr, m, ly)) {
        std::string txt = m[1].str();
        size_t p = 0;
        while ((p = txt.find("\\n", p)) != std::string::npos) txt.replace(p, 2, "\n"), ++p;
        return LRCParser::parse(txt);
    }
    return {};
}

std::vector<LyricLine> QQMusicSource::search(const std::string& song, const std::string& artist) {
    std::string q = song + (artist.empty() ? "" : " " + artist);
    std::string r = httpPost("https://u.y.qq.com/cgi-bin/musicu.fcg", 
        R"({"comm":{"ct":"19","cv":"1859"},"req":{"method":"DoSearchForQQMusicDesktop","module":"music.search.SearchCgiService","param":{"query":")" + q + R"(","search_type":0}}})");
    std::regex mid(R"("songmid":"([^"]+))"); std::smatch m;
    if (!std::regex_search(r, m, mid)) return {};
    std::string lr = httpGet("https://i.y.qq.com/lyric/fcgi-bin/fcg_query_lyric_new.fcg?songmid=" + m[1].str() + "&g_tk=5381");
    std::regex ly(R"("lyric":"((?:[^"\\]|\\.)*)")");
    if (std::regex_search(lr, m, ly)) {
        std::string txt = m[1].str();
        size_t p = 0;
        while ((p = txt.find("\\n", p)) != std::string::npos) txt.replace(p, 2, "\n"), ++p;
        return LRCParser::parse(txt);
    }
    return {};
}

std::vector<LyricLine> LocalSource::search(const std::string& song, const std::string& artist) {
    return {}; // TODO: 实现本地歌词文件搜索
}

std::vector<LyricLine> LRCParser::parse(const std::string& txt) {
    std::vector<LyricLine> out;
    std::regex r(R"((\[(\d+):(\d+)(?:\.(\d+))?\]\s*)(.*)$)");
    std::smatch m;
    std::string line;
    std::istringstream ss(txt);
    while (std::getline(ss, line)) {
        if (std::regex_search(line, m, r) && m.size() >= 6) {
            int ms = std::stoi(m[2].str()) * 60000 + std::stoi(m[3].str()) * 1000;
            if (m[4].matched) ms += (m[4].str().size() == 2 ? std::stoi(m[4].str()) * 10 : std::stoi(m[4].str()));
            out.push_back({ms, m[5].str()});
        }
    }
    std::sort(out.begin(), out.end(), [](auto& a, auto& b) { return a.timestamp < b.timestamp; });
    return out;
}

int LRCParser::getCurrentIndex(const std::vector<LyricLine>& v, double secs) {
    int ms = (int)(secs * 1000);
    for (int i = (int)v.size() - 1; i >= 0; --i)
        if (v[i].timestamp <= ms) return i;
    return -1;
}

LyricManager& LyricManager::get() {
    static LyricManager m;
    return m;
}

std::vector<LyricLine> LyricManager::search(const std::string& s, const std::string& a) {
    LocalSource local;
    auto r = local.search(s, a);
    if (!r.empty()) return r;
    NeteaseSource netease;
    r = netease.search(s, a);
    if (!r.empty()) return r;
    QQMusicSource qq;
    return qq.search(s, a);
}