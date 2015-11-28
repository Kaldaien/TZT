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

class tzfixcfg_Graphics
{
public:
  tzfixcfg_Graphics (void);

  bool setup_ui  (HWND hDlg);

//protected:
  tzt::ParameterBool*  aspect_correct_videos;
  tzt::ParameterBool*  aspect_correct_ui;
  tzt::ParameterBool*  disable_scissor;
  tzt::ParameterBool*  complete_mipmaps;

  tzt::ParameterInt*   shadow_scale;
  tzt::ParameterFloat* postprocess_ratio;

//private:
  HWND hWndAspectRatioCorrection;
  HWND hWndDisableScissor;
  HWND hWndPostProcessScale;
  HWND hWndShadowRes;
  HWND hWndDoubleResSorrey;
  HWND hWndForceCompleteMipMaps;
} *graphics = nullptr;

tzfixcfg_Graphics::tzfixcfg_Graphics (void)
{
  tzt::INI::File* tzfix_ini = config.get_file_tzfix ();
  tzt::INI::File* d3d9_ini  = config.get_file_d3d9  ();

  aspect_correct_videos = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Aspect Ratio Correct Bink Videos"
    )
  );
  aspect_correct_videos->register_to_ini ( tzfix_ini,
                                             L"TZFIX.Render",
                                               L"AspectCorrectVideos" );

  aspect_correct_ui = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Aspect Ratio Correct UI"
    )
  );
  aspect_correct_ui->register_to_ini ( tzfix_ini,
                                         L"TZFIX.Render",
                                           L"AspectCorrection" );

  disable_scissor = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Disable Scissor Test"
    )
  );
  disable_scissor->register_to_ini ( tzfix_ini,
                                       L"TZFIX.Render",
                                         L"DisableScissor" );

  complete_mipmaps = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Force Complete Mipmaps"
    )
  );
  complete_mipmaps->register_to_ini ( tzfix_ini,
                                        L"TZFIX.Render",
                                          L"CompleteMipmaps" );

  shadow_scale = static_cast <tzt::ParameterInt *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"Shadow Map Rescale"
    )
  );
  shadow_scale->register_to_ini ( tzfix_ini,
                                    L"TZFIX.Render",
                                      L"RescaleShadows" );

  postprocess_ratio = static_cast <tzt::ParameterFloat *> (
    tzt::g_ParameterFactory.create_parameter <float> (
      L"Post-Processing Ratio"
    )
  );
  postprocess_ratio->register_to_ini ( tzfix_ini,
                                         L"TZFIX.Render",
                                           L"PostProcessRatio" );

  aspect_correct_videos->load ();
  aspect_correct_ui->load     ();
  disable_scissor->load       ();

  complete_mipmaps->load      ();

  shadow_scale->load          ();
  postprocess_ratio->load     ();
}

