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
    explicit SuicidalContentPopup(LPCTSTR path, LPCTSTR title, LPCTSTR prefix);

public:
    void PostClose() override;
};
