#pragma once

#include <string>

class Status
{
public:
  void SetValid(bool valid) { m_valid = valid; }
  void SetMsg(const std::string&msg) { m_msg = msg; }

  bool Valid() const { return m_valid; }
  std::string Msg() const { return m_msg; }

private:
  bool m_valid = true;
  std::string m_msg = "";
};
