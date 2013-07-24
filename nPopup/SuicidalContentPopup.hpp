/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SuicidalContentPopup.hpp
 *  The nModules Project
 *
 *  A content popup which destroys itself when closed.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "ContentPopup.hpp"

class SuicidalContentPopup : public ContentPopup {
public:
    explicit SuicidalContentPopup(LPCSTR path, LPCSTR title, LPCSTR prefix);

public:
    void Close() override;
};
