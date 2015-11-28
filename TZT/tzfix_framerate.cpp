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
#include "TZT.h"

class tzfixcfg_Framerate
{
public:
  tzfixcfg_Framerate (void);

  bool setup_ui  (HWND hDlg);

  int  poll_prerender_limit (HWND hDlg);

  //protected:
  tzt::ParameterInt*   fps;
  tzt::ParameterBool*  auto_adjust;

  tzt::ParameterBool*  yield_processor;
  tzt::ParameterBool*  allow_fake_sleep;
  tzt::ParameterBool*  minimize_latency;

  tzt::ParameterInt*   backbuffer_count;
  tzt::ParameterInt*   prerender_limit;

  //private:
  HWND hWndFPS;
  HWND hWndTiming;
  HWND hWndYieldProcessor;
  HWND hWndAllowFakeSleep;
  HWND hWndMinimizeLatency;
  HWND hWndTripleBuffering;
  HWND hWndPreRenderLimit;
} *framerate = nullptr;

tzfixcfg_Framerate::tzfixcfg_Framerate (void)
{
  tzt::INI::File* tzfix_ini = config.get_file_tzfix ();
  tzt::INI::File* d3d9_ini  = config.get_file_d3d9  ();

  allow_fake_sleep = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Allow Fake Sleep"
    )
  );
  allow_fake_sleep->register_to_ini ( tzfix_ini,
    L"TZFIX.FrameRate",
      L"AllowFakeSleep" );
  yield_processor = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Yield Processor (HT Optimization)"
    )
  );
  yield_processor->register_to_ini ( tzfix_ini,
    L"TZFIX.FrameRate",
      L"YieldProcessor" );

  minimize_latency = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Minimize Input Latency"
      )
    );
  minimize_latency->register_to_ini ( tzfix_ini,
    L"TZFIX.FrameRate",
      L"MinimizeLatency" );

  fps = static_cast <tzt::ParameterInt *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"Target Framerate"
    )
  );
  fps->register_to_ini (tzfix_ini,
    L"TZFIX.FrameRate",
      L"Target" );

  auto_adjust = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"Auto Adjust TickScale"
    )
  );
  auto_adjust->register_to_ini (tzfix_ini,
    L"TZFIX.FrameRate",
      L"AutoAdjust" );

  backbuffer_count = static_cast <tzt::ParameterInt *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"Backbuffer Count"
    )
  );
  backbuffer_count->register_to_ini (d3d9_ini,
    L"Render.D3D9",
      L"BackBufferCount" );

  prerender_limit = static_cast <tzt::ParameterInt *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"Pre-Rendered Frame Limit"
    )
  );
  prerender_limit->register_to_ini (d3d9_ini,
    L"Render.D3D9",
      L"PreRenderLimit" );

  fps->load            ();
  auto_adjust->load    ();

  yield_processor->load  ();
  allow_fake_sleep->load ();
  minimize_latency->load ();

  backbuffer_count->load ();
  prerender_limit->load  ();
}

