#pragma once

#include <chrono>

class StopWatch
{
public:
  StopWatch() {}

  inline void Start()
  {
    if (!m_running)
    {
      m_start_time = std::chrono::high_resolution_clock::now();
      m_running = true;
    }
  }

  inline void Restart()
  {
    m_start_time = std::chrono::high_resolution_clock::now();
    m_running = true;
  }

  inline void Stop()
  {
    if (m_running)
    {
      m_end_time = std::chrono::high_resolution_clock::now();
      m_running = false;
    }
  }

  inline double Elapsed() const
  {
    if (m_running)
    {
      auto duration = std::chrono::high_resolution_clock::now() - m_start_time;
      return std::chrono::duration<double>(duration).count();
    }
    else
    {
      auto duration = m_end_time - m_start_time;
      return std::chrono::duration<double>(duration).count();
    }
  }

private:
  bool m_running = false;
  std::chrono::high_resolution_clock::time_point m_start_time;
  std::chrono::high_resolution_clock::time_point m_end_time;
};
