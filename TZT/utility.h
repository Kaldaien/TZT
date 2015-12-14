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

#ifndef __TZT__UTILITY_H__
#define __TZT__UTILITY_H__

#include <string>

std::wstring  TZT_GetDocumentsDir      (void);
bool          TZT_GetUserProfileDir    (wchar_t* buf, uint32_t* pdwLen);
std::wstring  TZT_GetLocalAppDataDir   (void);
std::wstring  TZT_GetSteamDir          (void);
std::wstring  TZT_GetSteamExecutable   (void);
std::wstring  TZT_GetSteamUIDLL        (void);
std::wstring  TZT_GetSteamUserDataDir  (void);
std::wstring  TZT_GetRTSSInstallDir    (void);
bool          TZT_IsTrue               (const wchar_t* string);
bool          TZT_IsAdmin              (void);
int           TZT_MessageBox           (std::wstring caption, std::wstring title, uint32_t flags);

void          TZT_InitializeConfig     (void);
void          TZT_DeleteAllConfigFiles (void);

bool          TZT_HasBackupConfigFiles (void);
std::wstring  TZT_GetConfigFileTime    (void);
void          TZT_CreateBackupConfig   (void);
void          TZT_RestoreConfigFiles   (void);
void          TZT_SetNormalFileAttribs (std::wstring file);

std::wstring  TZT_GetBackupFileTime    (void);
std::wstring  TZT_GetConfigFileTime    (void);


#include <combaseapi.h>

#pragma comment (lib, "Shlwapi.lib")

#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>

#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC

class CDialogEventHandler : public IFileDialogEvents,
  public IFileDialogControlEvents
{
public:
  CDialogEventHandler (void) : _cRef (1) { };

  // IUnknown methods
  IFACEMETHODIMP QueryInterface (REFIID riid, void** ppv)
  {
    static const QITAB qit [] = {
                 QITABENT (CDialogEventHandler, IFileDialogEvents),
                 QITABENT (CDialogEventHandler, IFileDialogControlEvents),
                 { 0 },
    };
    return QISearch (this, qit, riid, ppv);
  }

  IFACEMETHODIMP_(ULONG) AddRef (void)
  {
    return InterlockedIncrement (&_cRef);
  }

  IFACEMETHODIMP_(ULONG) Release (void)
  {
    long cRef = InterlockedDecrement (&_cRef);

    if (!cRef)
      delete this;

    return cRef;
  }

  // IFileDialogEvents methods
  IFACEMETHODIMP OnFileOk          (IFileDialog  *)    { return S_OK; };
  IFACEMETHODIMP OnFolderChange    (IFileDialog  *)    { return S_OK; };
  IFACEMETHODIMP OnFolderChanging  ( IFileDialog *,
                                     IShellItem  * )   { return S_OK; };
  IFACEMETHODIMP OnHelp            (IFileDialog  *)    { return S_OK; };
  IFACEMETHODIMP OnSelectionChange (IFileDialog  *)    { return S_OK; };
  IFACEMETHODIMP OnShareViolation  ( IFileDialog *,
                                     IShellItem  *,
                     FDE_SHAREVIOLATION_RESPONSE * )   { return S_OK; };
  IFACEMETHODIMP OnTypeChange      (IFileDialog  *pfd) { return S_OK; };
  IFACEMETHODIMP OnOverwrite       (IFileDialog  *,
                                    IShellItem   *,
                          FDE_OVERWRITE_RESPONSE * )   { return S_OK; };

  // IFileDialogControlEvents methods
  IFACEMETHODIMP OnItemSelected       ( IFileDialogCustomize *pfdc, DWORD dwIDCtl,
                                        DWORD                 dwIDItem)    { return S_OK; };
  IFACEMETHODIMP OnButtonClicked      (IFileDialogCustomize  *,     DWORD) { return S_OK; };
  IFACEMETHODIMP OnCheckButtonToggled ( IFileDialogCustomize *,     DWORD,
                                        BOOL)                              { return S_OK; };
  IFACEMETHODIMP OnControlActivating  (IFileDialogCustomize *,     DWORD)  { return S_OK; };

private:
  ~CDialogEventHandler (void) { };
  long _cRef;
};

// Instance creation helper
HRESULT CDialogEventHandler_CreateInstance (REFIID riid, void **ppv);

#endif /* __TZT__UTILITY_H__ */