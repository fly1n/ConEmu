﻿
/*
Copyright (c) 2009-present Maximus5
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#pragma once

#define WM_TRAYNOTIFY WM_USER+1

#include "MenuIds.h"

#if !defined(IID_IShellLink)
#define IID_IShellLink IID_IShellLinkW
#endif

#define GET_X_LPARAM(inPx) ((int)(short)LOWORD(inPx))
#define GET_Y_LPARAM(inPy) ((int)(short)HIWORD(inPy))

#include <intrin.h>

//#define RCLICKAPPSTIMEOUT 600
//#define RCLICKAPPS_START 100 // начало отрисовки кружка вокруг курсора
//#define RCLICKAPPSTIMEOUT_MAX 10000
//#define RCLICKAPPSDELTA 3
//#define DRAG_DELTA 5

//typedef DWORD (WINAPI* FGetModuleFileNameEx)(HANDLE hProcess,HMODULE hModule,LPWSTR lpFilename,DWORD nSize);

//typedef HRESULT(WINAPI* FDwmIsCompositionEnabled)(BOOL *pfEnabled);

class CAttachDlg;
class CConEmuBack;
class CConEmuChild;
class CConEmuInside;
class CConEmuMacro;
class CConEmuMenu;
class CDefaultTerminal;
class CGestures;
class CRecreateDlg;
class CRunQueue;
class CStatus;
class CTabBarClass;
class CToolTip;
class CVConGroup;
class CVConGuard;
class MFileLogEx;
struct MSectionSimple;
struct MSectionLockSimple;
enum ConEmuWindowMode;
struct CEFindDlg;
union CESize;
class CPushInfo;
class CAltNumpad;
struct HandleMonitor;

struct ConsoleInfoArg
{
	CONSOLE_SCREEN_BUFFER_INFO sbi;
	SMALL_RECT srRealWindow;
	COORD crCursor;
	CONSOLE_CURSOR_INFO cInfo;
	TOPLEFTCOORD TopLeft;
};

#include "DwmHelper.h"
#include "TaskBar.h"
#include "FrameHolder.h"
#include "GuiServer.h"
#include "GestureEngine.h"
#include "ConEmuCtrl.h"
#include "ConEmuSize.h"
#include "ConEmuStart.h"
#include "../common/MArray.h"
#include "../common/MMap.h"
#include "../common/MFileMapping.h"

// IME support (WinXP or later)
typedef BOOL (WINAPI* ImmSetCompositionFontW_t)(HIMC hIMC, LPLOGFONT lplf);
typedef BOOL (WINAPI* ImmSetCompositionWindow_t)(HIMC hIMC, LPCOMPOSITIONFORM lpCompForm);
typedef HIMC (WINAPI* ImmGetContext_t)(HWND hWnd);


//struct GuiShellExecuteExArg
//{
//	CVirtualConsole* pVCon;
//	SHELLEXECUTEINFO* lpShellExecute;
//	HANDLE hReadyEvent;
//	BOOL bInProcess;
//	BOOL bResult;
//	DWORD dwErrCode;
//};

typedef LRESULT (*CallMainThreadFn)(LPARAM lParam);
struct CallMainThreadArg;


typedef DWORD ConEmuInstallMode;
const ConEmuInstallMode
	cm_Normal       = 0x0000,
	cm_MinGW        = 0x0001, // ConEmu was installed as MinGW package
	cm_PortableApps = 0x0002, // ConEmu was installed as PortableApps.com package
	cm_GitForWin    = 0x0004, // ConEmu was installed as ‘Git-For-Windows’ package (implies cm_MinGW)
	cm_MSysStartup  = 0x1000  // the bash from msys was found: "%ConEmuDir%\..\msys\1.0\bin\bash.exe" (MinGW mode)
	;


class CConEmuMain
	: public CDwmHelper
	, public CTaskBar
	, public CFrameHolder
	, public CGestures
	, public CConEmuCtrl
	, public CConEmuSize
	, public CConEmuStart
{
	public:
		wchar_t ms_ConEmuDefTitle[32] = L"";          // Full title with build number: "ConEmu 110117 [32]"
		wchar_t ms_ConEmuBuild[16] = L"";             // Build number only: "110117" or "131129dbg"
		wchar_t ms_ConEmuExe[MAX_PATH+1] = L"";       // FULL PATH with NAME to the ConEmu.exe (GUI): "C:\Tools\ConEmu.exe"
		wchar_t ms_ConEmuExeDir[MAX_PATH+1] = L"";    // WITHOUT trailing slash. The folder containing ConEmu.exe
		wchar_t ms_ConEmuBaseDir[MAX_PATH+1] = L"";   // WITHOUT trailing slash. The folder containing ConEmuC.exe, ConEmuCD.dll, ConEmuHk.dll
		wchar_t ms_ConEmuWorkDir[MAX_PATH+1] = L"";   // WITHOUT trailing slash. Startup folder of ConEmu.exe (GetCurrentDirectory)
		wchar_t ms_AppID[40] = L"";                   // Generated in ::UpdateAppUserModelID
		void SetAppID(LPCWSTR asExtraArgs);
		bool mb_ConEmuWorkDirArg = false;             // Was started as "ConEmu.exe /Dir C:\abc ..."; may be overridden with "/dir" switch in task parameter
		void StoreWorkDir(LPCWSTR asNewCurDir = NULL);
		LPCWSTR WorkDir(LPCWSTR asOverrideCurDir = NULL);
		bool ChangeWorkDir(LPCWSTR asTempCurDir);
		private:
		LPWSTR  mps_ConEmuExtraArgs = NULL;            // Used with TaskBar jump list creation (/FontDir, /FontFile, etc.)
		public:
		void AppendExtraArgs(LPCWSTR asSwitch, LPCWSTR asSwitchValue = NULL);
		LPCWSTR MakeConEmuStartArgs(CEStr& rsArgs, LPCWSTR asOtherConfig = NULL);
		wchar_t ms_ComSpecInitial[MAX_PATH] = L"";
		CEStr ms_PostGuiMacro;
		void SetPostGuiMacro(LPCWSTR asGuiMacro);
		CEStr ms_PostRConMacro;
		void AddPostGuiRConMacro(LPCWSTR asGuiMacro);
		void ExecPostGuiMacro();
		wchar_t *mps_IconPath = nullptr;
		HICON mh_TaskbarIcon = NULL;
		void SetWindowIcon(LPCWSTR asNewIcon);
		void SetTaskbarIcon(HICON ahNewIcon);
		CPushInfo *mp_PushInfo = nullptr;
		BOOL mb_DosBoxExists = FALSE;
		ConEmuInstallMode m_InstallMode = cm_Normal;
		bool isMingwMode();
		bool isMSysStartup();
		bool isUpdateAllowed();
		bool mb_UpdateJumpListOnStartup = false;
		bool mb_FindBugMode = false;
		UINT mn_LastTransparentValue = 255;
	private:
		struct
		{
			bool  bBlockChildrenDebuggers;
		} m_DbgInfo;
	private:
		bool CheckBaseDir();
		BOOL CheckDosBoxExists();
		void FinalizePortableReg();
		bool mb_ForceUseRegistry = false;
		bool mb_SpecialConfigPath = false;
		wchar_t ms_ConEmuXml[MAX_PATH+1] = L"";       // полный путь к портабельным настройкам
		wchar_t ms_ConEmuIni[MAX_PATH+1] = L"";       // полный путь к портабельным настройкам
	public:
		bool SetConfigFile(LPCWSTR asFilePath, bool abWriteReq = false, bool abSpecialPath = false);
		void SetForceUseRegistry();
		LPCWSTR FindConEmuXml(CEStr& szXmlFile);
		LPWSTR ConEmuXml(bool* pbSpecialPath = NULL);
		LPWSTR ConEmuIni();
		wchar_t ms_ConEmuChm[MAX_PATH+1] = L"";       // полный путь к chm-файлу (help)
		wchar_t ms_ConEmuC32Full[MAX_PATH+12] = L"";  // полный путь к серверу (ConEmuC.exe) с длинными именами
		wchar_t ms_ConEmuC64Full[MAX_PATH+12] = L"";  // полный путь к серверу (ConEmuC64.exe) с длинными именами
		LPCWSTR ConEmuCExeFull(LPCWSTR asCmdLine=NULL);
		//wchar_t *mpsz_ConEmuArgs;    // Use opt.cmdLine, opt.cfgSwitches and opt.runCommand instead
		void GetComSpecCopy(ConEmuComspec& ComSpec);
		void CreateGuiAttachMapping(DWORD nGuiAppPID);
		void InitComSpecStr(ConEmuComspec& ComSpec);
		bool IsResetBasicSettings();
		bool IsFastSetupDisabled();
		bool IsAllowSaveSettingsOnExit();
	private:
		ConEmuGuiMapping m_GuiInfo = {sizeof(ConEmuGuiMapping)};
		MFileMapping<ConEmuGuiMapping> m_GuiInfoMapping;
		MFileMapping<ConEmuGuiMapping> m_GuiAttachMapping;
	public:
		void GetGuiInfo(ConEmuGuiMapping& GuiInfo);
	private:
		void FillConEmuMainFont(ConEmuMainFont* pFont);
		void UpdateGuiInfoMapping();
		void UpdateGuiInfoMappingActive(bool bActive, bool bUpdatePtr = true);
		bool mb_LastTransparentFocused = false; // required for check gpSet->isTransparentSeparate
	public:
		bool InCreateWindow();
		bool InQuakeAnimation();
		UINT IsQuakeVisible();
	protected:
		struct WindowsOverQuake
		{
			RECT rcWnd;
			HRGN hRgn;
			int  iRc;
		};
		static BOOL CALLBACK EnumWindowsOverQuake(HWND hWnd, LPARAM lpData);
	public:
		//CConEmuChild *m_Child;
		//CConEmuBack  *m_Back;
		//CConEmuMacro *m_Macro;
		CConEmuMenu *mp_Menu;
		CTabBarClass *mp_TabBar;
		CConEmuInside *mp_Inside = nullptr;
		CStatus *mp_Status;
		CToolTip *mp_Tip = nullptr;
		MFileLogEx *mp_Log = nullptr;
		MSectionSimple* mpcs_Log; // mcs_Log - для создания
		CDefaultTerminal *mp_DefTrm;
		CEFindDlg *mp_Find;
		CRunQueue *mp_RunQueue;
		HandleMonitor *mp_HandleMonitor = nullptr;

		bool CreateLog();
		bool LogString(LPCWSTR asInfo, bool abWriteTime = true, bool abWriteLine = true);
		bool LogString(LPCSTR asInfo, bool abWriteTime = true, bool abWriteLine = true);
		bool LogMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool LogWindowPos(LPCWSTR asPrefix, LPRECT prcWnd = NULL);

	public:
		bool  WindowStartMinimized = false; // ключик "/min" или "Свернуть" в свойствах ярлыка
		bool  WindowStartTsa = false;       // ключики "/StartTSA" или "/MinTSA"
		bool  WindowStartNoClose = false;   // ключик "/MinTSA"
		bool  ForceMinimizeToTray = false;  // ключики "/tsa" или "/tray"
		bool  DisableKeybHooks = false;     // ключик "/nokeyhook"
		bool  DisableAllMacro = false;      // ключик "/nomacro"
		bool  DisableAllHotkeys = false;    // ключик "/nohotkey"
		bool  DisableSetDefTerm = false;    // ключик "/nodeftrm"
		bool  DisableRegisterFonts = false; // ключик "/noregfont"
		bool  DisableCloseConfirm = false;  // ключик "/nocloseconfirm"
		bool  SilentMacroClose = false;     // closing from GuiMacro (no confirmations at all)

		bool  mb_ExternalHidden = false;

		SYSTEMTIME mst_LastConsole32StartTime = {}, mst_LastConsole64StartTime = {};

		enum StartupStage {
			ss_Starting,
			ss_OnCreateCalled,
			ss_OnCreateFinished,
			ss_PostCreate1Called,
			ss_PostCreate1Finished,
			ss_PostCreate2Called,
			ss_VConAreCreated,
			ss_CreateQueueReady,
			ss_PostCreate2Finished,
			ss_Started = ss_PostCreate2Finished,
			ss_VConStarted,
			ss_Destroying,
			ss_Destroyed,
		};
	private:
		StartupStage mn_StartupFinished = ss_Starting;
	public:
		StartupStage GetStartupStage() const;

		struct
		{
			WORD  state;
			bool  bSkipRDblClk;
			bool  bIgnoreMouseMove;
			bool  bCheckNormalRect; // call StoreNormalRect after resize in main timer

			COORD LClkDC, LClkCon;
			POINT LDblClkDC; // заполняется в PatchMouseEvent
			DWORD LDblClkTick;
			COORD RClkDC, RClkCon;
			DWORD RClkTick;

			// Для обработки gpSet->isActivateSplitMouseOver
			POINT  ptLastSplitOverCheck;

			// Чтобы не слать в консоль бесконечные WM_MOUSEMOVE
			UINT   lastMsg;
			WPARAM lastMMW = (WPARAM)-1;
			LPARAM lastMML = (LPARAM)-1;

			// Пропустить клик мышкой (окно было неактивно)
			UINT nSkipEvents[2]; UINT nReplaceDblClk;
			// Activated by touch, skip next "activation" emulated by real mouse click
			BOOL bTouchActivation;
			// не пропускать следующий клик в консоль!
			BOOL bForceSkipActivation;

			// таскание окошка за клиентскую область
			POINT ptWndDragStart;
			RECT  rcWndDragStart;

			// настройки скролла мышкой (сколько линий/символов "за клик")
			UINT nWheelScrollChars, nWheelScrollLines;
			void  ReloadWheelScroll()
			{
				#ifndef SPI_GETWHEELSCROLLCHARS
				#define SPI_GETWHEELSCROLLCHARS   0x006C
				#endif
				if (!SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &nWheelScrollChars, 0) || !nWheelScrollChars)
					nWheelScrollChars = 3;
				if (!SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nWheelScrollLines, 0) || !nWheelScrollLines)
					nWheelScrollLines = 3;
			};
			UINT GetWheelScrollChars()
			{
				if (!nWheelScrollChars)
					ReloadWheelScroll();
				_ASSERTE(nWheelScrollChars<=3);
				return nWheelScrollChars;
			};
			UINT GetWheelScrollLines()
			{
				if (!nWheelScrollLines)
					ReloadWheelScroll();
				return nWheelScrollLines;
			};
		} mouse = {};
		struct SessionInfo
		{
			HMODULE hWtsApi;
			WPARAM  wState;     // session state change event
			LPARAM  lSessionID; // session ID

			typedef BOOL (WINAPI* WTSRegisterSessionNotification_t)(HWND hWnd, DWORD dwFlags);
			WTSRegisterSessionNotification_t pfnRegister;
			typedef BOOL (WINAPI* WTSUnRegisterSessionNotification_t)(HWND hWnd);
			WTSUnRegisterSessionNotification_t pfnUnregister;

			#define SESSION_LOG_SIZE 128
			struct EvtLog {
				DWORD  nTick;
				DWORD  wState;     // session state change event
				LPARAM lSessionID; // session ID
			} g_evt[SESSION_LOG_SIZE];
			LONG g_evtidx;
			void Log(WPARAM State, LPARAM SessionID);

			BOOL bWasLocked; // To be sure that mn_IgnoreSizeChange will be processed properly

			bool Connected();

			void SessionChanged(WPARAM State, LPARAM SessionID);

			void SetSessionNotification(bool bSwitch);
		} session = {};
		struct DpiInfo
		{
			// Win 8.1: shcore.dll
			enum Monitor_DPI_Type { MDT_Effective_DPI = 0, MDT_Angular_DPI = 1, MDT_Raw_DPI = 2, MDT_Default = MDT_Effective_DPI };
			typedef HRESULT (WINAPI* GetDPIForMonitor_t)(/*_In_*/HMONITOR hmonitor, /*_In_*/Monitor_DPI_Type dpiType, /*_Out_*/UINT *dpiX, /*_Out_*/UINT *dpiY);
			enum Process_DPI_Awareness { Process_DPI_Unaware = 0, Process_System_DPI_Aware = 1, Process_Per_Monitor_DPI_Aware = 2 };
			typedef HRESULT (WINAPI* SetProcessDPIAwareness_t)(/*_In_*/Process_DPI_Awareness value);
		} dpi;
		bool isPiewUpdate = false;
		HWND hPictureView = NULL; bool bPicViewSlideShow = false; DWORD dwLastSlideShowTick = 0; RECT mrc_WndPosOnPicView = {};
		HWND mh_ShellWindow = NULL; // The Progman window for "Desktop" mode
		DWORD mn_ShellWindowPID = 0;
		BOOL mb_FocusOnDesktop = TRUE;
		POINT cursor = {}, Rcursor = {};
		bool mb_InCaptionChange = false;
		DWORD m_FixPosAfterStyle = 0;
		RECT mrc_FixPosAfterStyle = {};
		DWORD m_ProcCount = 0;
		bool mb_IsUacAdmin = false; // ConEmu itself is started elevated
		bool IsActiveConAdmin();
		HICON GetCurrentVConIcon();
		HCURSOR mh_CursorWait, mh_CursorArrow, mh_CursorAppStarting, mh_CursorMove, mh_CursorIBeam;
		HCURSOR mh_SplitV = nullptr, mh_SplitV2 = nullptr, mh_SplitH = nullptr;
		HCURSOR mh_DragCursor = NULL;
		CDragDrop *mp_DragDrop = nullptr;
		// TODO: ==>> m_Foreground
		bool mb_SkipOnFocus = false;
		bool mb_LastConEmuFocusState = false;
		DWORD mn_ForceTimerCheckLoseFocus = 0; // GetTickCount()
		bool mb_AllowAutoChildFocus = false;
	public:
		void OnOurDialogOpened();
		void OnOurDialogClosed();
		void CheckAllowAutoChildFocus(DWORD nDeactivatedTID);
		bool isMenuActive();
		bool CanSetChildFocus();
		void SetScClosePending(bool bFlag);
		void PostScClose();
		bool OnScClose();
		bool isScClosing();
		BYTE CloseConfirmFlags(); // CloseConfirmOptions
	protected:
		bool mb_ScClosePending = false; // Set into TRUE from CVConGroup::CloseQuery
	protected:
		friend class CVConGroup;

		friend class CGuiServer;
		CGuiServer m_GuiServer;

		TCHAR Title[MAX_TITLE_SIZE+192] = L"";
		TCHAR TitleTemplate[128] = L"";
		short mn_Progress = -1;
		bool mb_InTimer = false;
		bool mb_ProcessCreated = false;
		bool mb_WorkspaceErasedOnClose = false;
		#ifndef _WIN64
		HWINEVENTHOOK mh_WinHook = NULL;
		static VOID CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD anEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
		#endif
		bool mb_ShellHookRegistered = false;
		CAttachDlg *mp_AttachDlg = nullptr;
		CRecreateDlg *mp_RecreateDlg = nullptr;
		BOOL mb_WaitCursor = false;
		struct
		{
			BOOL  bChecked;
			DWORD nReadyToSelNoEmpty;
			DWORD nReadyToSel;
		} m_Pressed = {};
	public:
		void OnTabbarActivated(bool bTabbarVisible, bool bInAutoShowHide);
	public:
		bool isInputGrouped() const;
	protected:
		bool mb_GroupInputFlag = false;
	protected:
		BOOL mb_MouseCaptured = false;
		void CheckActiveLayoutName();
		void AppendHKL(wchar_t* szInfo, size_t cchInfoMax, HKL* hKeyb, int nCount);
		void AppendRegisteredLayouts(wchar_t* szInfo, size_t cchInfoMax);
		void StoreLayoutName(int iIdx, DWORD dwLayout, HKL hkl);
		CAltNumpad* mp_AltNumpad = nullptr; // Alt+Numpad ==> wchar sequence
		DWORD_PTR m_ActiveKeybLayout = 0;
		struct {
			UINT   nLastLngMsg;
			LPARAM lLastLngPrm;
			// #KEYBOARD Add here Ctr/Alt/Shift/Menu/etc. states in Message processing cycle to reuse them in hotkeys processing
		} m_KeyboardState = {};
		struct LayoutNames
		{
			BOOL      bUsed;
			DWORD     klName;
			DWORD_PTR hkl;
		} m_LayoutNames[20] = {};
		struct TranslatedCharacters
		{
			wchar_t szTranslatedChars[16];
		} m_TranslatedChars[256] = {};
		BYTE mn_LastPressedVK = 0;
		bool mb_InImeComposition = false, mb_ImeMethodChanged = false;
		wchar_t ms_ConEmuAliveEvent[MAX_PATH] = L"";
		bool mb_AliveInitialized = false;
		HANDLE mh_ConEmuAliveEvent = NULL; bool mb_ConEmuAliveOwned = false; DWORD mn_ConEmuAliveEventErr = 0;
		HANDLE mh_ConEmuAliveEventNoDir = NULL; bool mb_ConEmuAliveOwnedNoDir = false; DWORD mn_ConEmuAliveEventErrNoDir = 0;
		//
		bool mb_HotKeyRegistered = false;
		HHOOK mh_LLKeyHook = NULL;
		HMODULE mh_LLKeyHookDll = NULL;
		HWND* mph_HookedGhostWnd = nullptr;
		HMODULE LoadConEmuCD();
		void RegisterHotKeys();
		void RegisterGlobalHotKeys(bool bRegister);
	public:
		void GlobalHotKeyChanged();
	protected:
		void UnRegisterHotKeys(bool abFinal=false);
		HBITMAP mh_RightClickingBmp = NULL; HDC mh_RightClickingDC = NULL;
		POINT m_RightClickingSize = {}; // {384 x 16} 24 фрейма, считаем, что четверть отведенного времени прошла до начала показа
		int m_RightClickingFrames = 0, m_RightClickingCurrent = -1;
		bool mb_RightClickingPaint = false, mb_RightClickingLSent = false, mb_RightClickingRegistered = false;
		void StartRightClickingPaint();
		void StopRightClickingPaint();
		static LRESULT CALLBACK RightClickingProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
		HWND mh_RightClickingWnd = NULL;
		bool PatchMouseEvent(UINT messg, POINT& ptCurClient, POINT& ptCurScreen, WPARAM wParam, bool& isPrivate);
	public:
		wchar_t* LoadConsoleBatch(LPCWSTR asSource, RConStartArgsEx* pArgs = NULL);
		static wchar_t IsConsoleBatchOrTask(LPCWSTR asSource);
	private:
		wchar_t* LoadConsoleBatch_File(LPCWSTR asSource);
		wchar_t* LoadConsoleBatch_Drops(LPCWSTR asSource);
		wchar_t* LoadConsoleBatch_Task(LPCWSTR asSource, RConStartArgsEx* pArgs = NULL);
	public:
		void RightClickingPaint(HDC hdcIntVCon, CVirtualConsole* apVCon);
		void RegisterMinRestore(bool abRegister);
		bool IsKeyboardHookRegistered();
		void RegisterHooks();
		void UnRegisterHooks(bool abFinal=false);
		void OnWmHotkey(WPARAM wParam, DWORD nTime = 0);
		void UpdateWinHookSettings();
		void CtrlWinAltSpace();
		void DeleteVConMainThread(CVirtualConsole* apVCon);
		UINT GetRegisteredMessage(LPCSTR asLocal, LPCWSTR asGlobal = NULL);
		LRESULT CallMainThread(bool bSync, CallMainThreadFn fn, LPARAM lParam);
	private:
		UINT RegisterMessage(LPCSTR asLocal, LPCWSTR asGlobal = NULL);
		void RegisterMessages();
	protected:
		friend class CConEmuCtrl;
		friend class CRunQueue;
		friend class CConEmuSize;
		DWORD mn_MainThreadId;
		// Registered messages
		UINT mn__FirstAppMsg;
		MMap<UINT,LPCSTR> m__AppMsgs;
		UINT mn_MsgPostCreate;
		UINT mn_MsgPostCopy;
		UINT mn_MsgMyDestroy;
		UINT mn_MsgUpdateSizes;
		UINT mn_MsgUpdateCursorInfo;
		UINT mn_MsgSetWindowMode;
		UINT mn_MsgUpdateTitle;
		UINT mn_MsgUpdateScrollInfo;
		UINT mn_MsgUpdateTabs; LONG mn_ReqMsgUpdateTabs; // = RegisterWindowMessage(CONEMUMSG_UPDATETABS);
		UINT mn_MsgOldCmdVer; BOOL mb_InShowOldCmdVersion;
		UINT mn_MsgTabCommand;
		UINT mn_MsgTabSwitchFromHook; /*BOOL mb_InWinTabSwitch;*/ // = RegisterWindowMessage(CONEMUMSG_SWITCHCON);
		UINT mn_MsgWinKeyFromHook;
		UINT mn_MsgSheelHook;
		UINT mn_PostConsoleResize;
		UINT mn_ConsoleLangChanged;
		UINT mn_MsgPostOnBufferHeight;
		UINT mn_MsgPostAltF9;
		UINT mn_MsgInitInactiveDC;
		UINT mn_MsgFlashWindow; // = RegisterWindowMessage(CONEMUMSG_FLASHWINDOW);
		UINT mn_MsgUpdateProcDisplay;
		UINT mn_MsgFontSetSize;
		UINT mn_MsgDisplayRConError;
		UINT mn_MsgMacroFontSetName;
		UINT mn_MsgCreateViewWindow;
		UINT mn_MsgPostTaskbarActivate; BOOL mb_PostTaskbarActivate;
		UINT mn_MsgInitVConGhost;
		UINT mn_MsgTaskBarCreated;
		UINT mn_MsgPanelViewMapCoord;
		UINT mn_MsgTaskBarBtnCreated;
		UINT mn_MsgDeleteVConMainThread;
		UINT mn_MsgReqChangeCurPalette;
		UINT mn_MsgMacroExecSync;
		UINT mn_MsgActivateVCon;
		UINT mn_MsgPostScClose;
		UINT mn_MsgOurSysCommand;
		UINT mn_MsgCallMainThread;

		//
		virtual void OnUseGlass(bool abEnableGlass) override;
		virtual void OnUseTheming(bool abEnableTheming) override;
		virtual void OnUseDwm(bool abEnableDwm) override;

		bool ExecuteProcessPrepare();
		void ExecuteProcessFinished(bool bOpt);

	public:
		DWORD CheckProcesses();
		DWORD GetFarPID(bool abPluginRequired=false);

	public:
		LPCWSTR GetDefaultTitle(); // вернуть ms_ConEmuDefTitle
		LPCWSTR GetDefaultTabLabel(); // L"ConEmu"
		LPCTSTR GetLastTitle(bool abUseDefault=true);
		LPCTSTR GetVConTitle(int nIdx);
		void SetTitle(HWND ahWnd, LPCWSTR asTitle, bool abTrySync = false);
		void SetTitleTemplate(LPCWSTR asTemplate);
		int GetActiveVCon(CVConGuard* pVCon = NULL, int* pAllCount = NULL);
		int isVConValid(CVirtualConsole* apVCon);
		void UpdateCaretPos(CVirtualConsole& vcon, const RECT& rect);
		void UpdateCursorInfo(const ConsoleInfoArg* pInfo);
		void UpdateProcessDisplay(bool abForce);
		void UpdateSizes();

	public:
		CConEmuMain();
		virtual ~CConEmuMain();

	public:
		BOOL Activate(CVirtualConsole* apVCon);
		int ActiveConNum(); // 0-based
		int GetConCount(); // количество открытых консолей
		void AskChangeBufferHeight();
		void AskChangeAlternative();
		void AttachToDialog();
		void CheckFocus(LPCWSTR asFrom);
		bool CheckRequiredFiles();
		bool CheckUpdates(UINT abShowMessages);
		DWORD isSelectionModifierPressed(bool bAllowEmpty);
		void ForceSelectionModifierPressed(DWORD nValue);
		enum DragPanelBorder CheckPanelDrag(COORD crCon);
		bool ConActivate(int nCon);
		bool ConActivateByName(LPCWSTR asName);
		bool ConActivateNext(bool abNext);
		bool CreateWnd(RConStartArgsEx *args);
		CVirtualConsole* CreateCon(RConStartArgsEx *args, bool abAllowScripts = false, bool abForceCurConsole = false);
		CVirtualConsole* CreateConGroup(LPCWSTR apszScript, bool abForceAsAdmin = false, LPCWSTR asStartupDir = NULL, const RConStartArgsEx *apDefArgs = NULL);
		LPCWSTR ParseScriptLineOptions(LPCWSTR apszLine, bool* rpbSetActive, RConStartArgsEx* pArgs);
		void CreateGhostVCon(CVirtualConsole* apVCon);
		BOOL CreateMainWindow();
		BOOL CreateWorkWindow();
		void DebugStep(LPCWSTR asMsg, BOOL abErrorSeverity=FALSE);
		void ForceShowTabs(BOOL abShow);
		DWORD_PTR GetActiveKeyboardLayout();
		bool isTabsShown();

	public:
		void Destroy(bool abForce = false);
	private:
		#ifdef _DEBUG
		bool mb_DestroySkippedInAssert = false;
		#endif
		int mn_InOurDestroy = 0;
		void DestroyAllChildWindows();
		void DeinitOnDestroy(HWND hWnd, bool abForce = false);

	public:
		bool isInside();
		bool isInsideInvalid();

	public:
		enum {
			fgf_Background   = 0x0000,
			fgf_ConEmuMain   = 0x0001,
			fgf_ConEmuDialog = 0x0002,
			fgf_RealConsole  = 0x0004,
			fgf_InsideParent = 0x0008,
			fgf_ConEmuAny    = (fgf_ConEmuMain|fgf_RealConsole|fgf_ConEmuDialog|fgf_InsideParent),
		};
		bool isMeForeground(bool abRealAlso=false, bool abDialogsAlso=true, HWND* phFore=NULL);
	protected:
		struct {
			HWND  hLastFore;
			HWND  hLastInsideFocus;
			DWORD ForegroundState;
			DWORD nDefTermNonResponsive;
			DWORD nDefTermTick;
			bool  bCaretCreated;
		} m_Foreground = {};
		bool RecheckForegroundWindow(LPCWSTR asFrom, HWND* phFore = NULL, HWND hForcedForeground = NULL);

	public:
		DWORD GetWindowStyle();
		DWORD FixWindowStyle(DWORD dwExStyle, ConEmuWindowMode wmNewMode = wmCurrent);
		void SetWindowStyle(DWORD anStyle);
		void SetWindowStyle(HWND ahWnd, DWORD anStyle);
		DWORD GetWindowStyleEx();
		void SetWindowStyleEx(DWORD anStyleEx);
		void SetWindowStyleEx(HWND ahWnd, DWORD anStyleEx);
		DWORD GetWorkWindowStyle();
		DWORD GetWorkWindowStyleEx();
		HWND GetRootHWND();
		BOOL Init();
		void InitInactiveDC(CVirtualConsole* apVCon);
		void Invalidate(LPRECT lpRect, BOOL bErase = TRUE);
		void InvalidateFrame();
		void InvalidateAll();
		void UpdateWindowChild(CVirtualConsole* apVCon);
		bool isCloseConfirmed();
		bool isDestroyOnClose(bool ScCloseOnEmpty = false);
		bool isConSelectMode();
		bool isConsolePID(DWORD nPID);
		bool isDragging();
		bool isFirstInstance(bool bFolderIgnore = false);
		bool isInImeComposition();
		bool isLBDown();
		bool isMouseOverFrame(bool abReal=false);
		bool isNtvdm(BOOL abCheckAllConsoles=FALSE);
		bool isOurConsoleWindow(HWND hCon);
		bool isPictureView();
		bool isProcessCreated();
		bool isRightClickingPaint();
		bool isValid(CRealConsole* apRCon);
		bool isValid(CVirtualConsole* apVCon);
		bool isVConExists(int nIdx);
		bool isVConHWND(HWND hChild, CVConGuard* pVCon = NULL);
		bool isViewer();
		void LoadIcons();
		void MoveActiveTab(CVirtualConsole* apVCon, bool bLeftward);
		void InvalidateGaps();
		void PostDragCopy(bool abMove, bool abReceived = false);
		void PostCreate(bool abReceived = false);
	protected:
		void OnMainCreateFinished();
		bool CreateStartupConsoles();
	public:
		void PostCreateCon(RConStartArgsEx *pArgs);
		HWND PostCreateView(CConEmuChild* pChild);
		void PostFontSetSize(int nRelative/*0/1/2*/, int nValue/*для nRelative==0 - высота, для ==1 - +-1, +-2,... | 100%*/);
		void PostMacro(LPCWSTR asMacro);
		void PostMacroFontSetName(wchar_t* pszFontName, WORD anHeight /*= 0*/, WORD anWidth /*= 0*/, BOOL abPosted);
		void PostDisplayRConError(CRealConsole* apRCon, wchar_t* pszErrMsg);
		void PostChangeCurPalette(LPCWSTR pszPalette, bool bChangeDropDown, bool abPosted);
		LRESULT SyncExecMacro(WPARAM wParam, LPARAM lParam);
		bool RecreateAction(RecreateActionParm aRecreate, BOOL abConfirm, RConBoolArg bRunAs = crb_Undefined);
		int RecreateDlg(RConStartArgsEx* apArg, bool abDontAutoSelCmd = false);
		void RequestPostUpdateTabs();
		int RunSingleInstance(HWND hConEmuWnd = NULL, LPCWSTR apszCmd = NULL);
		void SetDragCursor(HCURSOR hCur);
		bool SetSkipOnFocus(bool abSkipOnFocus);
		void SetWaitCursor(BOOL abWait);
		void CheckNeedRepaint();
		void EnterAltNumpadMode(UINT nBase);
	public:
		void ReportOldCmdVersion(DWORD nCmd, DWORD nVersion, int bFromServer, DWORD nFromProcess, HANDLE hFromModule, DWORD nBits);
		bool SetParent(HWND hNewParent);
		void setFocus();
		bool StartDebugLogConsole();
		bool StartDebugActiveProcess();
		bool MemoryDumpActiveProcess(bool abProcessTree = false);
		void SyncNtvdm();
		void SwitchKeyboardLayout(DWORD_PTR dwNewKeybLayout);
		BOOL TrackMouse();
		void SetSkipMouseEvent(UINT nMsg1, UINT nMsg2, UINT nReplaceDblClk);
		void Update(bool isForce = false);
		void UpdateActiveGhost(CVirtualConsole* apVCon);
	protected:
		void UpdateImeComposition();
	public:
		void CheckNeedUpdateTitle(LPCWSTR asRConTitle);
		void UpdateTitle();
		void UpdateProgress(/*BOOL abUpdateTitle*/);
		static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK WorkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		BOOL isDialogMessage(MSG &Msg);
		bool isSkipNcMessage(const MSG& Msg);
		void PreWndProc(UINT messg);
		void OnCreateFinished();
		LRESULT WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
	public:
		void OnSwitchGuiFocus(SwitchGuiFocusOp FocusOp);
		void OnAlwaysShowScrollbar(bool abSync = true);
		void OnBufferHeight();
		void OnConsoleKey(WORD vk, LPARAM Mods);
		LRESULT OnCreate(HWND hWnd, LPCREATESTRUCT lpCreate);
		LRESULT OnDestroy(HWND hWnd);
		LRESULT OnFlashWindow(WPARAM wParam, LPARAM lParam);
		void DoFlashWindow(CESERVER_REQ_FLASHWINFO* pFlash, bool bFromMacro);
		LRESULT OnFocus(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam, LPCWSTR asMsgFrom = NULL, bool abForceChild = false);
		bool IsChildFocusAllowed(HWND hChild);
		void RefreshWindowStyles();
		LRESULT OnKeyboard(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKeyboardHook(DWORD VkMod);
		LRESULT OnKeyboardIme(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
		LRESULT OnLangChange(UINT messg, WPARAM wParam, LPARAM lParam);
		LRESULT OnLangChangeConsole(CVirtualConsole *apVCon, const DWORD adwLayoutName);
		LRESULT OnMouse(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
		LRESULT OnActivateByMouse(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
		LRESULT OnMouse_Move(CVirtualConsole* pVCon, HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam, POINT ptCur, COORD cr);
		LRESULT OnMouse_LBtnDown(CVirtualConsole* pVCon, HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam, POINT ptCur, COORD cr);
		LRESULT OnMouse_LBtnUp(CVirtualConsole* pVCon, HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam, POINT ptCur, COORD cr);
		LRESULT OnMouse_LBtnDblClk(CVirtualConsole* pVCon, HWND hWnd, UINT& messg, WPARAM wParam, LPARAM lParam, POINT ptCur, COORD cr);
		LRESULT OnMouse_RBtnDown(CVirtualConsole* pVCon, HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam, POINT ptCur, COORD cr);
		LRESULT OnMouse_RBtnUp(CVirtualConsole* pVCon, HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam, POINT ptCur, COORD cr);
		LRESULT OnMouse_RBtnDblClk(CVirtualConsole* pVCon, HWND hWnd, UINT& messg, WPARAM wParam, LPARAM lParam, POINT ptCur, COORD cr);
		BOOL OnMouse_NCBtnDblClk(HWND hWnd, UINT& messg, WPARAM wParam, LPARAM lParam);
		LRESULT OnSetCursor(WPARAM wParam=-1, LPARAM lParam=-1);
		LRESULT OnQueryEndSession(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnSessionChanged(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnShellHook(WPARAM wParam, LPARAM lParam);
		UINT_PTR SetKillTimer(bool bEnable, UINT nTimerID, UINT nTimerElapse);
		LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
		void OnTimer_Background();
		void OnTimer_Main(CVirtualConsole* pVCon);
		void OnTimer_ActivateSplit();
		void OnTimer_ConRedraw(CVirtualConsole* pVCon);
		void OnTimer_FrameAppearDisappear(WPARAM wParam);
		void OnTimer_RClickPaint();
		void OnTimer_AdmShield();
		int mn_TBOverlayTimerCounter = 0;
		void OnTimer_QuakeFocus();
		void OnActivateSplitChanged();
		void OnTransparent();
		void OnTransparent(bool abFromFocus/* = false*/, bool bSetFocus/* = false*/);
		void OnTransparentSeparate(bool bSetFocus);
		void OnVConCreated(CVirtualConsole* apVCon, const RConStartArgsEx *args);
		void OnAllVConClosed();
		void OnAllGhostClosed();
		void OnGhostCreated(CVirtualConsole* apVCon, HWND ahGhost);
		void OnRConStartedSuccess(CRealConsole* apRCon);
		LRESULT OnUpdateScrollInfo(BOOL abPosted = FALSE);
		void OnPanelViewSettingsChanged(BOOL abSendChanges=TRUE);
		void OnGlobalSettingsChanged();
		void OnTaskbarCreated();
		void OnTaskbarButtonCreated();
		void OnTaskbarSettingsChanged();
		void OnDefaultTermChanged();
		#ifdef __GNUC__
		AlphaBlend_t GdiAlphaBlend = nullptr;
		#endif
		void OnActiveConWndStore(HWND hConWnd);

		// return true - when state was changes
		bool SetTransparent(HWND ahWnd, UINT anAlpha/*0..255*/, bool abColorKey = false, COLORREF acrColorKey = 0, bool abForceLayered = false);
		GetLayeredWindowAttributes_t _GetLayeredWindowAttributes = nullptr;
		#ifdef __GNUC__
		SetLayeredWindowAttributes_t SetLayeredWindowAttributes = nullptr;
		#endif

		// IME support (WinXP or later)
		HMODULE mh_Imm32 = NULL;
		ImmSetCompositionFontW_t _ImmSetCompositionFont = nullptr;
		ImmSetCompositionWindow_t _ImmSetCompositionWindow = nullptr;
		ImmGetContext_t _ImmGetContext = nullptr;

	protected:
		struct SshAgent
		{
			HANDLE hAgent;
			DWORD  nAgentPID;
			DWORD  nExitCode;
		};
		MArray<SshAgent> m_SshAgents;
		MSectionSimple* m_SshAgentsLock = nullptr;
		void TerminateSshAgents();
	public:
		void RegisterSshAgent(DWORD SshAgentPID);

	public:
		// Windows7 - lock creating new consoles (ConHost search related)
		bool LockConhostStart();
		void UnlockConhostStart();
		void ReleaseConhostDelay();
	protected:
		struct LockConhostStart {
			MSectionSimple* pcs;
			MSectionLockSimple* pcsLock;
			bool wait;
		} m_LockConhostStart = {};
};

// Message Logger
// Originally from http://preshing.com/20120522/lightweight-in-memory-logging
namespace ConEmuMsgLogger
{
	enum Source {
		msgCommon,
		msgMain,
		msgWork,
		msgApp,
		msgCanvas,
		msgBack,
		msgGhost,
	};
	static const int BUFFER_SIZE = RELEASEDEBUGTEST(0x1000,0x1000);   // Must be a power of 2
	extern MSG g_msg[BUFFER_SIZE];
	extern LONG g_msgidx;

	static const int BUFFER_POS_SIZE = RELEASEDEBUGTEST(0x100,0x100);   // Must be a power of 2
	struct Event {
		short x, y, w, h;
		enum {
			Empty,
			WindowPosChanging,
			WindowPosChanged,
			Sizing,
			Size,
			Moving,
			Move,
		} msg;
		DWORD time;
		HWND hwnd;
	};
	extern Event g_pos[BUFFER_POS_SIZE];
	extern LONG g_posidx;
	extern void LogPos(const MSG& msg, Source from);

	inline void Log(const MSG& msg, Source from)
	{
		// Get next message index
		LONG i = _InterlockedIncrement(&g_msgidx);
		// Write a message at this index
		g_msg[i & (BUFFER_SIZE - 1)] = msg; // Wrap to buffer size
		switch (msg.message)
		{
		case WM_WINDOWPOSCHANGING:
		case WM_WINDOWPOSCHANGED:
		case WM_MOVE:
		case WM_SIZE:
		case WM_SIZING:
		case WM_MOVING:
			LogPos(msg, from);
			break;
		}
	}
}

extern CConEmuMain *gpConEmu;
