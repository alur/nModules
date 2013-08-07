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
void SelectionRectangle::Paint(ID2D1RenderTarget *renderTarget) {
    if (!mHidden) {
        renderTarget->FillRoundedRectangle(mRect, mBackBrush.brush);
        renderTarget->DrawRoundedRectangle(mRect, mOutlineBrush.brush, mOutlineWidth);
    }
}


/// <summary>
///
/// </summary>
void SelectionRectangle::DiscardDeviceResources() {
    mBackBrush.Discard();
    mOutlineBrush.Discard();
}


/// <summary>
///
/// </summary>
HRESULT SelectionRectangle::ReCreateDeviceResources(ID2D1RenderTarget *renderTarget) {
    mBackBrush.ReCreate(renderTarget);
    mOutlineBrush.ReCreate(renderTarget);

    return S_OK;
}


/// <summary>
///
/// </summary>
void SelectionRectangle::Init(Settings *parentSettings) {
    Settings *settings = parentSettings->CreateChild(_T("SelectionRectangle"));
    Settings *outlineSettings = parentSettings->CreateChild(_T("Outline"));
    mRect.radiusX = settings->GetFloat(_T("CornerRadiusX"), 0.0f);
    mRect.radiusY = settings->GetFloat(_T("CornerRadiusY"), 0.0f);
    mOutlineWidth = outlineSettings->GetFloat(_T("Width"),  0.5f);

    BrushSettings backDefaults;
    backDefaults.color = 0x887777EE;
    mBackBrushSettings.Load(settings, &backDefaults);
    mBackBrush.Load(&mBackBrushSettings);

    BrushSettings outlineDefaults;
    outlineDefaults.color = 0xDDFFFFFF;
    mOutlineBrushSettings.Load(settings, &outlineDefaults);
    mOutlineBrush.Load(&mOutlineBrushSettings);

    delete settings;
    delete outlineSettings;
}


/// <summary>
///
/// </summary>
void SelectionRectangle::SetRect(D2D1_RECT_F rect) {
    mBackBrush.UpdatePosition(rect);
    mOutlineBrush.UpdatePosition(rect);
    mRect.rect = rect;
}


/// <summary>
///
/// </summary>
void SelectionRectangle::Show() {
    mHidden = false;
}


/// <summary>
///
/// </summary>
void SelectionRectangle::Hide() {
    mHidden = true;
}
