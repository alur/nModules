#include "Popup.hpp"
#include "nPopup.h"

#include "../nModuleBase/nModule.hpp"

#include "../nShared/AlgorithmExt.h"
#include "../nShared/LiteStep.h"
#include "../nShared/String.h"

#include "../Headers/lsapi.h"

#include <strsafe.h>
#include <unordered_map>

NModule gModule(L"nLabel", MakeVersion(1, 0, 0, 0), MakeVersion(1, 0, 0, 0));

static const UINT sLsMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
static StringKeyedMaps<std::wstring, Popup*>::UnorderedMap gPopups;


void __cdecl HandlePopupBang(HWND /* owner */, LPCWSTR bang, LPCWSTR args) {
  Popup *popup = nstd::get(gPopups, std::wstring(bang), (Popup*)nullptr);
  if (popup != nullptr) {
    wchar_t x[16], y[16];
    LPWSTR tokens[] = { x, y };
    if (LCTokenize(args, tokens, 2, nullptr) == 2) {
      popup->Show(_wtoi(x), _wtoi(y));
    } else {
      popup->Show();
    }
  }
}


static void DestroyPopups() {
  for (auto popup : gPopups) {
    RemoveBangCommand(popup.first.c_str());
    delete popup.second;
  }
  gPopups.clear();
}


static void LoadPopups() {
  // Pre-defined popups
  /*AddPopup(L"!PopupAdminTools", new ContentPopup(ContentPopup::ContentSource::ADMIN_TOOLS, L"Admin Tools", L"nPopup"));
  AddPopup(L"!PopupControlPanel", new ContentPopup(ContentPopup::ContentSource::CONTROL_PANEL, L"Control Panel", L"nPopup"));
  AddPopup(new ContentPopup(ContentPopup::ContentSource::MY_COMPUTER, L"My Computer", L"!PopupMyComputer", L"nPopup"));
  AddPopup(new ContentPopup(ContentPopup::ContentSource::NETWORK, L"Network", L"!PopupNetwork", L"nPopup"));
  AddPopup(new ContentPopup(ContentPopup::ContentSource::PRINTERS, L"Printers", L"!PopupPrinters", L"nPopup"));
  AddPopup(new ContentPopup(ContentPopup::ContentSource::PROGRAMS, L"Programs", L"!PopupPrograms", L"nPopup"));
  AddPopup(new ContentPopup(ContentPopup::ContentSource::RECENT_DOCUMENTS, L"Recent Documents", L"!PopupRecentDocuments", L"nPopup"));
  AddPopup(new ContentPopup(ContentPopup::ContentSource::RECYCLE_BIN, L"Recycle Bin", L"!PopupRecycleBin", L"nPopup"));
  AddPopup(new ContentPopup(ContentPopup::ContentSource::START_MENU, L"Start Menu", L"!PopupStartMenu", L"nPopup"));*/

  // RC Popups
  Popup *popup;
  wchar_t bang[MAX_BANGCOMMAND];
  LPVOID f = LCOpen(nullptr);
  while (LoadPopup(f, PopupLevel::Root, popup, bang, _countof(bang), L"nPopup")) {
    AddPopup(bang, popup);
  }
  LCClose(f);
}


static void AddPopup(LPCWSTR bang, Popup *popup) {
  gPopups.emplace(bang, popup);
  AddBangCommandEx(bang, HandlePopupBang);
}