bool
tzfixcfg_Graphics::setup_ui (HWND hDlg)
{
  hWndAspectRatioCorrection = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_ARC);
  hWndDisableScissor        = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_DISABLE_SCISSOR);
  hWndPostProcessScale      = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_POSTPROCESS_SCALE);
  hWndShadowRes             = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_SHADOW_RES);
  hWndDoubleResSorrey       = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_DOUBLE_RES_SORREY);
  hWndForceCompleteMipMaps  = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_FORCE_MIPMAPS);

  disable_scissor->bind_to_control (new tzt::UI::CheckBox (hWndDisableScissor));
  disable_scissor->set_value (disable_scissor->get_value ());

  postprocess_ratio->bind_to_control (new tzt::UI::EditBox (hWndPostProcessScale));
  postprocess_ratio->set_value (postprocess_ratio->get_value ());

  complete_mipmaps->bind_to_control (new tzt::UI::CheckBox (hWndForceCompleteMipMaps));
  complete_mipmaps->set_value (complete_mipmaps->get_value ());

  ComboBox_ResetContent (hWndAspectRatioCorrection);
  ComboBox_ResetContent (hWndShadowRes);

  ComboBox_InsertString (hWndAspectRatioCorrection, 0, L"Nothing");
  ComboBox_InsertString (hWndAspectRatioCorrection, 1, L"Only Videos");
  ComboBox_InsertString (hWndAspectRatioCorrection, 2, L"Only UI");
  ComboBox_InsertString (hWndAspectRatioCorrection, 3, L"Everything");

  int ar_sel;

  if (aspect_correct_videos->get_value () &&
      aspect_correct_ui->get_value     ())
    ar_sel = 3;
  else if (aspect_correct_videos->get_value ())
    ar_sel = 1;
  else if (aspect_correct_ui->get_value ())
    ar_sel = 2;
  else
    ar_sel = 0;

  ComboBox_SetCurSel (hWndAspectRatioCorrection, ar_sel);

  Button_Enable (hWndDisableScissor, aspect_correct_ui->get_value ());

  ComboBox_InsertString (hWndShadowRes, 0, L"Original (128x128)");
  ComboBox_InsertString (hWndShadowRes, 1, L" 2x (256x256)");
  ComboBox_InsertString (hWndShadowRes, 2, L" 4x (512x512)");
  ComboBox_InsertString (hWndShadowRes, 3, L" 8x (1024x1024)");
  ComboBox_InsertString (hWndShadowRes, 4, L"16x (2048x2048)");
  ComboBox_InsertString (hWndShadowRes, 5, L"32x (4096x4096)");

  ComboBox_SetCurSel (hWndShadowRes, abs (shadow_scale->get_value ()));

  Button_Enable   (hWndDoubleResSorrey, shadow_scale->get_value () != 0);
  Button_SetCheck (hWndDoubleResSorrey, shadow_scale->get_value ()  > 0);

  return true;
}

INT_PTR
CALLBACK
GraphicsConfig (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG:
    {
      if (graphics == nullptr) {
        graphics = new tzfixcfg_Graphics ();
      }

      graphics->setup_ui (hDlg);
    } break;

    case WM_COMMAND:
    {
      if (LOWORD (wParam) == IDC_TZFIX_GRAPHICS_ARC) {
        if (HIWORD (wParam) == CBN_SELCHANGE) {
          int ar_sel = ComboBox_GetCurSel (graphics->hWndAspectRatioCorrection);
          Button_Enable (graphics->hWndDisableScissor, ar_sel >= 2);
          graphics->aspect_correct_videos->set_value (ar_sel == 1 || ar_sel == 3);
          graphics->aspect_correct_ui->set_value     (ar_sel == 2 || ar_sel == 3);
        }
      }

      if (LOWORD (wParam) == IDC_TZFIX_GRAPHICS_SHADOW_RES ||
          LOWORD (wParam) == IDC_TZFIX_GRAPHICS_DOUBLE_RES_SORREY) {
        if (HIWORD (wParam) == CBN_SELCHANGE ||
            LOWORD (wParam) == IDC_TZFIX_GRAPHICS_DOUBLE_RES_SORREY) {
          int shadow_sel = ComboBox_GetCurSel (graphics->hWndShadowRes);
          Button_Enable (graphics->hWndDoubleResSorrey, shadow_sel > 0);

          int shadow_res = shadow_sel;
          if (! Button_GetCheck (graphics->hWndDoubleResSorrey))
            shadow_res *= -1;

          graphics->shadow_scale->set_value (shadow_res);
        }
      }

      if (LOWORD (wParam) == IDOK)
      {
        graphics->aspect_correct_videos->store ();
        graphics->aspect_correct_ui->store     ();
        graphics->disable_scissor->store       ();

        graphics->complete_mipmaps->store      ();

        graphics->shadow_scale->store          ();
        graphics->postprocess_ratio->store     ();

        EndDialog (hDlg, LOWORD (wParam));
        return (INT_PTR)TRUE;
      }
    } break;
  }

  return (INT_PTR)FALSE;
}