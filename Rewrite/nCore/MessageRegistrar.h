#pragma once

#include "../Headers/Windows.h"

void PostCoreMessage(UINT message, WPARAM wParam, LPARAM lParam);
void SendCoreMessage(UINT message, WPARAM wParam, LPARAM lParam);
