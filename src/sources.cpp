#include "sources.h"
#include "lrc_parser.h"
#include <winhttp.h>
#include <regex>
#include <pfc.h>

#pragma comment(lib, "winhttp.lib")

static std::string httpGet(const std::string& url) {
    HINTERNET hs = WinHttpOpen(L"lyric/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hs) return "";
    
    URL_COMPONENTS uc = {0}; uc.dwStructSize = sizeof(uc);
    wchar_t scheme[256], host[256], path[1024];
    uc.lpszScheme = scheme; uc.dwSchemeLength = 256;
    uc.lpszHostName = host; uc.dwHostNameLength = 256;
    uc.lpszUrlPath = path; uc.dwUrlPathLength = 1024;
    pfc::stringcvt::string_wide_from_utf8 wurl(url.c_str());
    if (!WinHttpCrackUrl(wurl.get_ptr(), 0, 0, &uc)) { WinHttpCloseHandle(hs); return ""; }
    
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
    pfc::stringcvt::string_wide_from_utf8 wurl(url.c_str());
    if (!WinHttpCrackUrl(wurl.get_ptr(), 0, 0, &uc)) { WinHttpCloseHandle(hs); return ""; }
    
    HINTERNET hc = WinHttpConnect(hs, host, uc.nPort, 0);
    if (!hc) { WinHttpCloseHandle(hs); return ""; }
    HINTERNET hr = WinHttpOpenRequest(hc, L"POST", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hr) { WinHttpCloseHandle(hc); WinHttpCloseHandle(hs); return ""; }
    
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
    std::string data = "s=" + q + "&limit=10&type=1";
    std::string r = httpPost("http://music.163.com/api/search/get/", data);
    if (r.empty()) return {};
    
    std::regex id(R"("id":(\d+))"); std::smatch m;
    if (!std::regex_search(r, m, id)) return {};
    std::string sid = m[1].str();
    
    std::string lr = httpGet("http://music.163.com/api/song/lyric?lv=-1&tv=-1&id=" + sid);
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
    std::string body = R"({"comm":{"ct":"19","cv":"1859","uin":"0"},"req":{"method":"DoSearchForQQMusicDesktop","module":"music.search.SearchCgiService","param":{"grp":1,"num_per_page":10,"page_num":1,"query":")" + q + R"(","search_type":0}}})";
    std::string r = httpPost("https://u.y.qq.com/cgi-bin/musicu.fcg", body);
    if (r.empty()) return {};
    
    std::regex mid(R"("songmid":"([^"]+))"); std::smatch m;
    if (!std::regex_search(r, m, mid)) return {};
    std::string smid = m[1].str();
    
    std::string lr = httpGet("https://i.y.qq.com/lyric/fcgi-bin/fcg_query_lyric_new.fcg?songmid=" + smid + "&g_tk=5381&format=json&nobase64=1");
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
    // Simplified: Looks in ./lyrics folder - not fully implemented for brevity
    return {};
}