static bool LoadPopup(LPVOID f, PopupLevel level, Popup *&popup, LPWSTR bang, size_t cchBang,
    LPCWSTR parentPrefix) {
  wchar_t line[MAX_LINE_LENGTH], title[MAX_LINE_LENGTH], command[MAX_LINE_LENGTH],
    icon[MAX_LINE_LENGTH], prefix[MAX_LINE_LENGTH];

  while (LCReadNextConfig(f, L"*Popup", line, _countof(line))) {
    PopupLineType type = ParseLine(line, title, _countof(title), command, _countof(command), icon,
      _countof(icon), prefix, _countof(prefix));
    if (level == PopupLevel::Root) {
      if (type == PopupLineType::New) {
        StringCchCopy(bang, cchBang, command);
        popup = new Popup(title, prefix[0] == L'\0' ? parentPrefix : prefix);
        return LoadPopup(f, PopupLevel::New, popup, nullptr, 0, *prefix ? parentPrefix : prefix);
      } else {
        gModule.log->Warning(L"Invalid popup line at the root level: %ls", line);
        popup = nullptr;
        return true;
      }
    } else switch (type) {
    case PopupLineType::Folder: {
        //Popup *folder = new Popup(title, *prefix ? parentPrefix : prefix);
        //LoadPopup(f, PopupLevel::Folder, folder, nullptr, 0, *prefix ? parentPrefix : prefix);
        //popup->AddItem(new nPopup::FolderItem(popup, title, popup, icon));
      }
      break;

    case PopupLineType::EndFolder:
      if (level == PopupLevel::Folder) {
        return true;
      } else {
        gModule.log->Warning(L"Unexpected ~Folder: %ls", line);
      }
      break;

    case PopupLineType::EndNew:
      if (level == PopupLevel::New) {
        return true;
      } else {
        gModule.log->Warning(L"Unexpected ~New: %ls", line);
      }
      break;

    case PopupLineType::Command:
      //popup->AddItem(new CommandItem(popup, title, command, icon));
      break;

    case PopupLineType::Content:
      //popup->AddItem(new nPopup::FolderItem(popup, title, new ContentPopup(source, title, command, prefix[0] == L'\0' ? parentPrefix : prefix), icon));
      break;

    case PopupLineType::ContentPath:
      //popup->AddItem(new nPopup::FolderItem(popup, title, new ContentPopup(command, false, title, L"", prefix[0] == L'\0' ? parentPrefix : prefix), icon));
      break;

    case PopupLineType::ContentPathDynamic:
      //popup->AddItem(new nPopup::FolderItem(popup, title, new ContentPopup(command, true, title, L"", prefix[0] == L'\0' ? parentPrefix : prefix), icon));
      break;

    case PopupLineType::Info:
      //popup->AddItem(new InfoItem(popup, title, icon));
      break;

    case PopupLineType::Separator:
      //popup->AddItem(new SeparatorItem(popup));
      break;

    case PopupLineType::Container:
      //popup->AddItem(new ContainerItem(popup, prefix));
      break;

    case PopupLineType::New:
      gModule.log->Warning(L"Unexpected New: %ls", line);
      break;

    case PopupLineType::Invalid:
      gModule.log->Warning(L"Invalid popup line: %ls", line);
      break;

    default: // Failure
      gModule.log->Warning(L"Unrecougnized popup line: %ls", line);
      break;
    }
  }

  return false;
}


