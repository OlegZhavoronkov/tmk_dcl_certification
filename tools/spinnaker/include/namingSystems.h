//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#ifndef NAMING_SYSTEMS_H
#define NAMING_SYSTEMS_H

#include <string>

struct NamingSysytem {
  virtual ~NamingSysytem() {}
  virtual std::string getName() = 0;
  virtual bool setName(const std::string &name) = 0;
  virtual void reset() = 0;
  virtual bool isOverfulled() = 0;
};

class TwoLettersNamer : public NamingSysytem {
public:
  TwoLettersNamer(const std::string &prefix)
      : m_low('A'), m_high('A' - 1), m_preffix(prefix) {}
  TwoLettersNamer() : TwoLettersNamer("SP") {}
  virtual ~TwoLettersNamer() {}
  std::string getName() override;
  bool setName(const std::string &name) override;
  void reset() override;
  bool isOverfulled() override;

private:
  char m_low;
  char m_high;
  std::string m_preffix;
};

#endif // NAMING_SYSTEMS_H
