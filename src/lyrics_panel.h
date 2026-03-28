#pragma once
#include "lrc_parser.h"
#include <atlbase.h>
#include <atlwin.h>
#include <gdiplus.h>
#include <memory>
#include <vector>
#include <string>

class LyricSearchTask;

#pragma comment(lib, "gdiplus.lib")

class LyricsPanel : public CWindowImpl<LyricsPanel> {
public:
    DECLARE_WND_CLASS(_T("LyricsPanel"))

    BEGIN_MSG_MAP(LyricsPanel)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_LYRIC_SEARCH_COMPLETE, OnSearchComplete)
    END_MSG_MAP()

    LyricsPanel();
    ~LyricsPanel();

    void SetLyrics(const std::vector<LyricLine>& lyrics);
    void UpdatePosition(double time_seconds);

public:
    std::vector<LyricLine> lyrics_;
    int current_index_;

private:
    LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnSearchComplete(UINT, WPARAM, LPARAM, BOOL&);

    void RenderLyrics(Gdiplus::Graphics& graphics);

private:
    int scroll_offset_ = 0;
    int font_height_ = 24;
    int line_spacing_ = 8;
    Gdiplus::Color text_color_ = Gdiplus::Color::White;
    Gdiplus::Color highlight_color_ = Gdiplus::Color::Yellow;
    Gdiplus::Color background_color_ = Gdiplus::Color::Black;
    ULONG_PTR gdiplus_token_ = 0;
};