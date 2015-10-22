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

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "parameter.h"

std::wstring
tzt::ParameterInt::get_value_str (void)
{
  wchar_t str [32];
  _itow (value, str, 10);

  return std::wstring (str);
}

int
tzt::ParameterInt::get_value (void)
{
  return value;
}

void
tzt::ParameterInt::set_value (int val)
{
  value = val;

  if (ui_control != nullptr)
    ui_control->set_value_str (get_value_str ());
}


void
tzt::ParameterInt::set_value_str (std::wstring str)
{
  value = _wtoi (str.c_str ());

  if (ui_control != nullptr) {
    ui_control->set_value_str (get_value_str ());
  }
}


std::wstring
tzt::ParameterInt64::get_value_str (void)
{
  wchar_t str [32];
  _i64tow (value, str, 10);

  return std::wstring (str);
}

int64_t
tzt::ParameterInt64::get_value (void)
{
  return value;
}

void
tzt::ParameterInt64::set_value (int64_t val)
{
  value = val;

  if (ui_control != nullptr)
    ui_control->set_value_str (get_value_str ());
}


void
tzt::ParameterInt64::set_value_str (std::wstring str)
{
  value = _wtol (str.c_str ());

  if (ui_control != nullptr) {
    ui_control->set_value_str (get_value_str ());
  }
}


std::wstring
tzt::ParameterBool::get_value_str (void)
{
  if (value == true)
    return L"true";

  return L"false";
}

bool
tzt::ParameterBool::get_value (void)
{
  return value;
}

void
tzt::ParameterBool::set_value (bool val)
{
  value = val;

  if (ui_control != nullptr)
    ui_control->set_value_str (get_value_str ());
}


void
tzt::ParameterBool::set_value_str (std::wstring str)
{
  if (str.length () == 1 &&
      str [0] == L'1')
    value = true;

  else if (str.length () == 4 &&
      towlower (str [0]) == L't' &&
      towlower (str [1]) == L'r' &&
      towlower (str [2]) == L'u' &&
      towlower (str [3]) == L'e')
    value = true;

  else
    value = false;

  if (ui_control != nullptr) {
    ui_control->set_value_str (get_value_str ());
  }
}


std::wstring
tzt::ParameterFloat::get_value_str (void)
{
  wchar_t val_str [16];
  swprintf (val_str, L"%f", value);

  return std::wstring (val_str);
}

float
tzt::ParameterFloat::get_value (void)
{
  return value;
}

void
tzt::ParameterFloat::set_value (float val)
{
  value = val;

  if (ui_control != nullptr)
    ui_control->set_value_str (get_value_str ());
}


void
tzt::ParameterFloat::set_value_str (std::wstring str)
{
  value = (float)wcstod (str.c_str (), NULL);

  if (ui_control != nullptr) {
    ui_control->set_value_str (get_value_str ());
  }
}

#include <Windows.h>
#include <windowsx.h>

std::wstring
tzt::UI::EditBox::get_value_str (void)
{
  wchar_t val_str [32];
  Edit_GetText (handle, val_str, 32);
  return std::wstring (val_str);
}

void
tzt::UI::EditBox::set_value_str (std::wstring str)
{
  Edit_SetText (handle, str.c_str ());
}

std::wstring
tzt::UI::CheckBox::get_value_str (void)
{
  if (Button_GetCheck (handle)) {
    if (numeric)
      return L"1";

    return L"true";
  }
  else {
    if (numeric)
      return L"0";

    return L"false";
  }
}

void
tzt::UI::CheckBox::set_value_str (std::wstring str)
{
  if (str.length () == 1 &&
      str [0] == L'1') {
    Button_SetCheck (handle, true);
    numeric = true;
  }
  else if (str.length () == 1 &&
           str [0] == L'0') {
    Button_SetCheck (handle, false);
    numeric = true;
  }

  else if (str.length () == 4 &&
      towlower (str [0]) == L't' &&
      towlower (str [1]) == L'r' &&
      towlower (str [2]) == L'u' &&
      towlower (str [3]) == L'e') {
    Button_SetCheck (handle, true);
    numeric = false;
  }
  else {
    Button_SetCheck (handle, false);
    numeric = false;
  }
}


std::wstring
tzt::ParameterStringW::get_value_str (void)
{
  return value;
}

std::wstring
tzt::ParameterStringW::get_value (void)
{
  return value;
}

void
tzt::ParameterStringW::set_value (std::wstring val)
{
  value = val;
}


void
tzt::ParameterStringW::set_value_str (std::wstring str)
{
  value = str;
}



template <>
tzt::iParameter*
tzt::ParameterFactory::create_parameter <int> (const wchar_t* name)
{
  iParameter* param = new ParameterInt ();
  params.push_back (param);

  return param;
}

template <>
tzt::iParameter*
tzt::ParameterFactory::create_parameter <int64_t> (const wchar_t* name)
{
  iParameter* param = new ParameterInt64 ();
  params.push_back (param);

  return param;
}

template <>
tzt::iParameter*
tzt::ParameterFactory::create_parameter <bool> (const wchar_t* name)
{
  iParameter* param = new ParameterBool ();
  params.push_back (param);

  return param;
}

template <>
tzt::iParameter*
tzt::ParameterFactory::create_parameter <float> (const wchar_t* name)
{
  iParameter* param = new ParameterFloat ();
  params.push_back (param);

  return param;
}

template <>
tzt::iParameter*
tzt::ParameterFactory::create_parameter <std::wstring> (const wchar_t* name)
{
  iParameter* param = new ParameterStringW ();
  params.push_back (param);

  return param;
}




tzt::ParameterFactory tzt::g_ParameterFactory;

tzt::ParameterInt*     refresh_rate;
tzt::ParameterInt*     res_x;
tzt::ParameterInt*     res_y;

tzt::ParameterBool*    use_vsync;

tzt::ParameterInt*     anisotropy;
tzt::ParameterInt*     texture_res;

tzt::ParameterStringW* level_of_detail;
tzt::ParameterStringW* shadow_quality;
tzt::ParameterStringW* antialiasing;
tzt::ParameterStringW* draw_distance;

//tzt::ParameterInt*   framerate_limiting;
//tzt::ParameterInt*   max_fps;

tzt::ParameterBool*    decline_backup;