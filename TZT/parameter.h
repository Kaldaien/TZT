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

#ifndef __TZT__PARAMETER_H__
#define __TZT__PARAMETER_H__

#include "cfg.h"

#include <Windows.h>
#include <vector>

namespace tzt {
  namespace UI {

class Control {
  public:
    Control (void) {
      handle = NULL;
    }

    virtual std::wstring get_value_str (void) = 0;
    virtual void         set_value_str (std::wstring val_str) = 0;

    BOOL         exists (void) { return IsWindow (handle); }

  protected:
    HWND    handle;

  private:
};

class EditBox : public Control {
public:
  EditBox (HWND hWnd) {
    handle = hWnd;
  }

  virtual std::wstring get_value_str (void);
  virtual void         set_value_str (std::wstring val_str);
};

class CheckBox : public Control {
public:
  CheckBox (HWND hWnd) {
    handle  = hWnd;
    numeric = false; // When true, input will be 1/0 and output will be 1/0
  }

  virtual std::wstring get_value_str (void);
  virtual void         set_value_str (std::wstring val_str);

private:
  bool numeric;
};

};

class iParameter {
public:
  iParameter (void) {
    cfg        = nullptr;
    ui_control = nullptr;
  }

  virtual std::wstring get_value_str (void) = 0;
  virtual void         set_value_str (std::wstring str) = 0;

  // Read value from CFG
  bool load (void)
  {
    if (cfg != nullptr) {
      CFG::File::Section& section = cfg->get_section (cfg_section);

      if (section.contains_key (cfg_key)) {
        set_value_str (section.get_value (cfg_key));
        return true;
      }
    }

    return false;
  }

  // Store value in CFG
  bool store (void)
  {
    bool ret = false;

    if (ui_control != nullptr)
      set_value_str (ui_control->get_value_str ());

    wcsncpy_s (backing_string, get_value_str ().c_str (), 64);

    if (cfg != nullptr) {
      CFG::File::Section& section = cfg->get_section (cfg_section);

      if (section.contains_key (cfg_key)) {
        section.get_value (cfg_key) = backing_string;
        ret = true;
      }

      // Add this key/value if it doesn't already exist.
      else {
        section.add_key_value (cfg_key, backing_string);
        ret = true;// +1;
      }
    }

    return ret;
  }

  void register_to_cfg (CFG::File* file, std::wstring key)
  {
    cfg         = file;
    cfg_section = L"GLOBAL";
    cfg_key     = key;
  }

  void bind_to_control (UI::Control* ui_ctl)
  {
    ui_control = ui_ctl;
  }

protected:
  UI::Control*             ui_control;
  wchar_t                  backing_string [64]; // Required by XML

private:
  CFG::File*               cfg;
  std::wstring             cfg_section;
  std::wstring             cfg_key;
};

template <typename _T>
class Parameter : public iParameter {
public:
  virtual std::wstring get_value_str (void) = 0;
  virtual _T           get_value     (void) = 0;

  virtual void         set_value     (_T val)           = 0;
  virtual void         set_value_str (std::wstring str) = 0;

protected:
  _T                       value;
};

class ParameterInt : public Parameter <int>
{
public:
  std::wstring get_value_str (void);
  int          get_value     (void);

  void         set_value     (int val);
  void         set_value_str (std::wstring str);

protected:
  int value;
};

class ParameterInt64 : public Parameter <int64_t>
{
public:
  std::wstring get_value_str (void);
  int64_t      get_value (void);

  void         set_value (int64_t val);
  void         set_value_str (std::wstring str);

protected:
  int64_t value;
};

class ParameterBool : public Parameter <bool>
{
public:
  std::wstring get_value_str (void);
  bool         get_value     (void);

  void         set_value     (bool val);
  void         set_value_str (std::wstring str);

protected:
  bool value;
};

class ParameterFloat : public Parameter <float>
{
public:
  std::wstring get_value_str (void);
  float        get_value (void);

  void         set_value (float val);
  void         set_value_str (std::wstring str);

protected:
  float value;
};

class ParameterStringW : public Parameter <std::wstring>
{
public:
  std::wstring get_value_str (void);
  std::wstring get_value     (void);

  void         set_value     (std::wstring str);
  void         set_value_str (std::wstring str);

protected:
  std::wstring value;
};

class ParameterFactory {
public:
  template <typename _T> iParameter* create_parameter  (const wchar_t* name);
protected:
private:
  std::vector <iParameter *> params;
} extern g_ParameterFactory;

}


extern tzt::ParameterInt*     refresh_rate;
extern tzt::ParameterInt*     res_x;
extern tzt::ParameterInt*     res_y;

//extern tzt::ParameterInt* max_fps;

extern tzt::ParameterBool*    use_vsync;

extern tzt::ParameterInt*     anisotropy;
extern tzt::ParameterInt*     texture_res;

extern tzt::ParameterStringW* level_of_detail;
extern tzt::ParameterStringW* shadow_quality;
extern tzt::ParameterStringW* antialiasing;
extern tzt::ParameterStringW* draw_distance;

// This controls the auto-backup behavior (it's another non-game setting)
extern tzt::ParameterBool*    decline_backup;

#endif /* __TZT__PARAMETER_H__ */