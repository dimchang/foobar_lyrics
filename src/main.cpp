#include <foobar2000/SDK/foobar2000.h>
#include <foobar2000/SDK/mainmenu.h>
#include <foobar2000/SDK/play_callback_manager.h>
#include "lyrics_panel.h"
#include "lyric_search_task.h"
#include <atomic>

LyricsPanel* g_panel = nullptr;
HWND g_panel_hwnd = nullptr;
std::atomic<LyricSearchTask*> g_current_task{nullptr};

class playback_callback_impl : public play_callback_impl_base {
public:
    playback_callback_impl() : play_callback_impl_base(
        play_callback::flag_on_playback_new_track |
        play_callback::flag_on_playback_time |
        play_callback::flag_on_playback_stop |
        play_callback::flag_on_playback_pause
    ) {}

    void on_playback_new_track(metadb_handle_ptr p_track) override {
        if (!g_panel) return;
        metadb_v2_rec_t rec;
        if (p_track->get_v2(rec)) {
            const char* a = rec.meta_get("ARTIST", 0);
            const char* t = rec.meta_get("TITLE", 0);
            if (a && t) {
                // Start new search - old task auto-aborted if new one started
                auto* task = new LyricSearchTask(g_panel_hwnd, t, a);
                task->start(); // Handles atomic swap internally
            }
        }
    }

    void on_playback_time(double p) override {
        if (g_panel) g_panel->UpdatePosition(p);
    }

    void on_playback_stop(play_control::t_stop_reason) override {
        if (g_panel) { g_panel->current_index_ = -1; g_panel->Invalidate(); }
    }
    void on_playback_pause(bool) override {}
};

class my_initquit : public initquit {
public:
    service_ptr_t<playback_callback_impl> cb_;
    void on_init() override {
        console::print("foo_lyric_source: Initializing");
        g_panel = new LyricsPanel();
        g_panel->Create(nullptr, CRect(100,100,600,800), _T("Lyrics"), WS_OVERLAPPEDWINDOW|WS_VISIBLE);
        g_panel_hwnd = g_panel->m_hWnd;
        cb_ = new service_impl_t<playback_callback_impl>();
        play_callback_manager::get()->register_callback(cb_);
    }
    void on_quit() override {
        // Let the task clean itself up via atomic
        if (cb_.is_valid()) {
            play_callback_manager::get()->unregister_callback(cb_);
            cb_.release();
        }
        if (g_panel) { g_panel->DestroyWindow(); delete g_panel; g_panel = nullptr; }
        g_panel_hwnd = nullptr;
    }
};

class my_mainmenu : public mainmenu_commands {
public:
    t_uint32 get_command_count() override { return 1; }
    GUID get_command(t_uint32) override {
        static const GUID g = {0x12345678,0x1234,0x1234,{0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef}};
        return g;
    }
    void get_name(t_uint32, pfc::string_base& o) override { o = "Show Lyrics Panel"; }
    bool get_description(t_uint32, pfc::string_base& o) override { o = "Show/hide"; return true; }
    void execute(t_uint32, service_ptr_t<service_base>) override {
        if (g_panel && g_panel->IsWindow()) {
            g_panel->ShowWindow(g_panel->IsWindowVisible()?SW_HIDE:SW_SHOW);
        }
    }
};

static initquit_factory_t<my_initquit> g_initquit;
static mainmenu_commands_factory_t<my_mainmenu> g_mainmenu;