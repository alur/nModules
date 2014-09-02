//-------------------------------------------------------------------------------------------------
// /nTray/Tray.hpp
// The nModules Project
//
// A system tray, contains the tray icons.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "TrayIcon.hpp"
#include "Types.h"

#include "../nShared/Balloon.hpp"
#include "../nShared/LayoutSettings.hpp"
#include "../nShared/Settings.hpp"
#include "../nShared/Tooltip.hpp"
#include "../nShared/Window.hpp"

#include <forward_list>
#include <map>
#include <strsafe.h>

class Tray : public Drawable {
public:
  enum class States {
    Base,
    Count
  };

public:
  explicit Tray(LPCTSTR);
  ~Tray();

public:
  TrayIcon *AddIcon(IconData&);
  void RemoveIcon(TrayIcon*);

  void InitCompleted();
  void ShowTip(LPCWSTR text, LPRECT position);
  void HideTip();
  void EnqueueBalloon(TrayIcon*, LPCWSTR infoTitle, LPCWSTR info, DWORD infoFlags,
    HICON balloonIcon, bool realTime);

  // IMessageHandler
public:
  LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

private:
  // Dismisses a balloon notification prematurely.
  void DismissBalloon(UINT message);

  // Hides the current balloon, and possible shows the next balloon.
  void ShowNextBalloon();

  // Returns true if we should display the given icon.
  bool WantIcon(IconData&);

private:
  enum class OverflowAction {
    None,
    SizeLeft,
    SizeRight,
    SizeUp,
    SizeDown
  };

  // All data required to display a balloon.
  struct BalloonData {
    TrayIcon *icon;
    LPCWSTR infoTitle;
    LPCWSTR info;
    DWORD infoFlags;
    HICON balloonIcon;
  };

  // All data required to determine whether to show or hide a particular icon.
  struct IconId {
    IconId(GUID guid) {
      type = Type::GUID;
      this->guid = guid;
    }
    IconId(LPCWSTR process) {
      type = Type::Process;
      StringCchCopyW(this->process, _countof(this->process), process);
    }

    enum class Type {
      GUID,
      Process
    } type;

    union {
      GUID guid;
      WCHAR process[MAX_PATH];
    };
  };

private:
  void LoadSettings();
  void Relayout();
  vector<TrayIcon*>::iterator FindIcon(TrayIcon*);

private:
  StateRender<States> mStateRender;
  StateRender<TrayIcon::States> mIconStates;

  WindowSettings mIconWindowSettings;

private:
  Balloon mBalloon;
  Tooltip mTooltip;
  vector<TrayIcon*> mIcons;

  // Balloons queued up to be displayed.
  std::list<BalloonData> mQueuedBalloons;

  // The active balloon icon.
  TrayIcon *mActiveBalloonIcon;

  // The size that the tray should be when it's not overflowing
  D2D1_SIZE_F mTargetSize;

  // The position where the tray should be when it's not overflowing
  D2D1_POINT_2F mTargetPosition;

  // Fires when we should hide the current balloon, and possibly show the next one.
  UINT_PTR mBalloonTimer;

  // Standard balloon icons.
  HICON mInfoIcon, mWarningIcon, mErrorIcon;


  // Settings
private:
  std::forward_list<IconId> mIconBlacklist;
  int mIconSize;
  LayoutSettings mIconLayout;
  bool mHideBalloons;
  bool mNoTooltips;
  TCHAR mOnResize[MAX_LINE_LENGTH];
  OverflowAction mOverflowAction;

  // Number of milliseconds to show balloons.
  int mBalloonTime;

  // If true, don't play any notification sounds.
  bool mNoNotificationSounds;

  // The sound to play when showing new notifications.
  WCHAR mNotificationSound[MAX_PATH];
};
