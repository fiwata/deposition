#ifndef RENDER_H
#define RENDER_H

#include <windows.h>
#include <d2d1.h>
#include <opencv2/opencv.hpp>

#pragma comment(lib, "d2d1")

class FrameRenderer {
public:
    FrameRenderer(HWND hwnd)
        : m_hwnd(hwnd), m_pD2DFactory(NULL), m_pRenderTarget(NULL), m_pBitmap(NULL) {
        Initialize();
    }

    ~FrameRenderer() {
        Cleanup();
    }

    void Render(const cv::Mat& frame) {
        if (!m_pRenderTarget) return;

        // Calculate the stride
        RECT windowRect;
        GetClientRect(m_hwnd, &windowRect);
        int windowWidth = windowRect.right - windowRect.left;
        int windowHeight = windowRect.bottom - windowRect.top;

        cv::Mat resizedFrame;
        cv::resize(frame, resizedFrame, cv::Size(windowWidth, windowHeight));
        int stride = resizedFrame.cols * resizedFrame.channels();
        double frameAspectRatio = static_cast<double>(resizedFrame.cols) / resizedFrame.rows;

        // Calculate the target size of the combined frame while preserving its aspect ratio
        int targetWidth, targetHeight;
        if (windowWidth / frameAspectRatio <= windowHeight) {
            // Fit by width
            targetWidth = windowWidth;
            targetHeight = static_cast<int>(windowWidth / frameAspectRatio);
        }
        else {
            // Fit by height
            targetHeight = windowHeight;
            targetWidth = static_cast<int>(windowHeight * frameAspectRatio);
        }

        // Create bitmap properties
        D2D1_BITMAP_PROPERTIES bmpProperties;
        bmpProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bmpProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        bmpProperties.dpiX = 96.0f;
        bmpProperties.dpiY = 96.0f;

        // Create a bitmap
        HRESULT hr = m_pRenderTarget->CreateBitmap(
            D2D1::SizeU(targetWidth, targetHeight),
            resizedFrame.data,
            stride,
            &bmpProperties,
            &m_pBitmap
        );

        if (SUCCEEDED(hr)) {
            // Draw the bitmap within the render target
            m_pRenderTarget->BeginDraw();
            m_pRenderTarget->DrawBitmap(m_pBitmap, D2D1::RectF(0, 0, windowWidth, windowHeight));
            m_pRenderTarget->EndDraw();
            // Release bitmap
            m_pBitmap->Release();
            m_pBitmap = NULL;
        }
    }

private:
    void Initialize() {
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
        if (SUCCEEDED(hr)) {
            RECT windowRect;
            GetClientRect(m_hwnd, &windowRect);
            int windowWidth = windowRect.right - windowRect.left;
            int windowHeight = windowRect.bottom - windowRect.top;

            hr = m_pD2DFactory->CreateHwndRenderTarget(
                D2D1::RenderTargetProperties(),
                D2D1::HwndRenderTargetProperties(m_hwnd, D2D1::SizeU(windowWidth, windowHeight)),
                &m_pRenderTarget
            );
        }
    }

    void Cleanup() {
        if (m_pBitmap) {
            m_pBitmap->Release();
            m_pBitmap = NULL;
        }
        if (m_pRenderTarget) {
            m_pRenderTarget->Release();
            m_pRenderTarget = NULL;
        }
        if (m_pD2DFactory) {
            m_pD2DFactory->Release();
            m_pD2DFactory = NULL;
        }
    }

    HWND m_hwnd;
    ID2D1Factory* m_pD2DFactory;
    ID2D1HwndRenderTarget* m_pRenderTarget;
    ID2D1Bitmap* m_pBitmap;
};

#endif // !RENDER_H
