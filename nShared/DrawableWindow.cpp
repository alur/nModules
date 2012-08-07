/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	DrawableWindow.cpp												July, 2012
 *	The nModules Project
 *
 *	Implementation of the DrawableWindow class. A generic drawable window.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "DrawableWindow.hpp"
#include "PaintSettings.hpp"
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <Wincodec.h>
#include "Factories.h"

using namespace D2D1;

// Safe way to release objects which required ->Release()
#define SAFERELEASE(x) if (*x != NULL) { (*x)->Release(); *x = NULL; }

/// <summary>
/// Constructor
/// </summary>
DrawableWindow::DrawableWindow(HWND parent, LPCSTR pszWndClass, PaintSettings * paintSettings, HINSTANCE hInst) {
	m_hWndParent = parent;
	m_pszText = "Test";
	m_pPaintSettings = paintSettings;
	CreateWnd(pszWndClass, hInst);
}

/// <summary>
/// Destructor
/// </summary>
DrawableWindow::~DrawableWindow() {
	KillTimer(m_hWnd, 1337);

	if (m_hWnd)
		DestroyWindow(m_hWnd);
	
	SAFERELEASE(&m_pRenderTarget)
	SAFERELEASE(&m_pBackBrush)
	SAFERELEASE(&m_pTextBrush)
	SAFERELEASE(&m_pTextFormat)
	PurgeOverlays();
}

/// <summary>
/// Gets the HWND of this class.
/// </summary>
HWND DrawableWindow::getWindow() {
	return m_hWnd;
}

/// <summary>
/// Reloads graphics resources from the paintsettings object.
/// </summary>
void DrawableWindow::UpdateBrushes() {
	// Update the background brush
	ID2D1SolidColorBrush *backBrush = reinterpret_cast<ID2D1SolidColorBrush*>(m_pBackBrush);
	backBrush->SetColor(m_pPaintSettings->backColor);

	Repaint();
}

/// <summary>
/// Repaints the entire window.
/// </summary>
void DrawableWindow::Repaint() {
	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
}

/// <summary>
/// Adds an overlay to this window.
/// </summary>
HRESULT DrawableWindow::AddOverlay(D2D1_RECT_F f, HICON hIcon) {
	IWICBitmap *pSource = NULL;
	IWICBitmapScaler *pScaler = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICImagingFactory *pFactory = NULL;
	ID2D1Bitmap *pBitmap = NULL;
	Overlay overlay;

	// Create our helper objects.
	//nCore::Drawing::GetWICFactory(reinterpret_cast<LPVOID*>(&pFactory));
	Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&pFactory));
	pFactory->CreateBitmapScaler(&pScaler);
	pFactory->CreateFormatConverter(&pConverter);

	// Generate a WIC bitmap
	pFactory->CreateBitmapFromHICON(hIcon, &pSource);

	// Resize the image
	UINT width, height;
	pSource->GetSize(&width, &height);
	pScaler->Initialize(pSource, (UINT)(f.right - f.left), (UINT)(f.bottom - f.top), WICBitmapInterpolationModeCubic);
	
	// Convert it to an ID2D1Bitmap
	pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	m_pRenderTarget->CreateBitmapFromWicBitmap(pConverter, 0, &pBitmap);

    // Create a brush based on the bitmap
	m_pRenderTarget->CreateBitmapBrush(pBitmap, reinterpret_cast<ID2D1BitmapBrush**>(&overlay.brush));
	
	// Move the origin of the brush to match the overlay position
	overlay.brush->SetTransform(D2D1::Matrix3x2F::Translation(f.left, f.top));
	overlay.position = f;

	// Release stuff
	SAFERELEASE(&pScaler)
	SAFERELEASE(&pConverter)
	SAFERELEASE(&pSource)
	SAFERELEASE(&pBitmap)

	// Add the overlays to the overlay list
	m_overlays.push_back(overlay);

	return S_OK;
}

/// <summary>
/// Removes all overlays from the window.
/// </summary>
void DrawableWindow::PurgeOverlays() {
	for (vector<Overlay>::iterator iter = m_overlays.begin(); iter != m_overlays.end(); iter++) {
		SAFERELEASE(&iter->brush);
	}
	m_overlays.clear();
}

/// <summary>
/// Enables/Disables bluring of the window background.
/// </summary>
HRESULT DrawableWindow::SetBlur(bool bEnable) {
	// Specify blur-behind and blur region.
	DWM_BLURBEHIND bb = {0};
	bb.dwFlags = bEnable ? DWM_BB_ENABLE : NULL;
	bb.hRgnBlur = NULL;
	bb.fEnable = bEnable;

	// Enable blur-behind.
	return DwmEnableBlurBehindWindow(m_hWnd, &bb);
}

