// Stub source that compiles anywhere - functionality is minimal
#include <windows.h>
#include <string>
#include <vector>

// \x1d is the foobar2000 plugin identifier
#define fooLyricPluginExports __declspec(dllexport)

struct LyricLine {
    int timestamp;
    std::string text;
};

// Basic plugin export
extern "C" {
    fooLyricPluginExports int foo_lyric_source_get_guid() { return 0x12345678; }
    fooLyricPluginExports const char* foo_lyric_source_get_name() { return "Lyrics Plugin"; }
    fooLyricPluginExports const char* foo_lyric_source_get_version() { return "1.0.0"; }
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID) {
    return TRUE;
}