/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LayoutSettings.cpp
 *  The nModules Project
 *
 *  Settings used for laying out items in a rectangle.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "LayoutSettings.hpp"


/// <summary>
/// Initalizes the class to all default settings.
/// </summary>
LayoutSettings::LayoutSettings() {
    this->columnSpacing = 2;
    this->padding.left = 0;
    this->padding.top = 0;
    this->padding.right = 0;
    this->padding.bottom = 0;
    this->primaryDirection = Direction::Horizontal;
    this->rowSpacing = 2;
    this->startPosition = StartPosition::TopLeft;
}


/// <summary>
/// Destructor.
/// </summary>
LayoutSettings::~LayoutSettings() {
}


/// <summary>
/// Loads settings from an RC file using the specified defaults.
/// </summary>
void LayoutSettings::Load(Settings* settings, LayoutSettings* defaults) {
    char buffer[32];

    this->columnSpacing = settings->GetInt("ColumnSpacing", defaults->columnSpacing);
    settings->GetOffsetRect("PaddingLeft", "PaddingTop", "PaddingRight", "PaddingBottom", &this->padding, &defaults->padding);

    settings->GetString("Start", buffer, sizeof(buffer), "TopLeft");
    if (_stricmp(buffer, "TopRight") == 0) {
        this->startPosition = StartPosition::TopRight;
    }
    else if (_stricmp(buffer, "BottomLeft") == 0) {
        this->startPosition = StartPosition::BottomLeft;
    }
    else if (_stricmp(buffer, "BottomRight") == 0) {
        this->startPosition = StartPosition::BottomRight;
    }
    else {
        this->startPosition = StartPosition::TopLeft;
    }

    this->rowSpacing = settings->GetInt("RowSpacing", defaults->rowSpacing);

    settings->GetString("PrimaryDirection", buffer, sizeof(buffer), "Horizontal");
    if (_stricmp(buffer, "Vertical") == 0) {
        this->primaryDirection = Direction::Vertical;
    }
    else {
        this->primaryDirection = Direction::Horizontal;
    }
}
