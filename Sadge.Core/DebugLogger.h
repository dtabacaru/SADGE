#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#define DEBUG_OUTPUT 1

#define LOG_TO_SCREEN 0
#define LOG_TO_FILE 1

class DebugLogger
{
public:
  DebugLogger(const std::string& caller [[maybe_unused]])
  {
#if DEBUG_OUTPUT
    m_caller = caller;
#if LOG_TO_FILE
    auto now = std::chrono::system_clock::now();
    std::string filename = std::format("{:%y%m%d-%H%M%S}.log", now);

    m_log_file_stream.open(filename);
#endif
#endif
  }

#if DEBUG_OUTPUT
  inline void DebugMessage(const std::string& msg)
  {
#if LOG_TO_SCREEN
    std::cout << m_caller << msg << std::endl;
#endif
#if LOG_TO_FILE
    m_log_file_stream << m_caller << msg << std::endl;
#endif
  }
#else
  #define DebugMessage(...)
#endif

#if DEBUG_OUTPUT
  inline void DebugSection(const std::string& header_msg)
  {
#if LOG_TO_SCREEN
    std::cout << std::endl;
    std::cout << SECTION_DELIMETER << std::endl;
    std::cout << ">>> " << header_msg << " <<<" << std::endl;
    std::cout << SECTION_DELIMETER << std::endl;
    std::cout << std::endl;
#endif
#if LOG_TO_FILE
    m_log_file_stream << std::endl;
    m_log_file_stream << SECTION_DELIMETER << std::endl;
    m_log_file_stream << ">>> " << header_msg << " <<<" << std::endl;
    m_log_file_stream << SECTION_DELIMETER << std::endl;
    m_log_file_stream << std::endl;
#endif
  }
#else
  #define DebugSection(...)
#endif

private:
  constexpr static const char* SECTION_DELIMETER = "><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><";

  std::string m_caller;
  std::ofstream m_log_file_stream;
};
