/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  FadeEffect.hpp                                                 August, 2012
*  The nModules Project
*
*  Function declarations for the FadeEffect class.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef FADEEFFECT_HPP
#define FADEEFFECT_HPP

#include "../TransitionEffect.hpp"

class FadeEffect : public TransitionEffect {
public:
    enum FadeType {
        FADE_IN,
        FADE_OUT
    };

    explicit FadeEffect(FadeType fadeType);
    virtual ~FadeEffect();

    void Initialize(TransitionSettings* transitionSettings);
    void Start(ID2D1BitmapBrush* oldBrush, ID2D1BitmapBrush* newBrush);
    void Paint(ID2D1RenderTarget* renderTarget, float fProgress);
    void Resize();
    void End();

private:
    // The type of fade we are doing
    FadeType m_fadeType;
};

#endif /* FADEEFFECT_HPP */
