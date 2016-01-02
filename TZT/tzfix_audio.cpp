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

class tzfixcfg_Audio
{
public:
  tzfixcfg_Audio (void);

  bool setup_ui  (HWND hDlg);

  int  poll_sample_rate (HWND hDlg);
  int  poll_channels    (HWND hDlg);

//protected:
  tzt::ParameterBool* enable;
  tzt::ParameterBool* compatibility;
  tzt::ParameterInt*  sample_rate;
  tzt::ParameterInt*  max_channels;

//private:
  HWND hWndEnable;
  HWND hWndCompatibility;
  HWND hWndChannels;
  HWND hWndChannelsLabel;
  HWND hWndSampleRate;
  HWND hWndSampleRateLabel;
  HWND hWndStatus;
} *audio = nullptr;

#include <initguid.h>
#pragma comment (lib, "ksproxy.lib")
#include <ks.h>
//#include <ksproxy.h>
#include <ksmedia.h>

#include <Mmdeviceapi.h>
#include <audioendpoints.h>
#include <audioclient.h>

#include <mmeapi.h>
#include <dsound.h>
#pragma comment (lib, "dsound.lib")

wchar_t wszConfigStatus [16384] = { L"\0" };

const wchar_t*
dsound_channel_config_name (DWORD dwLayout)
{
  switch (dwLayout)
  {
  case DSSPEAKER_DIRECTOUT:
    return L"Direct Out";
  case DSSPEAKER_HEADPHONE:
    return L"Headphone";
  case DSSPEAKER_MONO:
    return L"Monophonic";
  case DSSPEAKER_QUAD:
    return L"Quadraphonic";
  case DSSPEAKER_STEREO:
    return L"Stereo";
  case DSSPEAKER_SURROUND:
    return L"Surround";
  case DSSPEAKER_5POINT1:
    return L"5.1 (Obsolete Setting - AVOIDME)";
  case DSSPEAKER_7POINT1:
    return L"7.1 (Obsolete Setting - AVOIDME)";
  case DSSPEAKER_7POINT1_SURROUND:
    return L"7.1 Surround";
  case DSSPEAKER_5POINT1_SURROUND:
    return L"5.1 Surround";
  default:
    return L"UNKNOWN?!";
  }
}

std::wstring
dsound_speaker_geometry_name (DWORD dwGeometry)
{
  switch (dwGeometry)
  {
  case DSSPEAKER_GEOMETRY_MIN:
    return L"Minimum (5°)";
  case DSSPEAKER_GEOMETRY_NARROW:
    return L"Narrow (10°)";
  case DSSPEAKER_GEOMETRY_WIDE:
    return L"Wide (20°)";
  case DSSPEAKER_GEOMETRY_MAX:
    return L"Maximum (180°)";
  default:
  {
    wchar_t wszCustom [64];
    swprintf (wszCustom, L"Custom (%lu°)", dwGeometry);
    return wszCustom;
  }
  }
}

