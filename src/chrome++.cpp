#include <windows.h>
#include <stdio.h>
#include <psapi.h>

HMODULE hInstance;

#define MAGIC_CODE 0x1603ABD9

#include "MinHook.h"
#include "version.h"

#include "hijack.h"
#include "utils.h"
#include "patch.h"
#include "TabBookmark.h"
#include "portable.h"
#include "PakPatch.h"
#include "admin.h"
#include "appid.h"
#include "green.h"

typedef int(*Startup) ();
Startup ExeMain = NULL;

void ChromePlus()
{
	//if (IsNeedPortable())
	{
		// ��ݷ�ʽ
		//LockLnk();
		SetAppId();

		//
		MakeGreen();
	}

    // ��ǩҳ����ǩ����ַ����ǿ
    TabBookmark();

    // ��pak�ļ��򲹶�
    PakPatch();

	// ȥ������Ա����
	//RemoveAdminWarn();
}

void ChromePlusCommand(LPWSTR param)
{
    if (!wcsstr(param, L"--shuax"))
    {
        Portable(param);
    }
    else
    {
        ChromePlus();
    }
}

int Loader()
{
    // �ƽ�Flash���Ƴ������߾���
	MakePatch();

    // ֻ��ע������
    LPWSTR param = GetCommandLineW();
    //DebugLog(L"param %s", param);
    if (!wcsstr(param, L"-type="))
    {
        ChromePlusCommand(param);
    }

    //���ص�������
    return ExeMain();
}


void InstallLoader()
{
    //��ȡ������ڵ�
    MODULEINFO mi;
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &mi, sizeof(MODULEINFO));
    PBYTE entry = (PBYTE)mi.EntryPoint;

    // ��ڵ���ת��Loader
    MH_STATUS status = MH_CreateHook(entry, Loader, (LPVOID*)&ExeMain);
    if (status == MH_OK)
    {
        MH_EnableHook(entry);
    }
    else
    {
        DebugLog(L"MH_CreateHook InstallLoader failed:%d", status);
    }
}
#define EXTERNC extern "C"

EXTERNC __declspec(dllexport) void shuax()
{
}

EXTERNC BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        hInstance = hModule;

        // ����ϵͳdllԭ�й���
        LoadSysDll(hModule);

        //UnlinkModuleFromPEB(hModule);
        //RemovePeHeader(hModule);

        //if (!check_exe_crc32()) return true;

        //if (!check_txt_crc32()) return true;

        // ��ʼ��HOOK��ɹ��Ժ�װ������
        MH_STATUS status = MH_Initialize();
        if (status == MH_OK)
        {
            InstallLoader();
        }
        else
        {
            DebugLog(L"MH_Initialize failed:%d", status);
        }
    }
    return TRUE;
}
