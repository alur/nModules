/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ClockHand.hpp
 *  The nModules Project
 *
 *  A hand on a clock.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "ClockHand.hpp"


/// <summary>
/// Constructor
/// </summary>
ClockHand::ClockHand()
{
}


/// <summary>
/// Destructor
/// </summary>
ClockHand::~ClockHand()
{
}


/// <summary>
/// IPaintable::Paint
/// Paints the hand to the render target.
/// </summary>
void ClockHand::Paint(ID2D1RenderTarget *renderTarget)
{
    D2D1_MATRIX_3X2_F currentTransform;
    renderTarget->GetTransform(&currentTransform);

    renderTarget->SetTransform(currentTransform*D2D1::Matrix3x2F::Rotation(mRotation)*D2D1::Matrix3x2F::Translation(mCenterPoint));

    renderTarget->FillRectangle(mHandRect, mBrush.brush);

    renderTarget->SetTransform(currentTransform);
}


/// <summary>
/// IPaintable::DiscardDeviceResources
/// Releases all D2D resources.
/// </summary>
void ClockHand::DiscardDeviceResources()
{
    mBrush.Discard();
}


/// <summary>
/// IPaintable::ReCreateDeviceResources
/// (Re)Creates all D2D resources.
/// </summary>
HRESULT ClockHand::ReCreateDeviceResources(ID2D1RenderTarget *renderTarget)
{
    return mBrush.ReCreate(renderTarget);
}


/// <summary>
/// Initializes this clock hand.
/// </summary>
void ClockHand::Initialize(Settings *clockSettings, LPCTSTR prefix, float maxValue)
{
    Settings *settings = clockSettings->CreateChild(prefix);

    mSmoothMovement = settings->GetBool(_T("SmoothMovement"), false);
    mMaxValue = maxValue;

    BrushSettings brushDefaults;
    brushDefaults.color = 0xFF77FFEE;
    mBrushSettings.Load(settings, &brushDefaults);
    mBrush.Load(&mBrushSettings);

    float parentWidth = clockSettings->GetFloat(_T("Width"), 100.0f);
    float parentHeight = clockSettings->GetFloat(_T("Height"), 100.0f);
    float parentX = clockSettings->GetFloat(_T("X"), 100.0f);
    float parentY = clockSettings->GetFloat(_T("Y"), 100.0f);

    float length = settings->GetFloat(_T("Length"), 50);
    float thickness = settings->GetFloat(_T("Thickness"), 3);
    float offset = settings->GetFloat(_T("Offset"), 0);

    mHandRect.left = offset;
    mHandRect.top = -thickness / 2.0f;
    mHandRect.bottom = mHandRect.top + thickness;
    mHandRect.right = mHandRect.left + length;

    mCenterPoint = D2D1::SizeF(parentWidth / 2.0f + parentX, parentHeight / 2.0f + parentY);

    delete settings;
}


/// <summary>
/// Sets the rotation of the clock hand.
/// </summary>
void ClockHand::SetValue(float value)
{
    if (value > mMaxValue)
    {
        value -= mMaxValue;
    }

    if (!mSmoothMovement)
    {
        value = floorf(value);
    }

    mRotation = value * 360.0f / mMaxValue - 90.0f;
}
