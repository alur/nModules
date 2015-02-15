#pragma once

#include "IDiscardable.hpp"
#include "IPainter.hpp"

class IDiscardablePainter : public IPainter, public IDiscardable {};