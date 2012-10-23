/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.cpp
 *  The nModules Project
 *
 *  Implementation of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "Label.hpp"
#include "../nShared/Macros.h"
#include <strsafe.h>

extern LSModule* g_LSModule;
extern map<string, Label*> g_AllLabels;


Label::Label(LPCSTR name) : Drawable(name) {
    Initalize();
}


Label::Label(LPCSTR name, Drawable* parent) : Drawable(parent, name, true) {
    Initalize();
}


Label::~Label() {
    // Remove all overlays
    for (list<Drawable*>::const_iterator iter = this->overlays.begin(); iter != this->overlays.end(); iter++) {
        delete *iter;
    }
    this->overlays.clear();

    g_AllLabels.erase(this->allLabelsIter);
}


void Label::Initalize() {
    this->allLabelsIter = g_AllLabels.insert(g_AllLabels.begin(), pair<string, Label*>(string(this->settings->prefix), this));
    
    DrawableSettings* defaults = new DrawableSettings();
    defaults->evaluateText = true;
    defaults->registerWithCore = true;

    this->window->Initialize(defaults);

    LoadSettings();

    this->stateHover = this->window->AddState("Hover", new DrawableSettings(), 100);
    this->window->Show();
}


void Label::LoadSettings(bool /* isRefresh */) {
    // Load overlays.
    char line[MAX_LINE_LENGTH], prefix[256], label[256];
    LPSTR tokens[] = { label };
    LPVOID f = LiteStep::LCOpen(NULL);

    StringCchPrintf(prefix, sizeof(prefix), "*%sOverlayLabel", this->settings->prefix);

    while (LiteStep::LCReadNextConfig(f, prefix, line, sizeof(line))) {
        LiteStep::LCTokenize(line+strlen(prefix)+1, tokens, 1, NULL);
        this->overlays.push_back(new Label(label, this));
    }
    LiteStep::LCClose(f);
}


LRESULT WINAPI Label::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_MOUSEMOVE) {
        this->window->ActivateState(this->stateHover);
    }
    else if (uMsg == WM_MOUSELEAVE) {
        this->window->ClearState(this->stateHover);
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
