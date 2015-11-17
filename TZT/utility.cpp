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

#include "utility.h"

#include <UserEnv.h>
#pragma comment (lib, "userenv.lib")

#include <Shlobj.h>
#pragma comment (lib, "shell32.lib")

int
TZT_MessageBox (std::wstring caption, std::wstring title, uint32_t flags)
{
  extern bool messagebox_active;
  extern HWND hWndApp;

  HWND parent = IsWindow (hWndApp) ? hWndApp : NULL;

  messagebox_active = true;

  int ret = MessageBox (hWndApp, caption.c_str (), title.c_str (), flags | MB_SYSTEMMODAL | MB_TOPMOST | MB_SETFOREGROUND);

  messagebox_active = false;

  return ret;
}

std::wstring
TZT_GetDocumentsDir (void)
{
  HANDLE hToken;

  if (!OpenProcessToken (GetCurrentProcess (), TOKEN_READ, &hToken))
    return NULL;

  wchar_t* str;
  SHGetKnownFolderPath (FOLDERID_Documents, 0, hToken, &str);

    std::wstring ret = str;

  CoTaskMemFree (str);

  return ret;
}

BOOL
TZT_GetUserProfileDir (wchar_t* buf, DWORD* pdwLen)
{
  HANDLE hToken;

  if (! OpenProcessToken (GetCurrentProcess (), TOKEN_READ, &hToken))
    return FALSE;

  if (! GetUserProfileDirectory (hToken, buf, pdwLen))
    return FALSE;

  CloseHandle (hToken);
  return TRUE;
}

std::wstring
TZT_GetLocalAppDataDir (void)
{
  HANDLE hToken;

  if (! OpenProcessToken (GetCurrentProcess (), TOKEN_READ, &hToken))
    return NULL;

  wchar_t* str;
  SHGetKnownFolderPath (FOLDERID_LocalAppData, 0, hToken, &str);

    std::wstring ret = str;

  CoTaskMemFree (str);

  return ret;
}

#include <string>

bool
TZT_IsTrue (const wchar_t* string)
{
  if (std::wstring (string).length () == 1 &&
    string [0] == L'1')
    return true;

  if (std::wstring (string).length () != 4)
    return false;

  if (towlower (string [0]) != L't')
    return false;
  if (towlower (string [1]) != L'r')
    return false;
  if (towlower (string [2]) != L'u')
    return false;
  if (towlower (string [3]) != L'e')
    return false;

  return true;
}

bool
TZT_IsAdmin (void)
{
  bool   bRet   = false;
  HANDLE hToken = 0;

  if (OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &hToken)) {
    TOKEN_ELEVATION Elevation;
    DWORD cbSize = sizeof (TOKEN_ELEVATION);

    if (GetTokenInformation (hToken, TokenElevation, &Elevation, sizeof (Elevation), &cbSize)) {
      bRet = Elevation.TokenIsElevated != 0;
    }
  }

  if (hToken)
    CloseHandle (hToken);

  return bRet;
}