/// <summary>
/// Extracts information from a *Popup line.
/// </summary>
/// <returns>The type of *Popup line this is.</returns>
static PopupLineType ParseLine(LPCTSTR line, LPTSTR title, UINT cchTitle, LPTSTR command,
    UINT cchCommand, LPTSTR icon, UINT cchIcon, LPTSTR prefix, UINT cchPrefix) {
  wchar_t token[MAX_LINE_LENGTH];
  GetToken(line, nullptr, &line, FALSE); // Drop *Popup

  // The first token will be ~Folder, ~New, !Separator, !Info, !Container, .icon=, or a title.
  GetToken(line, token, &line, FALSE); // Drop *Popup
  if (_wcsicmp(token, L"~New") == 0) {
    return PopupLineType::EndNew;
  } else if (_wcsicmp(token, L"~Folder") == 0) {
    return PopupLineType::EndFolder;
  } else if (_wcsicmp(token, L"!Separator") == 0) {
    return PopupLineType::Separator;
  } else if (_wcsicmp(token, L"!Container") == 0) {
    // The next token should be a prefix.
    if (GetToken(line, token, &line, FALSE) == FALSE) {
      return PopupLineType::Invalid;
    }
    StringCchCopy(prefix, cchPrefix, token);
    return PopupLineType::Container;
  } else {
    // If we have a .icon, copy it over and move forward.
    if (_wcsnicmp(token, L".icon=", 6) == 0) {
      StringCchCopy(icon, cchIcon, token+6);
      if (GetToken(line, token, &line, FALSE) == FALSE) {
        return PopupLineType::Invalid; // Ending after .icon=
      }
    } else {
      *icon = L'\0';
    }

    if (_wcsicmp(token, L"!Info") == 0) {
      if (GetToken(line, token, &line, FALSE) != FALSE) {
        StringCchCopy(title, cchTitle, token);
      } else {
        *title = L'\0';
      }
      return PopupLineType::Info;
    }

    StringCchCopy(title, cchTitle, token);
    // The token after the title is either !New, Folder, or a command.

    // Store a pointer to here, if this turns out to be a command
    LPCTSTR commandPointer = line;

    // This would be an empty command, or something, might as well mark it invalid.
    if (GetToken(line, token, &line, FALSE) == FALSE) {
      return PopupLineType::Invalid;
    }

    //
    PopupLineType type;

    if (_wcsicmp(token, L"!New") == 0) {
      // !New is followed by the bang command
      if (GetToken(line, token, &line, FALSE) == FALSE) {
        return PopupLineType::Invalid;
      }
      StringCchCopy(command, cchCommand, token);
      type = PopupLineType::New;
    } else if (_wcsicmp(token, L"Folder") == 0){
      type = PopupLineType::Folder;
    } else if (_wcsicmp(token, L"!PopupAdminTools") == 0) {
      //source = ContentPopup::ContentSource::ADMIN_TOOLS;
      type = PopupLineType::Content;
    } else if (_wcsicmp(token, L"!PopupControlPanel") == 0) {
      //source = ContentPopup::ContentSource::CONTROL_PANEL;
      type = PopupLineType::Content;
    } else if (_wcsicmp(token, L"!PopupMyComputer") == 0) {
      //source = ContentPopup::ContentSource::MY_COMPUTER;
      type = PopupLineType::Content;
    } else if (_wcsicmp(token, L"!PopupNetwork") == 0) {
      //source = ContentPopup::ContentSource::NETWORK;
      type = PopupLineType::Content;
    } else if (_wcsicmp(token, L"!PopupPrinters") == 0) {
      //source = ContentPopup::ContentSource::PRINTERS;
      type = PopupLineType::Content;
    } else if (_wcsicmp(token, L"!PopupPrograms") == 0) {
      //source = ContentPopup::ContentSource::PROGRAMS;
      type = PopupLineType::Content;
    } else if (_wcsicmp(token, L"!PopupRecentDocuments") == 0) {
      //source = ContentPopup::ContentSource::RECENT_DOCUMENTS;
      type = PopupLineType::Content;
    } else if (_wcsicmp(token, L"!PopupRecycleBin") == 0) {
      //source = ContentPopup::ContentSource::RECYCLE_BIN;
      type = PopupLineType::Content;
    } else if (_wcsicmp(token, L"!PopupStartMenu") == 0) {
      //source = ContentPopup::ContentSource::START_MENU;
      type = PopupLineType::Content;
    } else if (_wcsnicmp(token, L"!PopupFolder:", _countof("!PopupFolder:") - 1) == 0) {
      //source = ContentPopup::ContentSource::PATH;
      StringCchCopy(command, cchCommand, commandPointer + _countof("!PopupFolder:"));
      command[wcslen(command)-1] = L'\0';
      type = PopupLineType::ContentPath;
    } else if (
        _wcsnicmp(token, L"!PopupDynamicFolder:", _countof("!PopupDynamicFolder:") - 1) == 0) {
      //source = ContentPopup::ContentSource::PATH;
      StringCchCopy(command, cchCommand, commandPointer + _countof("!PopupDynamicFolder:"));
      command[wcslen(command)-1] = L'\0';
      type = PopupLineType::ContentPathDynamic;
    } else {
      StringCchCopy(command, cchCommand, commandPointer);
      type = PopupLineType::Command;
    }

    // Everything, save commands, may be followed by a prefix.
    if (type != PopupLineType::Command) {
      if (GetToken(line, token, &line, FALSE) != FALSE) {
        StringCchCopy(prefix, cchPrefix, token);
      } else {
        prefix[0] = L'\0';
      }
    }

    return type;
  }
}


LRESULT WINAPI MessageHandlerProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CREATE:
    SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    LoadPopups();
    return 0;

  case WM_DESTROY:
    DestroyPopups();
    SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, WPARAM(window), LPARAM(sLsMessages));
    return 0;

  case LM_GETREVID:
    return HandleGetRevId(gModule.name, gModule.version, lParam);

  case LM_REFRESH:
    DestroyPopups();
    LoadPopups();
    return 0;
  }

  return DefWindowProc(window, message, wParam, lParam);
}
