/**
 * This file is par§t of Tales of Zestiria Tweak.
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

#include "Resource.h"
#include <Windows.h>
#include <windowsx.h>

#include "parameter.h"
#include "utility.h"
#include "TZT.h"

class tzfixcfg_OSD
{
public:
  tzfixcfg_OSD (void);

  bool setup_ui  (HWND hDlg);

//protected:
  tzt::ParameterInt*     stealth_mode;
  tzt::ParameterInt*     custom_d3d;
  tzt::ParameterStringW* delay_triggers;
  tzt::ParameterInt*     delay_ms;

//private:
  HWND hWndInstall;
  HWND hWndStealth;
  HWND hWndCustomD3D;
  HWND hWndUseDelay;
  HWND hWndDelayLabel;
  HWND hWndDelayTime;
  HWND hWndColor;
  HWND hWndColorLabel;
  HWND hWndRTSSGroup;
} *osd = nullptr;

static char default_rtss_profile [] = {
  "[OSD]\n"
  "EnableOSD=1\n"
  "EnableBgnd=1\n"
  "EnableStat=0\n"
  "BaseColor=0080EE11\n"
  "BgndColor=00000000\n"
  "PositionX=1\n"
  "PositionY=1\n"
  "ZoomRatio=2\n"
  "CoordinateSpace=0\n"
  "EnableFrameColorBar=0\n"
  "FrameColorBarsNum=1\n"
  "\n"
  "[Framerate]\n"
  "Limit=0\n"
  "\n"
  "[Hooking]\n"
  "EnableHooking=1\n"
  "EnableFloatingInjectionAddress=1\n"
  "EnableDynamicOffsetDetection=1\n"
  "HookLoadLibrary=0\n"
  "HookDirectDraw=0\n"
  "HookDirect3D8=0\n"
  "HookDirect3D9=1\n"
  "HookDirect3DSwapChain9Present=0\n"
  "HookDirect3DSwapChain9Present64=0\n"
  "HookDXGI=0\n"
  "HookOpenGL=0\n"
  "InjectionDelay=10000\n"
  "InjectionDelayTriggers=tzfix.dll\n"
  "\n"
  "[Font]\n"
  "Weight=700\n"
  "Face=Consolas\n"
  "Load=\n"
  "\n"
  "[RendererDirect3D8]\n"
  "Implementation=1\n"
  "\n"
  "[RendererDirect3D9]\n"
  "Implementation=1\n"
  "\n"
  "[RendererDirect3D10]\n"
  "Implementation=1\n"
  "\n"
  "[RendererDirect3D11]\n"
  "Implementation=1\n"
  "\n"
  "[RendererOpenGL]\n"
  "Implementation=1\n"
  "\n"
  "[Info]\n"
  "Timestamp=14-12-2015, 16:56:34\n"
};

tzfixcfg_OSD::tzfixcfg_OSD (void)
{
  tzt::INI::File* tzfix_ini = config.get_file_tzfix ();
  tzt::INI::File* d3d9_ini  = config.get_file_d3d9  ();
  tzt::INI::File* rtss_ini  = config.get_file_rtss  ();

  if (rtss_ini->get_sections ().empty ()) {
    std::wstring rtss_path = TZT_GetRTSSInstallDir ();
    rtss_path += L"Profiles\\Tales of Zestiria.exe.cfg";

    char szRTSSProfile [MAX_PATH];
    sprintf (szRTSSProfile, "%ws", rtss_path);

    FILE* fProfile = fopen (szRTSSProfile, "w");
    if (fProfile != nullptr) {
      fputs  (default_rtss_profile, fProfile);
      fclose (fProfile);
    }

    config.reload_rtss ();
  }

  stealth_mode = static_cast <tzt::ParameterInt *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"RTSS Stealth Injection"
    )
  );
  stealth_mode->register_to_ini ( rtss_ini,
                                    L"Hooking",
                                      L"EnableFloatingInjectionAddress" );
  stealth_mode->load ();

  custom_d3d = static_cast <tzt::ParameterInt *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"RTSS Custom D3D"
    )
  );
  custom_d3d->register_to_ini ( rtss_ini,
                                  L"Hooking",
                                    L"EnableDynamicOffsetDetection" );
  custom_d3d->load ();

  delay_triggers = static_cast <tzt::ParameterStringW *> (
    tzt::g_ParameterFactory.create_parameter <std::wstring> (
      L"RTSS Delay Triggers"
    )
  );
  delay_triggers->register_to_ini ( rtss_ini,
                                     L"Hooking",
                                       L"InjectionDelayTriggers" );
  delay_triggers->load ();

  delay_ms = static_cast <tzt::ParameterInt *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"RTSS Delay MS"
    )
  );
  delay_ms->register_to_ini ( rtss_ini,
                                L"Hooking",
                                  L"InjectionDelay" );
  delay_ms->load ();
}

bool
tzfixcfg_OSD::setup_ui (HWND hDlg)
{
  bool installed = wcslen (TZT_GetRTSSInstallDir ().c_str ()) > 0;

  hWndInstall    = GetDlgItem (hDlg, IDC_RTSS_INSTALL);

  hWndStealth    = GetDlgItem (hDlg, IDC_RTSS_STEALTH);
  hWndCustomD3D  = GetDlgItem (hDlg, IDC_RTSS_CUSTOM3D);
  hWndUseDelay   = GetDlgItem (hDlg, IDC_RTSS_DELAY);
  hWndDelayLabel = GetDlgItem (hDlg, IDC_RTSS_DELAY_LABEL);
  hWndDelayTime  = GetDlgItem (hDlg, IDC_RTSS_DELAY_TIME);

  hWndColorLabel = GetDlgItem (hDlg, IDC_RTSS_COLOR_LABEL);
  hWndColor      = GetDlgItem (hDlg, IDC_RTSS_COLOR);

  ShowWindow (hWndColorLabel, SW_HIDE);
  ShowWindow (hWndColor,      SW_HIDE);

  hWndRTSSGroup  = GetDlgItem (hDlg, IDC_RTSS_GROUP);

  Button_Enable (hWndInstall, (! installed));

  Button_Enable (hWndStealth,   installed);
  Button_Enable (hWndCustomD3D, installed);
  Button_Enable (hWndUseDelay,  installed);
  Button_Enable (hWndColor,     installed);

  if (installed && delay_triggers->get_value ().length ()) {
    EnableWindow    (hWndDelayLabel, installed);
    EnableWindow    (hWndDelayTime,  installed);
    Button_SetCheck (hWndUseDelay,   true);
  } else {
    EnableWindow    (hWndDelayLabel, false);
    EnableWindow    (hWndDelayTime,  false);
    Button_SetCheck (hWndUseDelay,   false);
  }

  EnableWindow (hWndColorLabel, installed);
  EnableWindow (hWndRTSSGroup,  installed);

  stealth_mode->bind_to_control (new tzt::UI::CheckBox (hWndStealth));
  stealth_mode->set_value       (stealth_mode->get_value ());

  custom_d3d->bind_to_control (new tzt::UI::CheckBox (hWndCustomD3D));
  custom_d3d->set_value       (custom_d3d->get_value ());

  delay_ms->bind_to_control (new tzt::UI::EditBox (hWndDelayTime));
  delay_ms->set_value       (delay_ms->get_value () / 1000);

  return true;
}

INT_PTR
CALLBACK
OSDConfig (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG:
    {
      if (osd == nullptr) {
        osd = new tzfixcfg_OSD ();
      }

      osd->setup_ui (hDlg);
    } break;

    case WM_COMMAND:
    {
      if (LOWORD (wParam) == IDC_RTSS_INSTALL)
      {
        ShellExecute ( hDlg,
                         L"OPEN",
                           L"http://www.guru3d.com/files-details/rtss-rivatuner-statistics-server-download.html",
                             NULL,
                               NULL,
                                 SW_SHOWNORMAL );
        return (INT_PTR)TRUE;
      }
      if (LOWORD (wParam) == IDC_RTSS_DELAY)
      {
        bool delay = Button_GetCheck (osd->hWndUseDelay);

        EnableWindow (osd->hWndDelayLabel, delay);
        EnableWindow (osd->hWndDelayTime,  delay);

        if (delay)
          osd->delay_triggers->set_value (L"tzfix.dll");
        else
          osd->delay_triggers->set_value (L"");
      }
      if (LOWORD (wParam) == IDOK)
      {
        osd->custom_d3d->store   ();
        osd->stealth_mode->store ();

        wchar_t wszDelayMS [16];
        Edit_GetText (osd->hWndDelayTime, wszDelayMS, 15);

        osd->delay_ms->set_value_str (wszDelayMS);
        osd->delay_ms->set_value (osd->delay_ms->get_value () * 1000);

        osd->delay_ms->store       ();
        osd->delay_triggers->store ();

        EndDialog (hDlg, LOWORD (wParam));
        return (INT_PTR)TRUE;
      }
    } break;
  }

  return (INT_PTR)FALSE;
}