/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ClickHandler.hpp
 *  The nModules Project
 *
 *  Function declarations for the ClickHandler class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <vector>
#include "../nShared/MessageHandler.hpp"

using std::vector;

class ClickHandler : public MessageHandler {
public:
    explicit ClickHandler();
    virtual ~ClickHandler();

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
    void Refresh();

    void AddHandler(LPCSTR);
    void RemoveHandlers(LPCSTR);

private:
    enum ClickType {
        WHEELUP, WHEELDOWN, WHEELRIGHT, WHEELLEFT,
        LEFTDOWN, LEFTUP, LEFTDOUBLE,
        MIDDLEDOWN, MIDDLEUP, MIDDLEDOUBLE,
        RIGHTDOWN, RIGHTUP, RIGHTDOUBLE,
        X1DOWN, X1UP, X1DOUBLE,
        X2DOWN, X2UP, X2DOUBLE,
        DROP,
        UNKNOWN
    };

    typedef struct ClickData {
        ClickType type;
        WORD mods; // Modifier keys
        RECT area; // Region of the virtual screen where this event is valid
        char action[MAX_LINE_LENGTH]; // Action to fire when this event occurs
    } ClickData;

    vector<ClickData> m_clickHandlers;

    void LoadSettings(bool = false);
    ClickData ParseLine(LPCSTR);
    ClickType TypeFromString(LPCSTR str);
    WORD ModsFromString(LPSTR str);
    bool Matches(ClickData a, ClickData b);
};
