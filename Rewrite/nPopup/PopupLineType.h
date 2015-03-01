#pragma once

/// <summary>
/// The possibly types for a *Popup line.
/// </summary>
enum class PopupLineType {
  EndNew, // ~New
  EndFolder, // ~Folder
  New, // !New
  Folder, // Folder
  Separator, // !Separator
  Info, // !Info
  Command, // Anything
  Content, // Retrieves its items from an external source
  ContentPath, // Retrieves its items from a path
  ContentPathDynamic, // Retrieves its items from a path, reloading the items each time it is opened
  Invalid, // An invalid line
  Container
};