void
TZT_DeleteAllConfigFiles (void)
{
  // Strip Read-Only
  TZT_SetNormalFileAttribs (std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.CFG"));

  DeleteFile (std::wstring (TZT_GetLocalAppDataDir  () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.CFG").c_str ());
  DeleteFile (std::wstring (TZT_GetSteamUserDataDir () + L"\\351970\\remote\\WinData.xml").c_str ());
}

bool
TZT_HasBackupConfigFiles (void)
{
  WIN32_FIND_DATA FindFileData;

  if (FindFirstFile (std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.TZT").c_str (), &FindFileData) != INVALID_HANDLE_VALUE)
    return true;

  return false;
}

// Copies a file preserving file times
void
TZT_FullCopy (std::wstring from, std::wstring to)
{
  // Strip Read-Only
  TZT_SetNormalFileAttribs (to);
  DeleteFile (to.c_str ());
  CopyFile   (from.c_str (), to.c_str (), FALSE);

  WIN32_FIND_DATA FromFileData;
  HANDLE hFrom = FindFirstFile (from.c_str (), &FromFileData);

  OFSTRUCT ofTo;
  ofTo.cBytes = sizeof (OFSTRUCT);

  char     szFileTo [MAX_PATH];

  WideCharToMultiByte (CP_OEMCP, 0, to.c_str (), -1, szFileTo, MAX_PATH, NULL, NULL);
  HFILE hfTo = OpenFile (szFileTo, &ofTo, NULL);

  CloseHandle ((HANDLE)hfTo);

  // Here's where the magic happens, apply the attributes from the original file to the new one!
  SetFileTime ((HANDLE)hfTo, &FromFileData.ftCreationTime, &FromFileData.ftLastAccessTime, &FromFileData.ftLastWriteTime);

  FindClose   (hFrom);
}

void
TZT_CreateBackupConfig (void)
{
  TZT_FullCopy ( std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.CFG"),
                 std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.TZT") );

  TZT_FullCopy ( std::wstring (TZT_GetSteamUserDataDir () + L"\\351970\\remote\\WinData.xml"),
                 std::wstring (TZT_GetSteamUserDataDir () + L"\\351970\\remote\\WinData.tzt") );
}

void
TZT_RestoreConfigFiles (void)
{
  TZT_FullCopy ( std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.TZT"),
                 std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.CFG") );

  TZT_FullCopy ( std::wstring (TZT_GetSteamUserDataDir () + L"\\351970\\remote\\WinData.tzt"),
                 std::wstring (TZT_GetSteamUserDataDir () + L"\\351970\\remote\\WinData.xml") );

  // Strip Read-Only
  TZT_SetNormalFileAttribs (std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.TZT"));

  DeleteFile (std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.TZT").c_str ());
}

// Gets the timestamp on the current backup file
std::wstring
TZT_GetBackupFileTime (void)
{
  WIN32_FIND_DATA FindFileData;

  HANDLE hFileBackup = FindFirstFile (std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.TZT").c_str (), &FindFileData);

  FindClose (hFileBackup);

  FILETIME   ftModified;
  FileTimeToLocalFileTime (&FindFileData.ftLastWriteTime, &ftModified);

  SYSTEMTIME stModified;
  FileTimeToSystemTime (&ftModified, &stModified);

  wchar_t wszFileTime [512];

  GetDateFormat (LOCALE_CUSTOM_UI_DEFAULT, DATE_AUTOLAYOUT, &stModified, NULL, wszFileTime, 512);

  std::wstring date_time = wszFileTime;

  GetTimeFormat (LOCALE_CUSTOM_UI_DEFAULT, TIME_NOSECONDS, &stModified, NULL, wszFileTime, 512);

  date_time += L" ";
  date_time += wszFileTime;

  return date_time;
}

// Gets the timestamp on the current config file
std::wstring
TZT_GetConfigFileTime (void)
{
  WIN32_FIND_DATA FindFileData;

  HANDLE hFileBackup = FindFirstFile (std::wstring (TZT_GetLocalAppDataDir () + L"\\BANDAI NAMCO Games\\Tales of Zestiria\\TOZ.CFG").c_str (), &FindFileData);

  FILETIME   ftModified;
  FileTimeToLocalFileTime (&FindFileData.ftLastWriteTime, &ftModified);

  SYSTEMTIME stModified;
  FileTimeToSystemTime (&ftModified, &stModified);

  FindClose (hFileBackup);

  wchar_t wszFileTime [512];

  GetDateFormat (LOCALE_CUSTOM_UI_DEFAULT, DATE_AUTOLAYOUT, &stModified, NULL, wszFileTime, 512);

  std::wstring date_time = wszFileTime;

  GetTimeFormat (LOCALE_CUSTOM_UI_DEFAULT, TIME_NOSECONDS, &stModified, NULL, wszFileTime, 512);

  date_time += L" ";
  date_time += wszFileTime;

  return date_time;
}

void
TZT_SetNormalFileAttribs (std::wstring file)
{
  SetFileAttributes (file.c_str (), FILE_ATTRIBUTE_NORMAL);
}






static const char* everything_else = {
  "shadows=high\n"
  "anisotropy=16\n"
  "antialiasing=fxaa_low\n"
  "draw_distance=medium\n"
  "lod_distance=medium\n"
  "ControlScheme00=0\n"
  "ControlScheme01=0\n"
  "ControlScheme02=0\n"
  "ControlScheme03=0\n"
  "ControlSA0=0;0;30;33;32;31;8;513;75;515;26;22;4;7;82;81;80;79;224;41;225;16;224;43;23;20;513;515;82;81;80;79;8;57;44;30;31;32;33;225;43;513;44;16;23;40;16;513;515;44;8;225;57;43;21;26;22;4;7\n"
  "ControlSA1=0;1;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;6;8;27;28;29;30;5;24;7;1;4;3;2;19;22;25;7;20;23;17;20;8;6;7;5;19;20;22;23;9;10;11;12\n"
  "ControlSA2=0;2;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;6;8;27;28;29;30;5;24;7;1;4;3;2;19;22;25;7;20;23;17;20;8;6;7;5;19;20;22;23;9;10;11;12\n"
  "ControlSA3=1;0;30;33;32;31;8;513;75;515;26;22;4;7;82;81;80;79;224;41;225;16;224;43;23;20;513;515;82;81;80;79;8;57;44;30;31;32;33;225;43;513;44;16;23;40;16;513;515;44;8;225;57;43;21;26;22;4;7\n"
  "ControlSA4=1;1;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;6;8;27;28;29;30;5;24;7;1;4;3;2;19;22;25;7;20;23;17;20;8;6;7;5;19;20;22;23;9;10;11;12\n"
  "ControlSA5=1;2;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;6;8;27;28;29;30;5;24;7;1;4;3;2;19;22;25;7;20;23;17;20;8;6;7;5;19;20;22;23;9;10;11;12\n"
  "ControlSA6=2;0;30;33;32;31;8;513;75;515;26;22;4;7;82;81;80;79;224;41;225;16;224;43;23;20;513;515;82;81;80;79;8;57;44;30;31;32;33;225;43;513;44;16;23;40;16;513;515;44;8;225;57;43;21;26;22;4;7\n"
  "ControlSA7=2;1;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;6;8;27;28;29;30;5;24;7;1;4;3;2;19;22;25;7;20;23;17;20;8;6;7;5;19;20;22;23;9;10;11;12\n"
  "ControlSA8=2;2;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;6;8;27;28;29;30;5;24;7;1;4;3;2;19;22;25;7;20;23;17;20;8;6;7;5;19;20;22;23;9;10;11;12\n"
  "ControlSA9=3;0;30;33;32;31;8;513;75;515;26;22;4;7;82;81;80;79;224;41;225;16;224;43;23;20;513;515;82;81;80;79;8;57;44;30;31;32;33;225;43;513;44;16;23;40;16;513;515;44;8;225;57;43;21;26;22;4;7\n"
  "ControlSA10=3;1;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;6;8;27;28;29;30;5;24;7;1;4;3;2;19;22;25;7;20;23;17;20;8;6;7;5;19;20;22;23;9;10;11;12\n"
  "ControlSA11=3;2;1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;16;17;18;19;20;21;22;23;24;6;8;27;28;29;30;5;24;7;1;4;3;2;19;22;25;7;20;23;17;20;8;6;7;5;19;20;22;23;9;10;11;12" };

std::wstring
TZT_ErrorMessage (errno_t        err,
                  const char*    args,
                  const wchar_t* ini_name,
                  UINT           line_no,
                  const char*    function_name,
                  const char*    file_name)
{
  wchar_t wszFile           [256];
  wchar_t wszFunction       [256];
  wchar_t wszArgs           [256];
  wchar_t wszFormattedError [1024];

  MultiByteToWideChar (CP_OEMCP, 0, file_name,     -1, wszFile,     256);
  MultiByteToWideChar (CP_OEMCP, 0, function_name, -1, wszFunction, 256);
  MultiByteToWideChar (CP_OEMCP, 0, args,          -1, wszArgs,     256);
  *wszFormattedError = L'\0';

  swprintf (wszFormattedError, 1024,
    L"Line %u of %s (in %s (...)):\n"
    L"------------------------\n\n"
    L"%s\n\n  File: %s\n\n"
    L"\t>> %s <<",
    line_no,
    wszFile,
    wszFunction,
    wszArgs,
    ini_name,
    _wcserror (err));

  return wszFormattedError;
}

#define TRY_FILE_IO(x,y,z) { (z) = ##x; if ((z) != 0) MessageBox (NULL, TZT_ErrorMessage ((z), #x, (y), __LINE__, __FUNCTION__, __FILE__).c_str (), L"File I/O Error", MB_OK | MB_ICONSTOP ); }

struct toz_cfg_t
{
  int res_x,
    res_y,
    refresh;

  bool fullscreen;
  bool vsync;

  void init (void) {
    std::wstring fname = TZT_GetLocalAppDataDir ();

    fname += L"\\BANDAI NAMCO Games";
    CreateDirectory (fname.c_str (), NULL);

    fname += L"\\Tales of Zestiria";
    CreateDirectory (fname.c_str (), NULL);

    fname += L"\\toz.cfg";

    //printf ("Writing configuration to '%ls'... ", fname.c_str ());

    FILE*   fCFG = nullptr;
    errno_t err;

    char fname_s [MAX_PATH];
    sprintf (fname_s, "%ls", fname.c_str ());

    //
    // If the config file cannot be opened, then create a new one.
    //
    if (! (fCFG = fopen (fname_s, "r"))) {
      DeleteFile (fname.c_str ());

      TRY_FILE_IO ( fopen_s (&fCFG, fname_s, "a"),
        fname.c_str (),
        err );

      if (err == 0 && fCFG != nullptr) {
        fprintf (fCFG, "resolution_X=%d\n"
          "resolution_Y=%d\n"
          "refreshRateHz=%d\n"
          "fullscreen=%d\n"
          "vsync=%d\n"
          "%hs",
          res_x, res_y, refresh, fullscreen, vsync,
          everything_else);
        fclose (fCFG);
      }
    }
    else {
      fclose (fCFG);
    }
  }
} cfg;

void
TZT_InitializeConfig (void)
{
  DEVMODE dmNow;

  memset (&dmNow, 0, sizeof (DEVMODE));
  dmNow.dmSize = sizeof (DEVMODE);

  EnumDisplaySettings (NULL, ENUM_CURRENT_SETTINGS, &dmNow);

  cfg.fullscreen = 0;
  cfg.vsync      = 1;
  cfg.res_x      = dmNow.dmPelsWidth;
  cfg.res_y      = dmNow.dmPelsHeight;
  cfg.refresh    = dmNow.dmDisplayFrequency;

  cfg.init ();
}

std::wstring
TZT_GetSteamDir (void)
{
  DWORD len = MAX_PATH;
  wchar_t wszSteamPath [MAX_PATH];

  RegGetValueW ( HKEY_CURRENT_USER,
                   L"SOFTWARE\\Valve\\Steam\\",
                     L"SteamPath",
                       RRF_RT_REG_SZ,
                         NULL,
                           wszSteamPath,
                             (LPDWORD)&len );

  return wszSteamPath;
}

std::wstring
TZT_GetSteamExecutable (void)
{
  return TZT_GetSteamDir () + L"\\Steam.exe";
}

std::wstring
TZT_GetSteamUIDLL (void)
{
  std::wstring steam_path =
    TZT_GetSteamDir ();

  steam_path += L"\\SteamUI.dll";

  return steam_path;
}

std::wstring
TZT_GetSteamUserDataDir (void)
{
  std::wstring steam_path =
    TZT_GetSteamDir ();

  DWORD len = wcslen (TZT_GetSteamDir ().c_str ());
  wchar_t wszSteamPath [MAX_PATH] = { L'\0' };

  wcsncpy (wszSteamPath, steam_path.c_str (), len);

  WIN32_FIND_DATA find_data;
  HANDLE          hFind;

  for (int i = 0; i < len; i++) {
    if (wszSteamPath [i] == L'/')
      wszSteamPath [i] = L'\\';
  }

  wchar_t wszPath [MAX_PATH];
  wsprintf (wszPath, L"%s\\userdata\\*", wszSteamPath);

  if (hFind = FindFirstFile (wszPath, &find_data)) {
    do
    {
      if (wcslen (find_data.cFileName) > 2) {
        std::wstring ret (wszSteamPath);
        ret += L"\\userdata\\";
        ret += find_data.cFileName;
        return ret;
      }
    } while (FindNextFile (hFind, &find_data));
    FindClose (hFind);
  }

  return L"<No Such Directory>";
}