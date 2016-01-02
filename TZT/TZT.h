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
#include "ini.h"
#include "utility.h"

class TO12_Config {
public:
           TO12_Config (void) { cfg       = nullptr;
                                tzfix_ini = nullptr;
                                d3d9_ini  = nullptr;
                                rtss_ini  = nullptr;
                              };
  virtual ~TO12_Config (void)
  {
    if (cfg != nullptr) {
      delete cfg;
      cfg = nullptr;
    }

    if (tzfix_ini != nullptr) {
      delete tzfix_ini;
      tzfix_ini = nullptr;
    }

    if (d3d9_ini != nullptr) {
      delete d3d9_ini;
      d3d9_ini = nullptr;
    }

    if (rtss_ini != nullptr) {
      delete rtss_ini;
      rtss_ini = nullptr;
    }
  }


  void load (std::wstring path) {
    cfg =
      new tzt::CFG::File ( (wchar_t *)
              std::wstring (path + std::wstring (L"TOZ.CFG")).c_str () );

    tzfix_ini = new tzt::INI::File (L"tzfix.ini");
    d3d9_ini  = new tzt::INI::File (L"d3d9.ini");

    std::wstring rtss_path = TZT_GetRTSSInstallDir ();
    rtss_path += L"Profiles\\Tales of Zestiria.exe.cfg";

    rtss_ini  = new tzt::INI::File (rtss_path.c_str ());
  }

  void save (std::wstring path) {
    cfg->write (std::wstring (path + std::wstring (L"TOZ.CFG")).c_str ());

    if (! (tzfix_ini->get_sections ().empty () ||
           d3d9_ini->get_sections  ().empty ())) {
      tzfix_ini->write (L"tzfix.ini");
      d3d9_ini->write  (L"d3d9.ini");
    }

    std::wstring rtss_path = TZT_GetRTSSInstallDir ();
    rtss_path += L"Profiles\\Tales of Zestiria.exe.cfg";

    rtss_ini->write  (rtss_path.c_str ());
  }

  void reload_rtss (void) {
    if (rtss_ini != nullptr)
      delete rtss_ini;

    std::wstring rtss_path = TZT_GetRTSSInstallDir ();
    rtss_path += L"Profiles\\Tales of Zestiria.exe.cfg";

    rtss_ini  = new tzt::INI::File (rtss_path.c_str ());
  }


  std::wstring
  lookup_value (std::wstring section_name, std::wstring key_name) {
    tzt::CFG::File::Section& section = cfg->get_section (section_name);
    return section.get_value (key_name);
  }

  std::wstring
  lookup_value_tzfix (std::wstring section_name, std::wstring key_name) {
    tzt::INI::File::Section& section = tzfix_ini->get_section (section_name);
    return section.get_value (key_name);
  }

  std::wstring
  lookup_value_d3d9 (std::wstring section_name, std::wstring key_name) {
    tzt::INI::File::Section& section = d3d9_ini->get_section (section_name);
    return section.get_value (key_name);
  }

  std::wstring
  lookup_value_rtss (std::wstring section_name, std::wstring key_name) {
    tzt::INI::File::Section& section = rtss_ini->get_section (section_name);
    return section.get_value (key_name);
  }


  void
  set_value (std::wstring section_name, std::wstring key_name, std::wstring value) {
    tzt::CFG::File::Section& section = cfg->get_section (section_name);
    section.get_value (key_name) = value;
  }

  void
  set_value_tzfix (std::wstring section_name, std::wstring key_name, std::wstring value) {
    tzt::INI::File::Section& section = tzfix_ini->get_section (section_name);
    section.get_value (key_name) = value;
  }

  void
  set_value_d3d9 (std::wstring section_name, std::wstring key_name, std::wstring value) {
    tzt::INI::File::Section& section = d3d9_ini->get_section (section_name);
    section.get_value (key_name) = value;
  }

  void
  set_value_rtss (std::wstring section_name, std::wstring key_name, std::wstring value) {
    tzt::INI::File::Section& section = rtss_ini->get_section (section_name);
    section.get_value (key_name) = value;
  }


  void import (std::wstring imp_data) {
    cfg->import (imp_data);
  }

  void import_tzfix (std::wstring imp_data) {
    tzfix_ini->import (imp_data);
  }

  void import_d3d9 (std::wstring imp_data) {
    d3d9_ini->import (imp_data);
  }

  void import_rtss (std::wstring imp_data) {
    rtss_ini->import (imp_data);
  }


  tzt::CFG::File* get_file       (void) { return cfg;       }

  tzt::INI::File* get_file_tzfix (void) { return tzfix_ini; }
  tzt::INI::File* get_file_d3d9  (void) { return d3d9_ini;  }
  tzt::INI::File* get_file_rtss  (void) { return rtss_ini;  }


private:
  tzt::CFG::File* cfg;

  tzt::INI::File* tzfix_ini;
  tzt::INI::File* d3d9_ini;
  tzt::INI::File* rtss_ini;
} extern config;