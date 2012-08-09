/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*	GridEffect.cpp												  August, 2012
*	The nModules Project
*
*	
*      
*													             Erik Welander
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "GridEffect.hpp"
#include <ctime>
#include <math.h>

/// <summary>
///	Constructor
/// </summary>
GridEffect::GridEffect(GridType fadeType, GridStyle gridStyle)
{
    m_gridType = fadeType;
    m_gridStyle = gridStyle;
    srand((unsigned)time(NULL));
    m_StartTimes = NULL;
    m_Squares = NULL;
    m_iSquaresY = 0;
    m_iSquaresX = 0;
    m_iSquares = 0;
}

/// <summary>
///	Destructor
/// </summary>
GridEffect::~GridEffect()
{
    free(m_StartTimes);
    free(m_Squares);
}

/// <summary>
///	Initializes the effect
/// </summary>
void GridEffect::Initialize(TransitionSettings* transitionSettings)
{
    m_pTransitionSettings = transitionSettings;
    Resize();
}

/// <summary>
///	Called just before a new painting session is about to begin
/// </summary>
void GridEffect::Start(ID2D1BitmapBrush* oldBrush, ID2D1BitmapBrush* newBrush)
{
    if (m_gridStyle == HIDE_OLD)
    {
        m_pNewBrush = oldBrush;
        m_pOldBrush = newBrush;
    }
    else
    {
        m_pOldBrush = oldBrush;
        m_pNewBrush = newBrush;
    }

    // Generate values in the range [0, 1 - fadeTime]
    int i = 0, x = 0, y = 0;
    switch (m_gridType)
    {
    case RANDOM:
        {
            for (i = 0; i < m_iSquares; i++)
            {
                m_StartTimes[i] = (rand() % 10000*(1.0f - m_pTransitionSettings->fFadeTime)) / 10000.0f;
            }
        }
        break;

    case LINEAR_HORIZONTAL:
        for (x = 0; x < m_iSquaresX; x++)
        {
            for (y = 0; y < m_iSquaresY; y++)
            {
                m_StartTimes[y*m_iSquaresX + x] = (i++)*(1.0f - m_pTransitionSettings->fFadeTime)/m_iSquares;
            }
        }
        break;

    case LINEAR_VERTICAL:
        {
            for (i = 0; i < m_iSquares; i++)
            {
                m_StartTimes[i] = i*(1.0f - m_pTransitionSettings->fFadeTime)/m_iSquares;
            }
        }
        break;

    case TRIANGULAR:
        {
            //
            float alpha = (float)atan2((double)m_iSquaresY, (double)m_iSquaresX);
            float up = sqrt(float(m_iSquaresX*m_iSquaresX+m_iSquaresY*m_iSquaresY))/(1.0f - m_pTransitionSettings->fFadeTime);
            for (x = 0; x < m_iSquaresX; x++)
            {
                for (y = 0; y < m_iSquaresY; y++)
                {
                    m_StartTimes[y*m_iSquaresX + x] = sqrt(float(x*x+y*y))*cos(atan2((float)y, (float)x) - alpha)/up;
                }
            }
        }
        break;
    }
}

/// <summary>
///	Paints the state of the animation at progress to the specified rendertarget
/// </summary>
void GridEffect::Paint(ID2D1RenderTarget* renderTarget, float fProgress)
{
    if (m_gridStyle == HIDE_OLD)
    {
        fProgress = 1.0f - fProgress; // Run in reverse
    }

    renderTarget->FillRectangle(m_pTransitionSettings->WPRect, m_pOldBrush);
    for (int i = 0; i < m_iSquares; i++)
    {
        m_pNewBrush->SetOpacity(min(1.0f, max(fProgress - m_StartTimes[i], 0.0f)/m_pTransitionSettings->fFadeTime));
        renderTarget->FillRectangle(m_Squares[i], m_pNewBrush);
    }
}

/// <summary>
///	End of the effect. Should cleanup.
/// </summary>
void GridEffect::End()
{
    m_pNewBrush->SetOpacity(1.0f);

    m_pOldBrush = NULL;
    m_pNewBrush = NULL;
}

/// <summary>
///	Called when the rendertarget has been resized
/// </summary>
void GridEffect::Resize()
{
    m_iSquaresY = (int)ceil(m_pTransitionSettings->WPRect.right/m_pTransitionSettings->iSquareSize);
    m_iSquaresX = (int)ceil(m_pTransitionSettings->WPRect.bottom/m_pTransitionSettings->iSquareSize);
    m_iSquares = m_iSquaresY * m_iSquaresX;

    // Reallocate memory for the squares
    m_StartTimes = (float*)realloc(m_StartTimes, sizeof(float)*m_iSquares);
    m_Squares = (D2D1_RECT_F*)realloc(m_Squares, sizeof(D2D1_RECT_F)*m_iSquares);

    // Work out the square positions
    int square = 0;
    for (float x = m_pTransitionSettings->WPRect.left; x < m_pTransitionSettings->WPRect.right; x += m_pTransitionSettings->iSquareSize)
    {
        for (float y = m_pTransitionSettings->WPRect.top; y < m_pTransitionSettings->WPRect.bottom; y += m_pTransitionSettings->iSquareSize)
        {
            m_Squares[square].left = x;
            m_Squares[square].right = x + m_pTransitionSettings->iSquareSize;
            m_Squares[square].top = y;
            m_Squares[square].bottom = y + m_pTransitionSettings->iSquareSize;
            square++;
        }
    }
}