void
setup_config_status (HWND hDlg)
{
  PROPVARIANT     property;
  WAVEFORMATEX*   pMixFormat = nullptr;

  wchar_t* pwszStat = wszConfigStatus;

  //pwszStat += swprintf (pwszStat, L"Creating Default DirectSound Device... ");

  LPDIRECTSOUND pDS = nullptr;
  if (SUCCEEDED (DirectSoundCreate (NULL, &pDS, NULL)) && pDS != nullptr) {
    pDS->Initialize (NULL);
    //pwszStat += swprintf (pwszStat, L"Success!\r\n");

    pwszStat += swprintf (pwszStat, L" >> Retrieving Channel Configuration...\t");
    DWORD dwSpeakerConfig;
    pDS->GetSpeakerConfig (&dwSpeakerConfig);
    pwszStat += swprintf (pwszStat, L"%d\t(%s)\r\n",
                                DSSPEAKER_CONFIG (dwSpeakerConfig),
                      dsound_channel_config_name (
                                DSSPEAKER_CONFIG (dwSpeakerConfig)));

#if 0
    pwszStat += swprintf (pwszStat, L" >> Retrieving Speaker Geometry...\t\t");
    pwszStat += swprintf (pwszStat, L"%d\t(%s)\r\n",
        DSSPEAKER_GEOMETRY (dwSpeakerConfig),
      dsound_speaker_geometry_name (
        DSSPEAKER_GEOMETRY (dwSpeakerConfig)).c_str ());
#endif

    DSCAPS sound_caps;
    if (SUCCEEDED (pDS->GetCaps (&sound_caps))) {
      pwszStat += swprintf (pwszStat, L"---------------------------------------------"
        L"\r\nMinimum Sample Rate:\t%lu"
        L"\r\nMaximum Sample Rate: %lu\r\n",
        sound_caps.dwMinSecondarySampleRate,
        sound_caps.dwMaxSecondarySampleRate );
    }

    IMMDeviceEnumerator* pEnumerator;

    const CLSID CLSID_MMDeviceEnumerator = __uuidof (MMDeviceEnumerator);
    const IID   IID_IMMDeviceEnumerator  = __uuidof (IMMDeviceEnumerator);

    HRESULT hr = CoCreateInstance (
                   CLSID_MMDeviceEnumerator, NULL,
                   CLSCTX_ALL, IID_IMMDeviceEnumerator,
                   (void **)&pEnumerator
                 );

    if (SUCCEEDED (hr)) {
      IMMDevice* pDev;

      if (SUCCEEDED (
             pEnumerator->GetDefaultAudioEndpoint (eRender, eConsole, &pDev)
           )
         )
      {
        IAudioClient* pAudioClient;
        if (SUCCEEDED (pDev->Activate (
                         __uuidof (IAudioClient), CLSCTX_ALL,
                         NULL, (void **)&pAudioClient )
                      )
           ) {
          IPropertyStore* pStore;

          if (SUCCEEDED (pDev->OpenPropertyStore (STGM_READ, &pStore)))
          {
            if (SUCCEEDED (pStore->GetValue (PKEY_AudioEngine_DeviceFormat, &property)))
            {
              pMixFormat = (PWAVEFORMATEX)property.blob.pBlobData;

              //pAudioClient->GetMixFormat ((WAVEFORMATEX **)&pMixFormat);

              pwszStat += swprintf (pwszStat, L"---------------------------------------------"
                                              L"\r\n * Default Playback Endpoint:\r\n"
                                              L"\tChannel Count.:\t%lu\r\n"
                                              L"\tSample Rate...:\t%4.1f kHz\r\n"
                                              L"\tData Type.....:\t%hu-Bits\r\n",
                                                pMixFormat->nChannels,
                                                (float)pMixFormat->nSamplesPerSec / 1000.0f,
                                                pMixFormat->wBitsPerSample);

          //pMixFormat->wBitsPerSample = ((PWAVEFORMATEXTENSIBLE)pMixFormat)->Samples.wValidBitsPerSample;

          //pMixFormat->nChannels = 4;
          //pStore->SetValue        (PKEY_AudioEngine_DeviceFormat, property);

          if (pMixFormat->nChannels > 6) {
#if 0
            MessageBox ( NULL,
                           L"Too many audio channels!"
                           L"\tPlease select a speaker configuration or device "
                            L"with 5.1 channels or fewer",
                             L"Bad Audio Configuration",
                               MB_OK | MB_ICONSTOP );

            char snd_cfg [MAX_PATH];
            GetSystemDirectoryA (snd_cfg, MAX_PATH);
            lstrcatA (snd_cfg, "\\control.exe mmsys.cpl");

            WinExec (snd_cfg, SW_SHOW);
#endif
          }
          }
          pStore->Release ();
          }
        }
        pDev->Release ();
      }
      pEnumerator->Release ();
    }

    //pwszStat += swprintf (pwszStat, L"Releasing Default DirectSound Device... ");
    pDS->Release ();
    //pwszStat += swprintf (pwszStat, L"done!\r\n");
  } else {
    pwszStat += swprintf (pwszStat, L"Failed to Create DirectSound Device!\r\n");
    Edit_SetText (GetDlgItem (hDlg, IDC_TZFIX_AUDIO_STATUS), wszConfigStatus);
  }

  pwszStat += swprintf (pwszStat, L"---------------------------------------------\r\n");

  if (pMixFormat != nullptr) {
    if (pMixFormat->nSamplesPerSec > 48000) {
      pwszStat += swprintf (pwszStat, L"Your audio configuration *REQUIRES* the Audio Fix.");
    } else {
      if (pMixFormat->nChannels > 6)
        pwszStat += swprintf (pwszStat, L"Your audio configuration _MAY_ require the Audio Fix.");
      else
        pwszStat += swprintf (pwszStat, L"Your audio configuration *DOES NOT* require the Audio Fix.");
    }
  } else {
    pwszStat += swprintf (pwszStat, L"Your audio configuration is *UNKNOWN*, game may crash.");
  }

  Edit_SetText (GetDlgItem (hDlg, IDC_TZFIX_AUDIO_STATUS), wszConfigStatus);
}

tzfixcfg_Audio::tzfixcfg_Audio (void)
{
  tzt::INI::File* tzfix_ini = config.get_file_tzfix ();

  enable = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Enable Audio Fix"
    )
  );
  enable->register_to_ini ( tzfix_ini,
                              L"TZFIX.Audio",
                                L"EnableFix" );

  compatibility = static_cast <tzt::ParameterBool *> (
    tzt::g_ParameterFactory.create_parameter <bool> (
      L"Compatibility Mode"
    )
  );
  compatibility->register_to_ini ( tzfix_ini,
                                     L"TZFIX.Audio",
                                       L"CompatibilityMode" );

  sample_rate = static_cast <tzt::ParameterInt *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"Audio Sample Rate"
    )
  );
  sample_rate->register_to_ini ( tzfix_ini,
                                   L"TZFIX.Audio",
                                     L"SampleRate" );

  max_channels = static_cast <tzt::ParameterInt *> (
    tzt::g_ParameterFactory.create_parameter <int> (
      L"Maximum Audio Channels"
    )
  );
  max_channels->register_to_ini ( tzfix_ini,
                                    L"TZFIX.Audio",
                                      L"Channels" );

  enable->load        ();
  compatibility->load ();
  sample_rate->load   ();
  max_channels->load  ();
}

