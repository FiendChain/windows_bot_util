#include "mss.h"

#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wingdi.h>

namespace util {

MSS::MSS() 
: m_channels(4) {
    m_src_hdc = GetWindowDC(0);
    m_dst_hdc = CreateCompatibleDC(m_src_hdc);
    m_max_width = 0;
    m_max_height = 0;

    m_waiting_write = false;
    SetSize(100, 100);
}

void MSS::SetSize(int width, int height) {
    // do not need to resize bitmap buffer
    if ((width <= m_max_width) && (height <= m_max_height)) {
        m_width = width;
        m_height = height;
        return;
    }

    // TODO: free bmp if it exists
    // we have a single infrequent writer
    auto waiting_write_lock = std::unique_lock(m_waiting_write_mutex);
    m_waiting_write = true;
    auto lock = std::unique_lock(m_buffer_mutex);

    // every time we resize, we double the buffer in both dimensions
    const int new_width  = std::max(width, m_max_width*2);
    const int new_height = std::max(height, m_max_height*2);

    BITMAPINFO bmi;
    auto &header = bmi.bmiHeader;
    header.biSize = sizeof(BITMAPINFOHEADER);
    header.biPlanes = 1;
    header.biBitCount = 32;
    header.biCompression = 0;
    header.biClrUsed = 0;
    header.biClrImportant = 0;
    header.biWidth = new_width;
    header.biHeight = new_height;

    void *bmp_bits;
    m_hbmp = CreateDIBSection(m_src_hdc, &bmi, DIB_RGB_COLORS, &bmp_bits, NULL, 0);
    SelectObject(m_dst_hdc, m_hbmp);
    m_max_width = new_width;
    m_max_height = new_height;
    m_width = width;
    m_height = height;

    m_waiting_write = false;
    waiting_write_lock.unlock();
    m_reader_cv.notify_all();
}

bool MSS::Grab(int top, int left) {
    {
        auto waiting_write_lock = std::unique_lock(m_waiting_write_mutex);
        m_reader_cv.wait(waiting_write_lock, [this]() {
            return !m_waiting_write;
        });
    }

    auto lock = std::shared_lock(m_buffer_mutex);

    bool status = BitBlt(
        m_dst_hdc, 
        0, 0,
        m_width, m_height,
        m_src_hdc,
        left, top,
        SRCCOPY | CAPTUREBLT);
    return status;
}

MSSBitmap MSS::GetBitmap() {
    {
        auto waiting_write_lock = std::unique_lock(m_waiting_write_mutex);
        m_reader_cv.wait(waiting_write_lock, [this]() {
            return !m_waiting_write;
        });
    }

    auto bitmap = MSSBitmap(m_buffer_mutex, m_channels);
    DIBSECTION sec;
    int status = GetObject(m_hbmp, sizeof(sec), (LPVOID)(&sec));
    bitmap.SetBitmap(sec);
    bitmap.SetSize(m_width, m_height);
    return bitmap;
}

}