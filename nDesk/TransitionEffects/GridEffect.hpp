/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  GridEffect.hpp                                                 August, 2012
*  The nModules Project
*
*  Function declarations for the GridEffect class.
*  
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef GRIDEFFECT_HPP
#define GRIDEFFECT_HPP

#include "../TransitionEffect.hpp"

class GridEffect : public TransitionEffect {
public:
    //
    enum GridType {
        RANDOM,
        LINEAR_VERTICAL,
        LINEAR_HORIZONTAL,
        TRIANGULAR,
		CLOCKWISE,
        COUNTERCLOCKWISE
    };

    //
    enum GridStyle {
        SHOW_NEW,
        HIDE_OLD
    };

    explicit GridEffect(GridType gridType, GridStyle gridStyle);
    virtual ~GridEffect();

    void Initialize(TransitionSettings* transitionSettings);
    void Start(ID2D1BitmapBrush* oldBrush, ID2D1BitmapBrush* newBrush);
    void Paint(ID2D1RenderTarget* renderTarget, float fProgress);
    void Resize();
    void End();

private:
    // The type of fade we are doing
    GridType m_gridType;

    // The
    GridStyle m_gridStyle;

    // When to start showing the square
    float* m_StartTimes;

    // Where the squares are of the screen
    D2D1_RECT_F* m_Squares;

    // The number of squares in a row/column
    int m_iSquaresY, m_iSquaresX, m_iSquares;
};

#endif /* GRIDEFFECT_HPP */
