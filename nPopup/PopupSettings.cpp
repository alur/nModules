/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupSettings.cpp
 *  The nModules Project
 *
 *  Contains all the settings used by button for a particular Popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "PopupSettings.hpp"


/// <summary>
/// Loads these settings from the settings file.
/// </summary>
void PopupSettings::Load(Settings *settings)
{
    // Load Command item settings
    {
        Settings *commandSettings = settings->CreateChild(L"Item");

        WindowSettings commandDefaults;
        commandDefaults.width = 190;
        commandDefaults.height = 20;
        mCommandWindowSettings.Load(commandSettings, &commandDefaults);
    
        StateRender<CommandItem::State>::InitData commandInitData;
        for (auto &stateInitData : commandInitData)
        {
            stateInitData.defaults.brushSettings[State::BrushType::Background].color = Color::Create(0xAAFFFF00);
            stateInitData.defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
            stateInitData.defaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
            stateInitData.defaults.textOffsetLeft = 20;
            stateInitData.defaults.textOffsetRight = 5;
        }
        commandInitData[CommandItem::State::Hover].prefix = L"Hover";
        mCommandStateRender.Load(commandInitData, commandSettings);

        delete commandSettings;
    }

    // Folder item settings
    {
        Settings *folderSettings = settings->CreateChild(L"FolderItem");

        WindowSettings folderDefaults;
        folderDefaults.width = 190;
        folderDefaults.height = 20;
        mFolderWindowSettings.Load(folderSettings, &folderDefaults);

        StateRender<nPopup::FolderItem::State>::InitData folderInitData;
        for (auto &stateInitData : folderInitData)
        {
            stateInitData.defaults.brushSettings[State::BrushType::Background].color = Color::Create(0xAA00FFFF);
            stateInitData.defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
            stateInitData.defaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
            stateInitData.defaults.textOffsetLeft = 20;
            stateInitData.defaults.textOffsetRight = 5;
        }
        folderInitData[nPopup::FolderItem::State::Hover].prefix = L"Hover";
        folderInitData[nPopup::FolderItem::State::Open].prefix = L"Open";
        mFolderStateRender.Load(folderInitData, folderSettings);

        delete folderSettings;
    }

    // Info item settings
    {
        Settings *infoSettings = settings->CreateChild(L"InfoItem");

        WindowSettings infoDefaults;
        infoDefaults.width = 190;
        infoDefaults.height = 20;
        infoDefaults.evaluateText = true;
        mInfoWindowSettings.Load(infoSettings, &infoDefaults);

        StateRender<InfoItem::State>::InitData infoInitData;
        for (auto &stateInitData : infoInitData)
        {
            stateInitData.defaults.brushSettings[State::BrushType::Background].color = Color::Create(0xAA00FF00);
            stateInitData.defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
            stateInitData.defaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
            stateInitData.defaults.textOffsetLeft = 20;
            stateInitData.defaults.textOffsetRight = 5;
        }
        infoInitData[InfoItem::State::Hover].prefix = L"Hover";
        mInfoStateRender.Load(infoInitData, infoSettings);

        delete infoSettings;
    }

    // Separator item settings
    {
        Settings *separatorSettings = settings->CreateChild(L"SeparatorItem");

        WindowSettings separatorDefaults;
        separatorDefaults.width = 190;
        separatorDefaults.height = 5;
        mSeparatorWindowSettings.Load(separatorSettings, &separatorDefaults);

        StateRender<SeparatorItem::State>::InitData separatorInitData;
        for (auto &stateInitData : separatorInitData)
        {
            stateInitData.defaults.brushSettings[State::BrushType::Background].color = Color::Create(0xAA000000);
            stateInitData.defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
        }
        separatorInitData[SeparatorItem::State::Hover].prefix = L"Hover";
        mSeparatorStateRender.Load(separatorInitData, separatorSettings);

        delete separatorSettings;
    }
}
