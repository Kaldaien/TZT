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
  tzt::ParameterBool*    aspect_correct_videos;
  tzt::ParameterBool*    aspect_correct_ui;
  tzt::ParameterBool*    clear_blackbars;
  tzt::ParameterBool*    complete_mipmaps;

  tzt::ParameterInt*     shadow_scale;
  tzt::ParameterFloat*   postprocess_ratio;

  tzt::ParameterStringW* intro_video;

//private:
  HWND hWndAspectRatioCorrection;
  HWND hWndClearBlackbars;
  HWND hWndPostProcessScale;
  HWND hWndShadowRes;
  HWND hWndDoubleResSorey;
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

  clear_blackbars = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Clear non-16:9 Tops / Sides"
    )
  );
  clear_blackbars->register_to_ini ( tzfix_ini,
                                       L"TZFIX.Render",
                                         L"ClearBlackbars" );

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

  intro_video = static_cast <tzt::ParameterStringW *> (
    tzt::g_ParameterFactory.create_parameter <std::wstring> (
      L"Intro Video"
    )
  );
  intro_video->register_to_ini ( tzfix_ini,
                                   L"TZFIX.System",
                                     L"IntroVideo" );

  aspect_correct_videos->load ();
  aspect_correct_ui->load     ();
  clear_blackbars->load       ();

  complete_mipmaps->load      ();

  shadow_scale->load          ();
  postprocess_ratio->load     ();

  intro_video->load           ();
}

bool
tzfixcfg_Graphics::setup_ui (HWND hDlg)
{
  hWndAspectRatioCorrection = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_ARC);
  hWndClearBlackbars        = GetDlgItem (hDlg, IDC_CLEAR_BLACKBARS);
  hWndPostProcessScale      = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_POSTPROCESS_SCALE);
  hWndShadowRes             = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_SHADOW_RES);
  hWndDoubleResSorey        = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_DOUBLE_RES_SOREY);
  hWndForceCompleteMipMaps  = GetDlgItem (hDlg, IDC_TZFIX_GRAPHICS_FORCE_MIPMAPS);

  clear_blackbars->bind_to_control (new tzt::UI::CheckBox (hWndClearBlackbars));
  clear_blackbars->set_value (clear_blackbars->get_value ());

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

  Button_Enable (hWndClearBlackbars, aspect_correct_ui->get_value ());

  ComboBox_InsertString (hWndShadowRes, 0, L"Original (128x128)");
  ComboBox_InsertString (hWndShadowRes, 1, L" 2x (256x256)");
  ComboBox_InsertString (hWndShadowRes, 2, L" 4x (512x512)");
  ComboBox_InsertString (hWndShadowRes, 3, L" 8x (1024x1024)");
  ComboBox_InsertString (hWndShadowRes, 4, L"16x (2048x2048)");
  ComboBox_InsertString (hWndShadowRes, 5, L"32x (4096x4096)");

  ComboBox_SetCurSel (hWndShadowRes, abs (shadow_scale->get_value ()));

  Button_Enable   (hWndDoubleResSorey, shadow_scale->get_value () != 0);
  Button_SetCheck (hWndDoubleResSorey, shadow_scale->get_value ()  > 0);

  return true;
}

HRESULT SelectIntroVideo (void)
{
  IFileDialog *pfd = nullptr;

  HRESULT hr =
    CoCreateInstance ( CLSID_FileOpenDialog,
                         NULL,
                           CLSCTX_INPROC_SERVER,
                             IID_PPV_ARGS (&pfd) );

  if (SUCCEEDED (hr)) {
    IFileDialogEvents *pfde = NULL;

    hr = CDialogEventHandler_CreateInstance (IID_PPV_ARGS (&pfde));

    if (SUCCEEDED (hr)) {
      DWORD dwCookie;

      hr = pfd->Advise (pfde, &dwCookie);
      if (SUCCEEDED (hr)) {
        DWORD dwFlags;

        hr = pfd->GetOptions (&dwFlags);

        if (SUCCEEDED (hr)) {
          hr = pfd->SetOptions (dwFlags | FOS_FORCEFILESYSTEM);

          if (SUCCEEDED (hr)) {
            COMDLG_FILTERSPEC rgSpec [] = {
                { L"Bink2 Video (.bk2)", L"*.bk2" },
            };

            hr = pfd->SetFileTypes (ARRAYSIZE (rgSpec), rgSpec);

            if (SUCCEEDED (hr)) {
              hr = pfd->SetFileTypeIndex (1);

              if (SUCCEEDED (hr)) {
                hr = pfd->SetDefaultExtension (L"bk2");

                if (SUCCEEDED (hr)) {
                  hr = pfd->Show (NULL);

                  if (SUCCEEDED (hr)) {
                    IShellItem *psiResult;

                    hr = pfd->GetResult(&psiResult);

                    if (SUCCEEDED (hr)) {
                      PWSTR pszFilePath = NULL;

                      hr =
                        psiResult->GetDisplayName ( SIGDN_FILESYSPATH,
                                                      &pszFilePath );

                      if (SUCCEEDED (hr)) {
                        graphics->intro_video->set_value (pszFilePath);
                        CoTaskMemFree (pszFilePath);
                      }

                      psiResult->Release ();
                    }
                  }
                }
              }
            }
          }
        }

        pfd->Unadvise (dwCookie);
      }

      pfde->Release ();
    }

    pfd->Release ();
  }

  return hr;
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
          Button_Enable (graphics->hWndClearBlackbars, ar_sel >= 2);
          graphics->aspect_correct_videos->set_value (ar_sel == 1 || ar_sel == 3);
          graphics->aspect_correct_ui->set_value     (ar_sel == 2 || ar_sel == 3);
        }
      }

      if (LOWORD (wParam) == IDC_TZFIX_GRAPHICS_SHADOW_RES ||
          LOWORD (wParam) == IDC_TZFIX_GRAPHICS_DOUBLE_RES_SOREY) {
        if (HIWORD (wParam) == CBN_SELCHANGE ||
            LOWORD (wParam) == IDC_TZFIX_GRAPHICS_DOUBLE_RES_SOREY) {
          int shadow_sel = ComboBox_GetCurSel (graphics->hWndShadowRes);
          Button_Enable (graphics->hWndDoubleResSorey, shadow_sel > 0);

          int shadow_res = shadow_sel;
          if (! Button_GetCheck (graphics->hWndDoubleResSorey))
            shadow_res *= -1;

          graphics->shadow_scale->set_value (shadow_res);
        }
      }

      if (LOWORD (wParam) == IDC_INTRO_VIDEO) {
        SelectIntroVideo ();
      }

      if (LOWORD (wParam) == IDOK)
      {
        graphics->aspect_correct_videos->store ();
        graphics->aspect_correct_ui->store     ();
        graphics->clear_blackbars->store       ();

        graphics->complete_mipmaps->store      ();

        graphics->shadow_scale->store          ();
        graphics->postprocess_ratio->store     ();

        graphics->intro_video->store           ();

        EndDialog (hDlg, LOWORD (wParam));
        return (INT_PTR)TRUE;
      }
    } break;
  }

  return (INT_PTR)FALSE;
}