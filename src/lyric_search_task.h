#pragma once
#include <string>
#include <vector>
#include "lyric_source.h"
#include <atomic>
#include <windows.h>

#define WM_LYRIC_SEARCH_COMPLETE (WM_USER + 100)

class LyricSearchTask {
public:
    LyricSearchTask(HWND hwnd, const std::string& song, const std::string& artist)
        : hwnd_(hwnd), song_name_(song), artist_(artist), is_running_(false), is_completed_(false) {}

    void start();
    bool is_running() const { return is_running_; }
    bool is_completed() const { return is_completed_; }
    const std::vector<LyricLine>& get_result() const { return result_; }

private:
    static DWORD WINAPI thread_func(LPVOID param);
    void do_search();

    HWND hwnd_;
    std::string song_name_, artist_;
    std::vector<LyricLine> result_;
    std::atomic_bool is_running_, is_completed_;
};