/// <summary>
/// Creates the window we are drawing.
/// </summary>
bool DrawableWindow::CreateWnd(LPCSTR pszWndClass, HINSTANCE hInst) {
	DWORD style = WS_CLIPCHILDREN;
	style |= m_hWndParent ? WS_CHILD : WS_POPUP;

	DWORD exStyle = WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW;
	if (!m_pPaintSettings->DWMBlur && !m_hWndParent) exStyle |= WS_EX_COMPOSITED;

	m_hWnd = CreateWindowEx(exStyle, pszWndClass, "", style,
		m_pPaintSettings->position.left, m_pPaintSettings->position.top, m_pPaintSettings->position.right - m_pPaintSettings->position.left,
		m_pPaintSettings->position.bottom - m_pPaintSettings->position.top, m_hWndParent, NULL, hInst, NULL);
	
	if (!m_hWnd) {
		return false;
	}

	D2D1_SIZE_U size = D2D1::SizeU(
		m_pPaintSettings->position.right - m_pPaintSettings->position.left,
		m_pPaintSettings->position.bottom - m_pPaintSettings->position.top
	);

	m_backArea.top = 0;
	m_backArea.left = 0;
	m_backArea.right = (float)size.width;
	m_backArea.bottom = (float)size.height;
	m_textArea = m_backArea;
	m_textArea.bottom -= m_pPaintSettings->textOffset.bottom;
	m_textArea.top += m_pPaintSettings->textOffset.top;
	m_textArea.left += m_pPaintSettings->textOffset.left;
	m_textArea.right -= m_pPaintSettings->textOffset.right;

	// Create Direct2D objects
	ID2D1Factory *pD2DFactory = NULL;
	//nCore::Drawing::GetD2DFactory(reinterpret_cast<LPVOID*>(&pD2DFactory));
	Factories::GetD2DFactory(reinterpret_cast<LPVOID*>(&pD2DFactory));
	pD2DFactory->CreateHwndRenderTarget(
		RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		),
        HwndRenderTargetProperties(m_hWnd, size),
        &m_pRenderTarget
	);

	m_pRenderTarget->CreateSolidColorBrush(m_pPaintSettings->backColor, (ID2D1SolidColorBrush**)&m_pBackBrush);
	m_pRenderTarget->CreateSolidColorBrush(m_pPaintSettings->fontColor, (ID2D1SolidColorBrush**)&m_pTextBrush);

	// Create DirectWrite objects
	IDWriteFactory *pDWFactory = NULL;
	//nCore::Drawing::GetDWriteFactory(reinterpret_cast<LPVOID*>(&pDWFactory));
	Factories::GetDWriteFactory(reinterpret_cast<LPVOID*>(&pDWFactory));
	pDWFactory->CreateTextFormat(
		m_pPaintSettings->font,
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		m_pPaintSettings->fontSize,
		L"en-US",
		&m_pTextFormat);

	m_pTextFormat->SetTextAlignment(m_pPaintSettings->textAlignment);
	m_pTextFormat->SetParagraphAlignment(m_pPaintSettings->textVerticalAlignment);

	SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	SetBlur(true);

	SetTimer(m_hWnd, 1337, 1000, NULL);

	

	return true;
}

/// <summary>
/// Shows the window
/// </summary>
void DrawableWindow::Show() {
	ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
}

void DrawableWindow::UpdatePosition() {
	D2D1_SIZE_U size = D2D1::SizeU(
		m_pPaintSettings->position.right - m_pPaintSettings->position.left,
		m_pPaintSettings->position.bottom - m_pPaintSettings->position.top
	);

	m_backArea.top = 0;
	m_backArea.left = 0;
	m_backArea.right = (float)size.width;
	m_backArea.bottom = (float)size.height;

	m_textArea = m_backArea;
	m_textArea.bottom -= m_pPaintSettings->textOffset.bottom;
	m_textArea.top += m_pPaintSettings->textOffset.top;
	m_textArea.left += m_pPaintSettings->textOffset.left;
	m_textArea.right -= m_pPaintSettings->textOffset.right;

	SetWindowPos(m_hWnd, 0, m_pPaintSettings->position.left, m_pPaintSettings->position.top, size.width, size.height, SWP_NOZORDER | SWP_FRAMECHANGED);
	m_pRenderTarget->Resize(size);

	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
}

LRESULT WINAPI DrawableWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_ERASEBKGND:
		return 1;

	case WM_PAINT:
		WCHAR wszText[MAX_LINE_LENGTH];
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->Clear();
		m_pRenderTarget->FillRectangle(m_backArea, m_pBackBrush);
		for (vector<Overlay>::iterator iter = m_overlays.begin(); iter != m_overlays.end(); iter++) {
			m_pRenderTarget->FillRectangle(iter->position, iter->brush);
		}
		nCore::System::FormatText(m_pPaintSettings->text, sizeof(wszText)/sizeof(WCHAR), wszText);
		m_pRenderTarget->DrawText(wszText, lstrlenW(wszText), m_pTextFormat, m_textArea, m_pTextBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
		m_pRenderTarget->EndDraw();
		
		ValidateRect(m_hWnd, NULL);

		return 0;

	case WM_SIZE:
		return 0;

	case WM_TIMER:
		switch (wParam) {
		case 1337:
			Repaint();
			return 1;
		}
		break;

	case WM_DISPLAYCHANGE:
		return 0;

	}
	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}