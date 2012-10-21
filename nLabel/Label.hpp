/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.hpp
 *  The nModules Project
 *
 *  Declaration of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"

class Label : public Drawable {
public:
    explicit Label(LPCSTR name);
    explicit Label(LPCSTR name, Drawable* parent);
    virtual ~Label();
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
    void Initalize();
    void LoadSettings(bool isRefresh = false);

    DrawableWindow::STATE stateHover;
    list<Drawable*> overlays;
    map<string, Label*>::iterator allLabelsIter;
};
