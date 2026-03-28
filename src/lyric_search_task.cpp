#include "lyric_search_task.h"
#include "lyric_manager.h"
#include <atomic>

static std::atomic<LyricSearchTask*> g_current_task{nullptr};

void LyricSearchTask::start() {
    if (is_running_) return;
    
    LyricSearchTask* expected = nullptr;
    if (!g_current_task.compare_exchange_strong(expected, this)) {
        delete this;
        return;
    }
    
    is_running_ = true;
    is_completed_ = false;
    
    HANDLE ht = CreateThread(NULL, 0, thread_func, this, 0, NULL);
    if (ht) CloseHandle(ht);
}

DWORD WINAPI LyricSearchTask::thread_func(LPVOID p) {
    static_cast<LyricSearchTask*>(p)->do_search();
    return 0;
}

void LyricSearchTask::do_search() {
    result_ = LyricManager::get().search(song_name_, artist_);
    is_completed_ = true;
    is_running_ = false;
    
    if (hwnd_) ::PostMessage(hwnd_, WM_LYRIC_SEARCH_COMPLETE, reinterpret_cast<WPARAM>(this), 0);
    // Task deleted in OnSearchComplete only
}