bool
tzfixcfg_Audio::setup_ui (HWND hDlg)
{
  hWndEnable        = GetDlgItem (hDlg, IDC_TZFIX_AUDIO_ENABLE);
  hWndCompatibility = GetDlgItem (hDlg, IDC_TZFIX_AUDIO_COMPATIBILITY);
  hWndChannels      = GetDlgItem (hDlg, IDC_TZFIX_AUDIO_CHANNELS);
  hWndChannelsLabel = GetDlgItem (hDlg, IDC_TZFIX_CHANNEL_LABEL);
  hWndSampleRate    = GetDlgItem (hDlg, IDC_TZFIX_AUDIO_RATE);
  hWndSampleRateLabel = GetDlgItem (hDlg, IDC_TZFIX_RATE_LABEL);
  hWndStatus        = GetDlgItem (hDlg, IDC_TZFIX_AUDIO_STATUS);

  setup_config_status (hDlg);

  enable->bind_to_control        (new tzt::UI::CheckBox (hWndEnable));
  enable->set_value (enable->get_value ());
  compatibility->bind_to_control (new tzt::UI::CheckBox (hWndCompatibility));
  compatibility->set_value (compatibility->get_value ());

  ComboBox_ResetContent (hWndChannels);
  ComboBox_ResetContent (hWndSampleRate);

  ComboBox_InsertString (hWndChannels, 0, L"Stereo");
  ComboBox_InsertString (hWndChannels, 1, L"Quadraphonic");
  ComboBox_InsertString (hWndChannels, 2, L"5.1 Surround");
  ComboBox_InsertString (hWndChannels, 3, L"7.1 Surround");

  int num_channels = max_channels->get_value ();

  if (num_channels == 2)
    ComboBox_SetCurSel (hWndChannels, 0);
  else if (num_channels == 4)
    ComboBox_SetCurSel (hWndChannels, 1);
  else if (num_channels == 6)
    ComboBox_SetCurSel (hWndChannels, 2);
  else /*if (num_channels == 8)*/
    ComboBox_SetCurSel (hWndChannels, 3);

  ComboBox_InsertString (hWndSampleRate, 0, L"44.1 kHz");
  ComboBox_InsertString (hWndSampleRate, 1, L"48.0 kHz");

  int rate = sample_rate->get_value ();

  if (rate == 44100)
    ComboBox_SetCurSel (hWndSampleRate, 0);
  else
    ComboBox_SetCurSel (hWndSampleRate, 1);

  bool check = Button_GetCheck (audio->hWndEnable);
  audio->enable->set_value (check);

  EnableWindow (audio->hWndChannels,        check);
  EnableWindow (audio->hWndChannelsLabel,   check);
  EnableWindow (audio->hWndSampleRate,      check);
  EnableWindow (audio->hWndSampleRateLabel, check);
  EnableWindow (audio->hWndCompatibility,   check);

  return true;
}

int
tzfixcfg_Audio::poll_channels (HWND hDlg)
{
  int sel = ComboBox_GetCurSel (hWndChannels);

  if (sel == 0)
    return 2;
  else if (sel == 1)
    return 4;
  else if (sel == 2)
    return 6;
  else /*if (sel == 3)*/
    return 8;
}

int
tzfixcfg_Audio::poll_sample_rate (HWND hDlg)
{
  int sel = ComboBox_GetCurSel (hWndSampleRate);

  if (sel == 0)
    return 44100;
  else /*if (sel == 1)*/
    return 48000;
}

INT_PTR
CALLBACK
AudioConfig (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG:
    {
      if (audio == nullptr) {
        audio = new tzfixcfg_Audio ();
      }

      audio->setup_ui (hDlg);
    } break;

    case WM_COMMAND:
    {
      if (LOWORD (wParam) == IDC_TZFIX_AUDIO_ENABLE) {
        bool check = Button_GetCheck (audio->hWndEnable);
        audio->enable->set_value (check);

        EnableWindow (audio->hWndChannels,        check);
        EnableWindow (audio->hWndChannelsLabel,   check);
        EnableWindow (audio->hWndSampleRate,      check);
        EnableWindow (audio->hWndSampleRateLabel, check);
        EnableWindow (audio->hWndCompatibility,   check);
      }

      if (LOWORD (wParam) == IDOK)
      {
        bool check = Button_GetCheck (audio->hWndEnable);
        audio->enable->set_value (check);

        check = Button_GetCheck (audio->hWndCompatibility);
        audio->compatibility->set_value (check);

        audio->enable->store        ();
        audio->compatibility->store ();

        audio->sample_rate->set_value  (audio->poll_sample_rate (hDlg));
        audio->max_channels->set_value (audio->poll_channels    (hDlg));

        audio->sample_rate->store   ();
        audio->max_channels->store  ();

        EndDialog (hDlg, LOWORD (wParam));
        return (INT_PTR)TRUE;
      }
    } break;
  }

  return (INT_PTR)FALSE;
}