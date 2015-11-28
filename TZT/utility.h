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

#endif /* __TZT__UTILITY_H__ */