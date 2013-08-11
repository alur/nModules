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

class Label : public Drawable {
public:
    explicit Label(LPCTSTR name);
    explicit Label(LPCTSTR name, Drawable* parent);
    virtual ~Label();
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

private:
    void Initalize();
    void LoadSettings(bool isRefresh = false);

    Window::STATE stateHover;
    list<Drawable*> overlays;
    map<wstring, Label*>::iterator allLabelsIter;
};
