#pragma once

template <typename Type>
class Optional {
  Type value;
  bool present;
};