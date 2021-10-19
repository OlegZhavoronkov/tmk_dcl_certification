//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#include "namingSystems.h"

std::string TwoLettersNamer::getName() {
  if (m_high > 'Z')
    return "---";
  std::string toReturn(2, m_low);
  if (m_high == 'A' - 1)
    // toReturn=QString(low);
    toReturn.resize(1);
  else {
    toReturn[0] = m_high;
  }

  m_low++;
  if (m_low > 'Z') {
    m_low = 'A';
    m_high++;
  }
  return m_preffix + toReturn;
}

bool TwoLettersNamer::setName(const std::string &name) {
  switch (name.length()) {
  case 1: {
    char l = name[0];
    if ('A' <= l && l <= 'Z') {
      m_low = l;
      m_high = 'A' - 1;
      return true;
    }
    return false;
  }
  case 2: {
    char h = name[0];
    char l = name[1];
    if ('A' <= l && l <= 'Z' && 'A' <= h && h <= 'Z') {
      m_low = l;
      m_high = h;
      return true;
    }
    return false;
  }
  default:
    return false;
  }
}

void TwoLettersNamer::reset() {
  m_low = 'A';
  m_high = 'A' - 1;
}

bool TwoLettersNamer::isOverfulled() { return m_low >= 'Z' && m_high >= 'Z'; }
