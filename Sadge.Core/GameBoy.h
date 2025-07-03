#pragma once

#include "Cpu.h"

class GameBoy
{
public:
  GameBoy() : m_cpu(true) {};
  ~GameBoy() {};

  Status InsertRom(const std::filesystem::path &rom_path);
  
  void InitScreen()
  {
    m_cpu.InitScreen();
  }

  inline void FullScreen(bool fullscreen)
  {
    m_cpu.FullScreen(fullscreen);
  }

  inline void* GetHandle()
  {
    return m_cpu.GetWindowHandle();
  }

  inline void TurnOn()
  {
    m_cpu.Run();
  }

  inline void TurnOff()
  {
    m_cpu.Stop();
  }

  void CloseScreen()
  {
    m_cpu.CloseWindow();
  }

  inline void SetScale(int scale)
  {
    m_cpu.SetScale(scale);
  }

private:

  Cpu m_cpu;
};