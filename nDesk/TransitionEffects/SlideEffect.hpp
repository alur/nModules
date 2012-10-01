/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  SlideEffect.hpp
*  The nModules Project
*
*  Function declarations for the SlideEffect class.
*  
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../TransitionEffect.hpp"

class SlideEffect : public TransitionEffect {
public:
    enum SlideDirection {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    enum SlideType {
        // Slides both the new and old image as one
        BOTH,

        // Slides the new image in above the old
        NEW,

        // Slides the old image out above the new
        OLD,

        // Slides none of the images
        SCAN
    };

    explicit SlideEffect(SlideType slideType, SlideDirection slideDirection);
    virtual ~SlideEffect();

    void Initialize(TransitionSettings* transitionSettings);
    void Start(ID2D1BitmapBrush* oldBrush, ID2D1BitmapBrush* newBrush);
    void Paint(ID2D1RenderTarget* renderTarget, float fProgress);
    void Resize();
    void End();

private:
    // The type of slide we are doing
    SlideType m_slideType;

    // The direction we are sliding in
    SlideDirection m_SlideDirection;

    // True if we should slide the new image
    bool m_bSlideNew;

    // True if we should slide the old image
    bool m_bSlideOld;

    // Rectangle of the new image
    D2D1_RECT_F m_newRect;

    // Rectangle of the old image
    D2D1_RECT_F m_oldRect;
};
