/**
 * This file is part of Tales of Zestiria Tweak.
 *
 * Tales of Zestiria Tweak is free software : you can redistribute it
 * and / or modify it under the terms of the GNU General Public License
 * as published by The Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Tales of Zestiria Tweak is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *     See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tales of Zestiria Tweak.
 *
 *    If no license was present, see <http://www.gnu.org/licenses/>.
**/

#pragma once

#include "resource.h"

#include <string>
#include "cfg.h"

class TO12_Config {
public:
           TO12_Config (void) { cfg = nullptr; };
  virtual ~TO12_Config (void)
  {
    if (cfg != nullptr) {
      delete cfg;
      cfg = nullptr;
    }
  }

  void load (std::wstring path) {
    cfg = new tzt::CFG::File ((wchar_t *)std::wstring (path + std::wstring (L"TOZ.CFG")).c_str ());
  }

  void save (std::wstring path) {
    cfg->write (std::wstring (path + std::wstring (L"TOZ.CFG")).c_str ());
  }

  std::wstring
    lookup_value (std::wstring section_name, std::wstring key_name) {
    tzt::CFG::File::Section& section = cfg->get_section (section_name);
    return section.get_value (key_name);
  }

  void
    set_value (std::wstring section_name, std::wstring key_name, std::wstring value) {
    tzt::CFG::File::Section& section = cfg->get_section (section_name);
    section.get_value (key_name) = value;
  }

  void import (std::wstring imp_data) {
    cfg->import (imp_data);
  }

  tzt::CFG::File* get_file (void) { return cfg; }

private:
  tzt::CFG::File* cfg;
} extern config;