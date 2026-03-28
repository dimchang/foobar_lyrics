#include "lyrics_panel.h"
#include "lyric_manager.h"
#include <gdiplus.h>
#include <algorithm>
#include <atomic>

extern std::atomic<LyricSearchTask*> g_current_task;

LyricsPanel::LyricsPanel()
    : text_color_(Gdiplus::Color::White)
    , highlight_color_(Gdiplus::Color::Yellow)
    , background_color_(Gdiplus::Color::Black)
{
    Gdiplus::GdiplusStartupInput in;
    Gdiplus::GdiplusStartup(&gdiplus_token_, &in, nullptr);
}

LyricsPanel::~LyricsPanel() {
    if (gdiplus_token_) Gdiplus::GdiplusShutdown(gdiplus_token_);
}

void LyricsPanel::SetLyrics(const std::vector<LyricLine>& lyrics) {
    lyrics_ = lyrics;
    current_index_ = -1;
    scroll_offset_ = 0;
    Invalidate();
}

void LyricsPanel::UpdatePosition(double secs) {
    int idx = LRCParser::getCurrentIndex(lyrics_, secs);
    if (idx != current_index_) {
        current_index_ = idx;
        Invalidate();
    }
}

LRESULT LyricsPanel::OnCreate(UINT, WPARAM, LPARAM, BOOL& b) { b = FALSE; return 0; }
LRESULT LyricsPanel::OnDestroy(UINT, WPARAM, LPARAM, BOOL&) { return 0; }

LRESULT LyricsPanel::OnPaint(UINT, WPARAM, LPARAM, BOOL&) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(&ps);
    if (hdc) { Gdiplus::Graphics g(hdc); g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias); RenderLyrics(g); }
    EndPaint(&ps);
    return 0;
}

LRESULT LyricsPanel::OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL& b) { b = TRUE; return 1; }
LRESULT LyricsPanel::OnSize(UINT, WPARAM, LPARAM, BOOL&) { Invalidate(); return 0; }

LRESULT LyricsPanel::OnSearchComplete(UINT, WPARAM wp, LPARAM, BOOL&) {
    auto* t = reinterpret_cast<LyricSearchTask*>(wp);
    if (t && t->is_completed()) {
        SetLyrics(t->get_result());
        // Clear atomic and delete
        LyricSearchTask* expected = t;
        g_current_task.compare_exchange_strong(expected, nullptr);
        delete t;
    }
    return 0;
}

void LyricsPanel::RenderLyrics(Gdiplus::Graphics& g) {
    CRect r; GetClientRect(&r);
    g.FillRectangle(&Gdiplus::SolidBrush(background_color_), 0, 0, r.Width(), r.Height());
    if (lyrics_.empty()) {
        Gdiplus::Font f(L"Arial", 16);
        Gdiplus::SolidBrush b(text_color_);
        Gdiplus::StringFormat fmt;
        fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
        g.DrawString(L"No lyrics", -1, &f, Gdiplus::RectF(0,0,r.Width(),r.Height()), &fmt, &b);
        return;
    }
    Gdiplus::Font font(L"Arial", (Gdiplus::REAL)font_height_);
    Gdiplus::StringFormat fmt; fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    int y = 100 - scroll_offset_;
    for (size_t i = 0; i < lyrics_.size(); ++i) {
        bool cur = (int)i == current_index_;
        Gdiplus::SolidBrush br(cur?highlight_color_:text_color_);
        Gdiplus::REAL sz = cur?(Gdiplus::REAL)(font_height_*1.2):(Gdiplus::REAL)font_height_;
        Gdiplus::Font lf(L"Arial", sz, cur?Gdiplus::FontStyleBold:Gdiplus::FontStyleRegular);
        pfc::stringcvt::string_wide_from_utf8 wt(lyrics_[i].text.c_str());
        g.DrawString(wt, -1, &lf, Gdiplus::PointF(r.Width()/2, (Gdiplus::REAL)y), &fmt, &br);
        y += font_height_ + line_spacing_;
    }
}