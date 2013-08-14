/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SelectionRectangle.cpp
 *  The nModules Project
 *
 *  Draws the selection rectangle, as a post-processing step.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "SelectionRectangle.hpp"


/// <summary>
///
/// </summary>
SelectionRectangle::SelectionRectangle()
{
    mHidden = true;
}


/// <summary>
///
/// </summary>
SelectionRectangle::~SelectionRectangle()
{
}


/// <summary>
///
/// </summary>
void SelectionRectangle::Paint(ID2D1RenderTarget *renderTarget)
{
    if (!mHidden)
    {
        if (mBackBrush.IsImageEdgeBrush())
        {
            for (Brush::EdgeType type = Brush::EdgeType(0); type != Brush::EdgeType::Count;
                type = Brush::EdgeType(std::underlying_type<Brush::EdgeType>::type(type) + 1))
            {
                renderTarget->FillRectangle(mBackBrush.GetImageEdgeRectAndScaleBrush(type), mBackBrush.brush);
            }
        }
        else
        {
            renderTarget->FillRoundedRectangle(mRect, mBackBrush.brush);
        }
        renderTarget->DrawRoundedRectangle(mRect, mOutlineBrush.brush, mOutlineWidth);
    }
}


/// <summary>
///
/// </summary>
void SelectionRectangle::DiscardDeviceResources()
{
    mBackBrush.Discard();
    mOutlineBrush.Discard();
}


/// <summary>
///
/// </summary>
HRESULT SelectionRectangle::ReCreateDeviceResources(ID2D1RenderTarget *renderTarget)
{
    mBackBrush.ReCreate(renderTarget);
    mOutlineBrush.ReCreate(renderTarget);

    return S_OK;
}


/// <summary>
/// IPaintable::UpdatePosition
/// Called when the parent has moved.
/// </summary>
void SelectionRectangle::UpdatePosition(D2D1_RECT_F /* parentPosition */)
{
    // This is handled in SetRect instead.
}


/// <summary>
/// IPaintable::UpdateDWMColor
/// Called when the DWM color has changed. Returns true if this Paintable is currently using the DWM color.
/// </summary>
bool SelectionRectangle::UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget)
{
    return mBackBrush.UpdateDWMColor(newColor, renderTarget);
}


/// <summary>
///
/// </summary>
void SelectionRectangle::Init(Settings *parentSettings)
{
    Settings *settings = parentSettings->CreateChild(_T("SelectionRectangle"));
    Settings *outlineSettings = settings->CreateChild(_T("Outline"));
    mRect.radiusX = settings->GetFloat(_T("CornerRadiusX"), 0.0f);
    mRect.radiusY = settings->GetFloat(_T("CornerRadiusY"), 0.0f);
    mOutlineWidth = outlineSettings->GetFloat(_T("Width"),  0.5f);

    BrushSettings backDefaults;
    backDefaults.color = Color::Create(0x887777EE);
    mBackBrushSettings.Load(settings, &backDefaults);
    mBackBrush.Load(&mBackBrushSettings);

    BrushSettings outlineDefaults;
    outlineDefaults.color = Color::Create(0xDDFFFFFF);
    mOutlineBrushSettings.Load(outlineSettings, &outlineDefaults);
    mOutlineBrush.Load(&mOutlineBrushSettings);

    delete settings;
    delete outlineSettings;
}


/// <summary>
///
/// </summary>
void SelectionRectangle::SetRect(D2D1_RECT_F rect)
{
    mBackBrush.UpdatePosition(rect);
    mRect.rect = mBackBrush.brushPosition;
    mOutlineBrush.UpdatePosition(mRect.rect);
}


/// <summary>
///
/// </summary>
const D2D1_RECT_F &SelectionRectangle::GetRect() const
{
    return mRect.rect;
}


/// <summary>
///
/// </summary>
void SelectionRectangle::Show()
{
    mHidden = false;
}


/// <summary>
///
/// </summary>
void SelectionRectangle::Hide()
{
    mHidden = true;
}
