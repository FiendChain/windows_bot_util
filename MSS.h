#pragma once

#include <stdint.h>
#include <shared_mutex>
#include <condition_variable>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <wingdi.h>

namespace util {

class MSS;

class MSSBitmap 
{
public:
    struct Vec2D {
        int x, y;
    };
private:
    DIBSECTION m_bitmap;
    std::shared_lock<std::shared_mutex> m_lock;
    Vec2D m_size;
    const int m_channels;
public:
    MSSBitmap(std::shared_mutex &mutex, const int channels)
    : m_lock(mutex), m_channels(channels) {}
    inline DIBSECTION &GetBitmap() { return m_bitmap; }
    inline Vec2D GetSize() const { return m_size; }
private:
    void SetBitmap(DIBSECTION &section) { m_bitmap = section; }
    void SetSize(const int x, const int y) { 
        m_size.x = x; 
        m_size.y = y; 
    }
    friend class MSS;
};

class MSS
{
public:
    struct Vec2D {
        int x;
        int y;
    };
private:
    HDC m_src_hdc;
    HDC m_dst_hdc;

    HBITMAP m_hbmp;

    int m_max_width, m_max_height;
    int m_width, m_height; 
    const int m_channels;

    std::shared_mutex m_buffer_mutex;
    std::mutex m_waiting_write_mutex;
    bool m_waiting_write;
    std::condition_variable m_reader_cv;
public:
    MSS();
    void SetSize(int width, int height);
    inline Vec2D GetSize() const { return {m_width, m_height}; }
    inline Vec2D GetMaxSize() const { return {m_max_width, m_max_height}; }
    bool Grab(int top, int left);
    MSSBitmap GetBitmap();
};

}