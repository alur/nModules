/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  State.hpp
 *  The nModules Project
 *
 *  Defines a "State" which the window can be in.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

class State;

#include "Brush.hpp"
#include "IPainter.hpp"
#include "IBrushOwner.hpp"
#include "StateTextRender.hpp"
#include "Window.hpp"

class State : public IBrushOwner
{
    friend class StateTextRender;

public:
    enum class BrushType
    {
        Background = 0,
        Outline,
        Text,
        TextStroke,
        Count
    };

    struct WindowData
    {
        WindowData()
            : textLayout(nullptr)
        {
        }

        ~WindowData()
        {
            if (textLayout != nullptr)
            {
                textLayout->Release();
            }
        }

        // The area we draw text in
        D2D1_RECT_F textArea;

        // The area we draw in.
        D2D1_ROUNDED_RECT drawingArea;
    
        // The area we draw the outline in.
        D2D1_ROUNDED_RECT outlineArea;

        // The point we rotate text around.
        D2D1_POINT_2F textRotationOrigin;

        // Per-brush window data.
        EnumArray<Brush::WindowData, BrushType> brushData;

        //
        IDWriteTextLayout *textLayout;
    };

public:
    class Settings
    {
    public:
        Settings();

        // Loads the actual settings.
        void Load(const ::Settings * settings, const Settings * defaults);
    
    public:
        static DWRITE_FONT_STRETCH ParseFontStretch(LPCTSTR string);
        static DWRITE_FONT_STYLE ParseFontStyle(LPCTSTR fontStyle);
        static DWRITE_FONT_WEIGHT ParseFontWeight(LPCTSTR fontWeight);
        static DWRITE_TEXT_ALIGNMENT ParseTextAlignment(LPCTSTR textAlignment);
        static DWRITE_PARAGRAPH_ALIGNMENT ParseParagraphAlignment(LPCTSTR paragraphAlignment);
        static DWRITE_TRIMMING_GRANULARITY ParseTrimmingGranularity(LPCTSTR trimmingGranularity);
        static DWRITE_READING_DIRECTION ParseReadingDirection(LPCTSTR readingDirection);
        static DWRITE_WORD_WRAPPING ParseWordWrapping(LPCTSTR wordWrapping);

    public:
        // The settings to use for the brushes.
        EnumArray<BrushSettings, State::BrushType> brushSettings;

        // The x corner radius. Default: 0
        float cornerRadiusX;

        // The y corner radius. Default: 0
        float cornerRadiusY;

        // The default font to use. Default: Arial
        WCHAR font[MAX_PATH];

        // The default font size. Default: 12
        float fontSize;

        // The default font stretch. Ultra Condensed, Extra Condensed, Condensed, 
        // Semi Condensed, Normal, Medium, Semi Expanded, Expanded, Extra Expanded,
        // Ultra Expanded. Default: Normal
        DWRITE_FONT_STRETCH fontStretch;

        // The default font style. Normal, Oblique, Italic. Default: Normal
        DWRITE_FONT_STYLE fontStyle;

        // The default font weight. Thin, Extra Light, Ultra Light, Light,
        // Semi Light, Normal, Regular, Medium, Semi Bold, Bold, Extra Bold, 
        // Ultra Bold, Black, Heavy, Extra Black, Ultra Black. Default: Normal
        DWRITE_FONT_WEIGHT fontWeight;

        // The width of the outline. Default: 0
        float outlineWidth;

        //
        DWRITE_READING_DIRECTION readingDirection;

        // The horizontal alignment of the text. Left, Center, Right. Default: Left
        DWRITE_TEXT_ALIGNMENT textAlign;

        // Text offset from the bottom. Default: 0
        float textOffsetBottom;

        // Text offset from the left. Default: 0
        float textOffsetLeft;

        // Text offset from the right. Default: 0
        float textOffsetRight;

        // Text offset from the top. Default: 0
        float textOffsetTop;

        // Text rotation. Default: 0
        float textRotation;

        // Text stroke width. Default: 0
        float fontStrokeWidth;

        // The trimming setting. None, Character, Word. Default: Character
        DWRITE_TRIMMING_GRANULARITY textTrimmingGranularity;

        // The vertical alignment of the text. Bottom, Middle, Top. Default: Top
        DWRITE_PARAGRAPH_ALIGNMENT textVerticalAlign;

        // 
        DWRITE_WORD_WRAPPING wordWrapping;
    };

public:
    explicit State();
    virtual ~State();

public:
    void Load(const Settings * defaultSettings, const ::Settings * settings, LPCTSTR name);
    const Settings * GetSettings();
        
    // Gets the "desired" size for a given width and height.
    void GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size, class Window *window);

    // IPainter
public:
    void DiscardDeviceResources();
    void Paint(ID2D1RenderTarget* renderTarget, WindowData *windowData);
    void PaintText(ID2D1RenderTarget* renderTarget, WindowData *windowData, class Window *window);
    HRESULT ReCreateDeviceResources(ID2D1RenderTarget* renderTarget);
    void UpdatePosition(D2D1_RECT_F position, WindowData *windowData);
    bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget);

    // IBrushOwner
public:
    Brush *GetBrush(LPCTSTR name) override;

public:
    void SetCornerRadiusX(float radius);
    void SetCornerRadiusY(float radius);
    void SetOutlineWidth(float width);

    void SetTextOffsets(float left, float top, float right, float bottom);

    void SetReadingDirection(DWRITE_READING_DIRECTION direction);
    void SetTextAlignment(DWRITE_TEXT_ALIGNMENT alignment);
    void SetTextRotation(float rotation);
    void SetTextTrimmingGranuality(DWRITE_TRIMMING_GRANULARITY granularity);
    void SetTextVerticalAlign(DWRITE_PARAGRAPH_ALIGNMENT alignment);
    void SetWordWrapping(DWRITE_WORD_WRAPPING wrapping);

public:
    // The name of this state.
    LPCTSTR mName;

    // Settings.
    const ::Settings * settings;

private:
    // Creates the text format for this state.
    HRESULT CreateTextFormat(IDWriteTextFormat *&textFormat);

    // Creates 
    HRESULT CreateBrush(BrushSettings* settings, ID2D1Brush* brush);

private:
    // The current drawing settings.
    Settings mStateSettings;

    // Our brushes.
    EnumArray<Brush, BrushType> mBrushes;

    // Defines how the text is formatted.
    IDWriteTextFormat* textFormat;

    StateTextRender *mTextRender;
};
