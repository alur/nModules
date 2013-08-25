/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.hpp
 *  The nModules Project
 *
 *  Declaration of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/Window.hpp"
#include "../nShared/StateRender.hpp"


class Label : public Drawable {
public:
    explicit Label(LPCTSTR name);
    explicit Label(LPCTSTR name, Drawable* parent);
    virtual ~Label();
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

private:
    enum class States
    {
        Base = 0,
        Hover,
        Count
    };

private:
    void Initalize();
    void LoadSettings(bool isRefresh = false);

    StateRender<States> mStateRender;
    
    list<Drawable*> mOverlays;
    map<wstring, Label*>::iterator mAllLabelsIter;
};