bool
tzfixcfg_Framerate::setup_ui (HWND hDlg)
{
  hWndFPS              = GetDlgItem (hDlg, IDC_TARGET_FPS);
  hWndTiming           = GetDlgItem (hDlg, IDC_TARGET_METHOD);
  hWndYieldProcessor   = GetDlgItem (hDlg, IDC_TZFIX_FRAMERATE_YIELD);
  hWndAllowFakeSleep   = GetDlgItem (hDlg, IDC_TZFIX_FRAMERATE_FAKESLEEP);
  hWndMinimizeLatency  = GetDlgItem (hDlg, IDC_TZFIX_FRAMERATE_MIN_LATENCY);
  hWndTripleBuffering  = GetDlgItem (hDlg, IDC_TZFIX_TRIPLE_BUFFERING);
  hWndPreRenderLimit   = GetDlgItem (hDlg, IDC_TZFIX_PRERENDER_LIMIT);

  ComboBox_ResetContent (hWndFPS);

  ComboBox_InsertString (hWndFPS, 0, L"60 FPS");
  ComboBox_InsertString (hWndFPS, 1, L"30 FPS");
  ComboBox_InsertString (hWndFPS, 2, L"20 FPS");
  ComboBox_InsertString (hWndFPS, 3, L"15 FPS");
  ComboBox_InsertString (hWndFPS, 4, L"12 FPS");
  ComboBox_InsertString (hWndFPS, 5, L"10 FPS");

  ComboBox_SetCurSel    (hWndFPS, (fps->get_value () / 60)-1);

  ComboBox_ResetContent (hWndTiming);

  ComboBox_InsertString (hWndTiming, 0, L"Adaptive (Auto-Adjust)");
  ComboBox_InsertString (hWndTiming, 1, L"Fixed-Tick");

  ComboBox_SetCurSel    (hWndTiming, (auto_adjust->get_value () == true ? 0 : 1));

  yield_processor->bind_to_control (new tzt::UI::CheckBox (hWndYieldProcessor));
  yield_processor->set_value       (yield_processor->get_value ());
  allow_fake_sleep->bind_to_control (new tzt::UI::CheckBox (hWndAllowFakeSleep));
  allow_fake_sleep->set_value       (allow_fake_sleep->get_value ());
  minimize_latency->bind_to_control (new tzt::UI::CheckBox (hWndMinimizeLatency));
  minimize_latency->set_value       (minimize_latency->get_value ());

  ComboBox_ResetContent (hWndPreRenderLimit);

  ComboBox_InsertString (hWndPreRenderLimit, 0, L"Driver Preference");
  ComboBox_InsertString (hWndPreRenderLimit, 1, L"1");
  ComboBox_InsertString (hWndPreRenderLimit, 2, L"2");
  ComboBox_InsertString (hWndPreRenderLimit, 3, L"3");
  ComboBox_InsertString (hWndPreRenderLimit, 4, L"4");
  ComboBox_InsertString (hWndPreRenderLimit, 5, L"5");
  ComboBox_InsertString (hWndPreRenderLimit, 6, L"6");

  int prerender = prerender_limit->get_value ();

  if (prerender == 1)
    ComboBox_SetCurSel (hWndPreRenderLimit, 1);
  else if (prerender == 2)
    ComboBox_SetCurSel (hWndPreRenderLimit, 2);
  else if (prerender == 3)
    ComboBox_SetCurSel (hWndPreRenderLimit, 3);
  else if (prerender == 4)
    ComboBox_SetCurSel (hWndPreRenderLimit, 4);
  else if (prerender == 5)
    ComboBox_SetCurSel (hWndPreRenderLimit, 5);
  else if (prerender == 6)
    ComboBox_SetCurSel (hWndPreRenderLimit, 6);
  else /*if (prerender == -1)*/
    ComboBox_SetCurSel (hWndPreRenderLimit, 0);

  bool check = backbuffer_count->get_value () == 2;
  Button_SetCheck (framerate->hWndTripleBuffering, check);

  return true;
}

int
tzfixcfg_Framerate::poll_prerender_limit (HWND hDlg)
{
  int sel = ComboBox_GetCurSel (hWndPreRenderLimit);

  if (sel == 0)
    return -1;
  else if (sel == 1)
    return 1;
  else if (sel == 2)
    return 2;
  else if (sel == 3)
    return 3;
  else if (sel == 4)
    return 4;
  else if (sel == 5)
    return 5;
  else if (sel == 6)
    return 6;
  else /*if (sel == 0)*/
    return -1;
}

INT_PTR
CALLBACK
FramerateConfig (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_INITDIALOG:
  {
    if (framerate == nullptr) {
      framerate = new tzfixcfg_Framerate ();
    }

    framerate->setup_ui (hDlg);
  } break;

  case WM_COMMAND:
  {
    if (LOWORD (wParam) == IDOK)
    {
      BOOL check;

      check = Button_GetCheck (framerate->hWndYieldProcessor);
      framerate->yield_processor->set_value (check);

      check = Button_GetCheck (framerate->hWndAllowFakeSleep);
      framerate->allow_fake_sleep->set_value (check);

      check = Button_GetCheck (framerate->hWndMinimizeLatency);
      framerate->minimize_latency->set_value (check);

      check = Button_GetCheck (framerate->hWndTripleBuffering);
      if (check) {
        framerate->backbuffer_count->set_value (2);
      } else {
        framerate->backbuffer_count->set_value (-1);
      }

      framerate->prerender_limit->set_value (
        framerate->poll_prerender_limit (hDlg)
      );

      framerate->fps->set_value (
        (ComboBox_GetCurSel (framerate->hWndFPS) + 1) * 60
      );
      framerate->fps->store ();

      framerate->auto_adjust->set_value (
        (ComboBox_GetCurSel (framerate->hWndTiming) == 0)
      );
      framerate->auto_adjust->store ();

      framerate->yield_processor->store  ();
      framerate->allow_fake_sleep->store ();
      framerate->minimize_latency->store ();

      framerate->backbuffer_count->store ();
      framerate->prerender_limit->store  ();

      EndDialog (hDlg, LOWORD (wParam));
      return (INT_PTR)TRUE;
    }
  } break;
  }

  return (INT_PTR)FALSE;
}