//--------------------------------------------------------------------------------------
// BrushBangs.cpp
// The nModules Project
//
// Bangs for brushes
//
//--------------------------------------------------------------------------------------
#include "BrushBangs.h"
#include "LiteStep.h"

#include "../nCoreCom/Core.h"

#include <strsafe.h>

// Used to map window name -> window
static std::function<Window* (LPCTSTR)> windowFinder;


/// <summary>
/// Retrieves the brush and owning window
/// </summary>
static Brush *FindBrush(LPCTSTR *args, int numArgs, Window *&window)
{
    TCHAR buffer[MAX_RCCOMMAND];
    int numTokens = LiteStep::CommandTokenize(*args, nullptr, 0, nullptr);

    // Order of precedence
    // window
    // window brushowner
    // window brushowner brush
    // window brush
    if (numTokens == numArgs + 1 || numTokens == numArgs + 2 || numTokens == numArgs + 3)
    {
        LiteStep::GetToken(*args, buffer, args, 0);
        window = windowFinder(buffer);
        if (window)
        {
            if (numTokens == numArgs + 2 || numTokens == numArgs + 3)
            {
                LiteStep::GetToken(*args, buffer, args, 0);
                IBrushOwner *owner = window->GetBrushOwner(buffer);
                if (owner)
                {
                    if (numTokens == numArgs + 3) // window brushowner brush
                    { 
                        LiteStep::GetToken(*args, buffer, args, 0);
                        return owner->GetBrush(buffer);
                    }
                    else // window brushowner
                    { 
                        return owner->GetBrush(L"");
                    }
                }
                else if (numTokens == numArgs + 2) // Window Brush
                { 
                    owner = window->GetBrushOwner(L"");
                    if (owner)
                    {
                        return owner->GetBrush(buffer);
                    }
                }
            }
            else // Window
            { 
                IBrushOwner *owner = window->GetBrushOwner(L"");
                if (owner)
                {
                    return owner->GetBrush(L"");
                }
            }
        }
    }

    return nullptr;
}


static const struct BangItem
{
    BangItem(LPCTSTR name, LiteStep::BANGCOMMANDPROC proc)
    {
        this->name = name;
        this->proc = proc;
    }

    LPCTSTR name;
    LiteStep::BANGCOMMANDPROC proc;
} BangMap [] =
{
    /*(BangItem(L"SetBrushType",            [] (HWND, LPCTSTR args) -> void
    {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush)
        {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            brush->SetType(BrushSetings::ParseType(arg));
            window->Repaint();
        }
    }),*/
    BangItem(L"SetColor",                [] (HWND, LPCTSTR args) -> void
    {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush)
        {
            TCHAR arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            IColorVal* color;
            if (ParseColor(arg, &color))
            {
                brush->SetColor(color);
                delete color;
                window->Repaint();
            }
        }
    }),
    /*BangItem(L"SetAlpha",                [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    }), 
    BangItem(L"SetGradientColors",       [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    }),
    BangItem(L"SetGradientStops",        [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    }),
    BangItem(L"SetGradientStart",        [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    }),
    BangItem(L"SetGradientEnd",          [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    }),
    BangItem(L"SetGradientCenter",       [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    }),
    BangItem(L"SetGradientRadius",       [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    }),
    BangItem(L"SetGradientOriginOffset", [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    }),*/
    BangItem(L"SetImage",                [] (HWND, LPCTSTR args) -> void
    {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush)
        {
            TCHAR arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            brush->SetImage(window->GetRenderTarget(), arg);
            window->Repaint();
        }
    }),
    /*BangItem(L"SetImageRotation",        [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            brush->SetImageRotation(strtof(arg, nullptr));
            window->Repaint();
        }
    }),*/
    /*BangItem(L"SetImageScalingMode",     [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    }),
    BangItem(L"SetTilingMode",           [] (HWND, LPCTSTR args) -> void {
        Window *window = nullptr;
        Brush *brush = FindBrush(&args, 1, window);
        if (brush) {
            char arg[MAX_RCCOMMAND];
            LiteStep::GetToken(args, arg, nullptr, 0);
            window->Repaint();
        }
    })*/
};


/// <summary>
/// Registers bangs.
/// </summary>
void BrushBangs::Register(LPCTSTR prefix, std::function<Window* (LPCTSTR)> windowFinder)
{
    TCHAR bangName[64];
    ::windowFinder = windowFinder;
    for (BangItem item : BangMap)
    {
        StringCchPrintf(bangName, _countof(bangName), L"!%s%s", prefix, item.name);
        LiteStep::AddBangCommand(bangName, item.proc);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void BrushBangs::UnRegister(LPCTSTR prefix)
{
    TCHAR bangName[64];
    for (BangItem item : BangMap)
    {
        StringCchPrintf(bangName, _countof(bangName), L"!%s%s", prefix, item.name);
        LiteStep::RemoveBangCommand(bangName);
    }
    ::windowFinder = nullptr;
}
