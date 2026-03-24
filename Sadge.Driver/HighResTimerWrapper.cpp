#include "HighResTimerWrapper.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <timeapi.h>
#undef ERROR
#undef DrawText
#undef LoadImage

HighResTimerWrapper::HighResTimerWrapper()
{
  timeBeginPeriod(1);
}

HighResTimerWrapper::~HighResTimerWrapper()
{
  timeEndPeriod(1);
}