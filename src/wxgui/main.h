//////////////////////////////////////////////////////////////////////////
//
//  UltraDefrag - a powerful defragmentation tool for Windows NT.
//  Copyright (c) 2007-2017 Dmitri Arkhangelski (dmitriar@gmail.com).
//  Copyright (c) 2010-2013 Stefan Pendl (stefanpe@users.sourceforge.net).
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _UDEFRAG_GUI_MAIN_H_
#define _UDEFRAG_GUI_MAIN_H_
#pragma comment(lib, "kernel32")

// =======================================================================
//                               Headers
// =======================================================================
#include "prec.h"

// =======================================================================
//                              Constants
// =======================================================================

#define MAX_UTF8_PATH_LENGTH (256 * 1024)
#define LIST_COLUMNS 6 // number of columns in the list of volumes

#define USAGE_TRACKING_ACCOUNT_ID wxT("UA-15890458-1")
#define TEST_TRACKING_ACCOUNT_ID  wxT("UA-70148850-1")

#ifndef _WIN64
  #define TRACKING_ID wxT("gui-x86")
#else
 #if defined(_IA64_)
  #define TRACKING_ID wxT("gui-ia64")
 #else
  #define TRACKING_ID wxT("gui-x64")
 #endif
#endif

// append html extension to the tracking id, for historical reasons
#define USAGE_TRACKING_PATH wxT("/appstat/") TRACKING_ID wxT(".html")

#define TEST_TRACKING_PATH \
    wxT("/") wxT(wxUD_ABOUT_VERSION) \
    wxT("/") TRACKING_ID wxT("/")

#define GA_REQUEST(type) ::Utils::GaRequest(type##_PATH, type##_ACCOUNT_ID)

enum {
    // file menu identifiers

    // NOTE: they share a single event handler
    ID_Analyze = 1,
    ID_Defrag,
    ID_QuickOpt,
    ID_FullOpt,
    ID_MoveToFront,
    ID_MoveToEnd,
    ID_MftOpt,

    ID_Pause,
    ID_Stop,

    ID_ShowReport,

    ID_SkipRem,
    ID_Rescan,

    ID_Repair,

    ID_WhenDoneNone,
    ID_WhenDoneExit,
    ID_WhenDoneStandby,
    ID_WhenDoneHibernate,
    ID_WhenDoneLogoff,
    ID_WhenDoneReboot,
    ID_WhenDoneShutdown,

    ID_Exit,

    // settings menu identifiers
    ID_LangTranslateOnline,
    ID_LangTranslateOffline,
    ID_LangOpenFolder,

    ID_GuiOptions,

    ID_BootEnable,
    ID_BootScript,

    ID_ChooseFont,
    ID_SortByPath,
    ID_SortBySize,
    ID_SortByCreationDate,
    ID_SortByModificationDate,
    ID_SortByLastAccessDate,

    ID_SortAscending,
    ID_SortDescending,

    // help menu identifiers
    ID_HelpContents,
    ID_HelpBestPractice,
    ID_HelpFaq,
    ID_HelpLegend,

    ID_DebugLog,
    ID_DebugSend,

    // NOTE: they share a single event handler
    ID_HelpUpgradeNone,
    ID_HelpUpgradeStable,
    ID_HelpUpgradeAll,
    ID_HelpUpgradeCheck,

    ID_HelpAbout,

    // event identifiers
    ID_AdjustListColumns,
    ID_AdjustListHeight,
    ID_AdjustFilesListColumns,      //genBTC fileslist.cpp
    ID_AdjustSystemTrayIcon,
    ID_AdjustTaskbarIconOverlay,
    ID_BootChange,
    ID_CacheJob,
    ID_DefaultAction,
    ID_DiskProcessingFailure,
    ID_JobCompletion,
    ID_QueryCompletion,             //genBTC query.cpp callback
    ID_PopulateList,
    ID_PopulateFilesList,           //genBTC fileslist.cpp
    ID_ReadUserPreferences,
    ID_RedrawMap,
    ID_RefreshDrivesInfo,
    ID_RefreshFrame,
    ID_SelectAll,
    ID_SetWindowTitle,
    ID_ShowUpgradeDialog,
    ID_Shutdown,
    ID_UpdateStatusBar,
    ID_UpdateVolumeInformation,
    ID_UpdateVolumeStatus,

    // tray icon menu identifiers
    ID_ShowHideMenu,
    ID_PauseMenu,
    ID_ExitMenu,
    ID_SelectProperDrive,
    ID_QueryClusters,               //genBTC query.cpp action
    ID_QueryFreeGaps,
    ID_QueryOperation2,
    ID_QueryOperation3,
    ID_QueryOperation4,
    
    ID_ANALYZE,                     //genBTC query.cpp buttons
    ID_WXCOMBOBOX1,
    ID_WXFILEPICKERCTRL1,
    ID_WXTEXTCTRL1,
    ID_WXSTATICTEXT1,
    ID_PerformQueryBUTTON,
    ID_LCNButton1,
    ID_LCNButton2,

    // language selection menu item, must always be last in the list
    ID_LocaleChange
};
//set defaults:
#define MAIN_WINDOW_DEFAULT_WIDTH  900
#define MAIN_WINDOW_DEFAULT_HEIGHT 600
#define MAIN_WINDOW_MIN_WIDTH      640
#define MAIN_WINDOW_MIN_HEIGHT     480
#define DEFAULT_LIST_HEIGHT        145
#define MIN_PANEL_HEIGHT            42

// dialog layout constants
#define SMALL_SPACING  DPI(5)
#define LARGE_SPACING  DPI(11)
//default gui.ini
#define DEFAULT_DRY_RUN          1      //genbtc wants this. TODO: decide.
#define DEFAULT_FREE_COLOR_R   255
#define DEFAULT_FREE_COLOR_G   255
#define DEFAULT_FREE_COLOR_B   255
#define DEFAULT_GRID_COLOR_R     0
#define DEFAULT_GRID_COLOR_G     0
#define DEFAULT_GRID_COLOR_B     0
#define DEFAULT_GRID_LINE_WIDTH  1
#define DEFAULT_MAP_BLOCK_SIZE   4
#define DEFAULT_MINIMIZE_TO_SYSTEM_TRAY          1
#define DEFAULT_SECONDS_FOR_SHUTDOWN_REJECTION  60
#define DEFAULT_SHOW_MENU_ICONS                  1
#define DEFAULT_SHOW_PROGRESS_IN_TASKBAR         1
#define DEFAULT_SHOW_TASKBAR_ICON_OVERLAY        1

/* user defined language IDs
   Important: never change their order when adding new translations
   or the selection of the user will be broken */
enum {
    wxUD_LANGUAGE_ILOKO = wxLANGUAGE_USER_DEFINED+1,
    wxUD_LANGUAGE_KAPAMPANGAN,
    wxUD_LANGUAGE_NORWEGIAN,
    wxUD_LANGUAGE_WARAY_WARAY,
    wxUD_LANGUAGE_ACOLI,
    wxUD_LANGUAGE_SINHALA_SRI_LANKA,
    wxUD_LANGUAGE_SILESIAN,
    wxUD_LANGUAGE_BASQUE,
    wxUD_LANGUAGE_LAST          // must always be last in the list
};

// =======================================================================
//                          Macro definitions
// =======================================================================

/*
* Convert wxString to formats acceptable by vararg functions.
* NOTE: Use it to pass strings to functions only as returned
* objects may be temporary!
*/
#define CVT_ANSI(s) ((const char *)s.mb_str())
#define ws(s) ((const wchar_t *)s.wc_str())

/* converts pixels from 96 DPI to the current one */
#define DPI(x) ((int)((double)x * g_scaleFactor))

/*
* Immediately processes a command event.
* NOTE: Call it from the main thread only!
*/
#define ProcessCommandEvent(window,id) { \
    wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED,id); \
    window->GetEventHandler()->ProcessEvent(event); \
}

/*
* Safely delivers a command event to the event queue.
*/
#define QueueCommandEvent(window,id) { \
    window->GetEventHandler()->QueueEvent( \
        new wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED,id) \
    ); \
}

// =======================================================================
//                            Declarations
// =======================================================================

class StatThread: public wxThread {
public:
    StatThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }
    ~StatThread() { Wait(); }

    void *Entry() override;
};

class Log: public wxLog {
public:
    Log()  { delete SetActiveTarget(this); };
    ~Log() { SetActiveTarget(nullptr); };

    void DoLogTextAtLevel(wxLogLevel level, const wxString& msg) override;
};

class App: public wxApp {
public:
    bool OnInit() override;
    int  OnExit() override;

    void OnInitCmdLine(wxCmdLineParser& parser) override
    {
        /* wxWidgets raises an "assert failure" message without them */
        parser.AddSwitch(wxT("s"),wxT("setup"),wxT("setup"));
        parser.AddSwitch(wxT("v"),wxT("verbose"),wxT("verbose"));
    }

    /* wxLANGUAGE_UNKNOWN forces to use the most suitable locale */
    static void InitLocale() { SetLocale(wxLANGUAGE_UNKNOWN); }
    static void SetLocale(int id);
    static void ResetLocale();
    static void SaveReportTranslation();

    static void AttachDebugger();

private:
    void Cleanup();

    Log *m_log;
    StatThread *m_statThread;
};

class BtdThread: public wxThread {
public:
    BtdThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }
    ~BtdThread() { Wait(); }

    void *Entry() override;
};

class ConfigThread: public wxThread {
public:
    ConfigThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }
    ~ConfigThread() { Wait(); }

    void *Entry() override;
};

class JobThread: public wxThread {
public:
    JobThread() : wxThread(wxTHREAD_JOINABLE) {
        m_launch = false; Create(); Run();
    }
    ~JobThread() { Wait(); }

    void *Entry() override;

    bool m_launch;
    wxArrayString *m_volumes;
    udefrag_job_type m_jobType;
    int m_mapSize;
    int m_flags;
    bool singlefile;

private:
    void ProcessVolume(int index);
    static void ProgressCallback(udefrag_progress_info *pi, void *p);
    static int Terminator(void *p);

    char m_letter;
};

class ListThread: public wxThread {
public:
    ListThread() : wxThread(wxTHREAD_JOINABLE) {
        m_rescan = true; Create(); Run();
    }
    ~ListThread() { Wait(); }

    void *Entry() override;

    bool m_rescan;
};

class RefreshDrivesInfoThread : public wxThread {
public:
    RefreshDrivesInfoThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }
    ~RefreshDrivesInfoThread() { Wait(); }

    void *Entry() override;
};

class RecoveryConsole : public wxProcess {
public:
    void OnTerminate(int pid, int status) override;
};

//genBTC query.cpp Query Thread.
class QueryThread: public wxThread {
public:
    QueryThread() : wxThread(wxTHREAD_JOINABLE) {
        m_startquery = false; Create(); Run();
    }
    ~QueryThread() { Wait(); }

    int stopgap_finish_operation3();
    void *Entry() override;
    
    void DisplayQueryResults(udefrag_query_parameters* qp);
    
    bool m_startquery;
    wxString m_querypath;
    int m_flags;
    bool singlefile;
    int m_mapSize;
    char m_letter;
    udefrag_query_type m_qType;
    udefrag_query_parameters* m_qp;

private:
    static void PostProgressCallback(udefrag_query_parameters* qp, void* p);
    static int Terminator(void *p);    
};

class UpgradeThread: public wxThread {
public:
    UpgradeThread(int level) : wxThread(wxTHREAD_JOINABLE) {
        m_level = level; m_check = true; Create(); Run();
    }
    ~UpgradeThread() { Wait(); }

    void *Entry() override;

    bool m_check;
    int m_level;

private:
    int ParseVersionString(const char *version);
};

class SystemTrayIcon: public wxTaskBarIcon {
public:
    wxMenu *CreatePopupMenu() override;

    void OnMenuShowHide(wxCommandEvent& event);
    void OnMenuPause(wxCommandEvent& event);
    void OnMenuExit(wxCommandEvent& event);

    void OnLeftButtonUp(wxTaskBarIconEvent& event);

    DECLARE_EVENT_TABLE()
};

class DrivesList: public wxListView {
public:
    DrivesList(wxWindow* parent, long style)
      : wxListView(parent,wxID_ANY,
        wxDefaultPosition,wxDefaultSize,style) {}
    ~DrivesList() {}

    void DeSelectAll();
    int GetIndexFromLetter(char sel = 0);
    char GetLetter(int i = -1);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnSelectionChange(wxListEvent& event);

    DECLARE_EVENT_TABLE()
};

class ListSortInfo{
public:
        ListSortInfo(): ListCtrl(NULL)
	{
		SortAscending = false;
		Column = 1; //pre-sorted by Fragments.
	}

	bool SortAscending;
    int Column;
    class FilesList *ListCtrl;
};
struct FilesListItem{
    wxString col0,col1,col2,col3,col4,col5;
    ULONGLONG col2bytes;
    long currindex;
    wxString ReturnColumn(long column) const
    {
        switch (column) {
        case 0:
            return col0;
        case 1:
            return col1;
        case 2:
            return col2;
        case 3:
            return col3;
        case 4:
            return col4;
        case 5:
            return col5;
        default:
            wxFAIL_MSG("Invalid column index in FilesList::OnGetItemText");
            return wxEmptyString;
        }        
    }
};

typedef std::vector<FilesListItem> FilesListItems;

//genBTC FilesList.cpp
class FilesList: public wxListCtrl {
public:
    FilesList(wxWindow* parent, long style)
		: wxListCtrl(parent, wxID_ANY,
		             wxDefaultPosition, wxDefaultSize, style), currentlyselected(0),
		  currently_being_workedon_filenames(NULL), n_lastItem(0)
	{
	}

    ~FilesList() {}
    
    FilesListItems allitems;    //data container for virtual list
    void SortVirtualItems(int Column);
    // -----------------------------------------------------------------------
    //BEGIN PROTOTYPES FROM wxListView
    // ---------------------
    // focus/selection stuff
    // ---------------------
    // [de]select an item
    void Select(long n, bool on = true){
        SetItemState(n, on ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED);}
    // focus and show the given item
    void Focus(long index)
    {
        SetItemState(index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        EnsureVisible(index);
    }
    // get the currently focused item or -1 if none
    long GetFocusedItem() const{
        return GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);}
    // get first and subsequent selected items, return -1 when no more
    long GetNextSelected(long item) const{
        return GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);}
    long GetFirstSelected() const{
        return GetNextSelected(-1);}
    // return true if the item is selected
    bool IsSelected(long index) const{
        return GetItemState(index, wxLIST_STATE_SELECTED) != 0;}
    //END PROTOTYPES FROM wxListView
    // -----------------------------------------------------------------------    
    void OnItemRClick(wxListEvent& event);
    void OnSelect(wxListEvent& event);
    void OnDeSelect(wxListEvent& event);

    void RClickOpenExplorer(wxCommandEvent& event);
    void RClickCopyClipboard(wxCommandEvent& event);
    void RClickSubMenuMoveFiletoDriveX(wxCommandEvent& event);
    void GetFilesAndMakeFilterLists();
    void ReSelectProperDrive(wxCommandEvent& event);
    void RClickDefragMoveSingle(wxCommandEvent& event);
    bool RemoveSingleFileAt();

    wxListItem GetListItem(long index=-1,long col=-1);

    long currentlyselected;
    wxArrayString *currently_being_workedon_filenames;
    
    int n_lastItem;
    ListSortInfo m_sortinfo; 
    void OnColClick(wxListEvent& event );

    DECLARE_EVENT_TABLE()

    //overload required for virtual mode.
	wxString OnGetItemText(long item, long column) const override;    
};

//simple struct for a out& param from ClusterMap::GetGridSizeforCMap
struct cmapreturn
{
    int width, height;
    int block_size;
    int line_width;
    int cell_size;
    int blocks_per_line;
    int lines;
};

class LegendTransientPopup : public wxPopupTransientWindow
{
public:
    explicit LegendTransientPopup(wxWindow* parent);
    wxWindow* m_owner;  //PARENT
    wxWindow* m_window;
};

class ClusterMap: public wxWindow {
public:
    explicit ClusterMap(wxWindow* parent);
    ~ClusterMap();

    void OnEraseBackground(wxEraseEvent& event);
    void OnPaint(wxPaintEvent& event);
    ULONGLONG getLCNsfromMousePos(const wxPoint& pos) const;
    void ClusterMapGetLCN(wxMouseEvent& event);
    static void DrawSingleRectangleBorder(HDC m_cacheDC2, int xblock, int yblock, int line_width, int cell_size, HBRUSH brush,
        HBRUSH infill);
    ClusterMap *m_ClusterMap;
private:
    static char *ScaleMap(int scaled_size);
    void GetGridSizeforCMap(cmapreturn* gridsize) const;    
    int m_width;
    int m_height;
    HDC m_cacheDC;
    HBITMAP m_cacheBmp;
    HBRUSH m_brushes[SPACE_STATES];
    LegendTransientPopup* m_legendPopup;

DECLARE_EVENT_TABLE()
};

typedef struct _JobsCacheEntry {
    udefrag_job_type jobType;
    udefrag_progress_info pi;
    char *clusterMap;
    bool stopped;
} JobsCacheEntry;

//STDLib unordered map.
WX_DECLARE_HASH_MAP(int, JobsCacheEntry*, \
    wxIntegerHash, wxIntegerEqual, JobsCache);;

// =======================================================================
//                          Main Frame
// =======================================================================

class MainFrame: public wxFrame {
public:
    MainFrame();
    ~MainFrame();

    WXLRESULT MSWWindowProc(WXUINT msg,WXWPARAM wParam,WXLPARAM lParam);

    bool CheckForTermination(int time);

    // action menu handlers
    void OnStartJob(wxCommandEvent& event);
    void OnPause(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnShowReport(wxCommandEvent& event);
    void OnRepair(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    // settings menu handlers
    void OnLangTranslateOnline(wxCommandEvent& event);
    void OnLangTranslateOffline(wxCommandEvent& event);
    void OnLangOpenFolder(wxCommandEvent& event);
    void OnGuiOptions(wxCommandEvent& event);
    void OnBootEnable(wxCommandEvent& event);
    void OnBootScript(wxCommandEvent& event);

    void ChooseFontDialog(wxCommandEvent& event);//genBTC fontpicker.

    // help menu handlers
    void OnHelpContents(wxCommandEvent& event);
    void OnHelpBestPractice(wxCommandEvent& event);
    void OnHelpFaq(wxCommandEvent& event);
    void OnHelpLegend(wxCommandEvent& event);

    void OnDebugLog(wxCommandEvent& event);
    void OnDebugSend(wxCommandEvent& event);

    void OnHelpUpgrade(wxCommandEvent& event);
    void OnHelpAbout(wxCommandEvent& event);

    // event handlers
    //void OnActivate(wxActivateEvent& event);
    void OnMove(wxMoveEvent& event);
    void OnSize(wxSizeEvent& event);

    void AdjustSystemTrayIcon(wxCommandEvent& event);
    void AdjustTaskbarIconOverlay(wxCommandEvent& event);
    void CacheJob(wxCommandEvent& event);
    void OnBootChange(wxCommandEvent& event);
    void OnDefaultAction(wxCommandEvent& event);
    void OnDiskProcessingFailure(wxCommandEvent& event);
    void OnJobCompletion(wxCommandEvent& event);
    void OnQueryCompletion(wxCommandEvent& event);  //genBTC query.cpp
    void OnLocaleChange(wxCommandEvent& event);
    void ReadUserPreferences(wxCommandEvent& event);
    void RedrawMap(wxCommandEvent& event);
    void RefreshDrivesInfo(wxCommandEvent& event);
    void RefreshFrame(wxCommandEvent& event);
    void SelectAll(wxCommandEvent& event);
    void SetWindowTitle(wxCommandEvent& event);
    void ShowUpgradeDialog(wxCommandEvent& event);
    void Shutdown(wxCommandEvent& event);
    void UpdateStatusBar(wxCommandEvent& event);

    //Volume List (mixed)
    void AdjustListColumns(wxCommandEvent& event);
    void AdjustListHeight(wxCommandEvent& event);
    void OnListSize(wxSizeEvent& event);
    void OnSplitChanged(wxSplitterEvent& event);
    void OnSkipRem(wxCommandEvent& event);
    void OnRescan(wxCommandEvent& event);
    void PopulateList(wxCommandEvent& event);
    void UpdateVolumeInformation(wxCommandEvent& event);
    void UpdateVolumeStatus(wxCommandEvent& event);
    int GetIndexFromDriveLetter();  //genBTC
    int GetDriveLetter();           //genBTC

    // Files List (new) by genBTC
    void FilesAdjustListColumns(wxCommandEvent& event);
    void FilesOnListSize(wxSizeEvent& event);
    void FilesPopulateList(wxCommandEvent& event);
    void ReSelectProperDrive(wxCommandEvent& event);

    //for Query Menu (new) by genBTC
    void QueryClusters(wxCommandEvent& event);   //genBTC query.cpp
    void QueryOperation4(wxCommandEvent& event);   //genBTC query.cpp Op2.
    void QueryOperation3(wxCommandEvent& event);   //genBTC query.cpp Op3.

    // common routines
    static int  CheckOption(const wxString& name);
    void SetTaskbarProgressState(TBPFLAG flag);
    void SetTaskbarProgressValue(ULONGLONG completed, ULONGLONG total);

    //for LCN Menu (new) by genBTC
    void InitLCNPanel();                            //genBTC LCN.cpp
    void GetAllLCNs(wxCommandEvent& event);
    void GetSpecificLCNRange(wxCommandEvent& event);

    bool m_skipRem;
    bool m_busy;
    bool m_paused;
    bool m_stopped;

    // overall progress counters
    int m_selected;
    int m_processed;

    SystemTrayIcon *m_systemTrayIcon;

    JobThread *m_jobThread;
    JobsCache m_jobsCache;
    JobsCacheEntry *m_currentJob;

    QueryThread *m_queryThread;

    wxMenu     *m_RClickPopupMenu1;     //genBTC Right Click Popup Menu
    wxMenu     *m_DriveSubMenu;         //genBTC Right Click Popup Menu

    wxTextCtrl  *m_WxTextCtrl1;            //genBTC Query - tab 3.
    wxButton    *m_PerformQuery1;             //genBTC Query - tab 3.
    wxFilePickerCtrl *m_WxFilePickerQuery1;    //genBTC Query - tab 3.
    wxToggleButton *m_toggleBtn1;       //genBTC LCN - tab 4
    wxButton    *m_toggleBtn2;       //genBTC LCN - tab 4
    volume_info  m_volinfocache;     //genBTC
    LegendTransientPopup* m_legendPopup;    //genBTC legend.
private:
    int GetMapSize();       //genBTC - used by job.cpp & query.cpp
    void InitQueryMenu();   //genBTC query.cpp
    void InitPopupMenus();  //genBTC Right Click Popup Menu
    void InitFilesList();    //genBTC FilesList.cpp
    void InitMenu();
    void InitToolbar();
    void InitStatusBar();
    void InitVolList();
    void ReadAppConfiguration();
    void ReleasePause();
    void RemoveTaskbarIconOverlay();
    void SaveAppConfiguration();
    void SetPause();
    void SetTaskbarIconOverlay(
        const wxString& icon,
        const wxString& text);
    int ShowShutdownDialog(int action);

    int  m_x;
    int  m_y;
    int  m_width;
    int  m_height;
    bool m_saved;
    bool m_maximized;
    int  m_separatorPosition;

    // list dimensions
    double m_fcolsr[LIST_COLUMNS]; //genBTC FilesList.cpp column ratios
    int m_fcolsw[LIST_COLUMNS]; //genBTC FilesList.cpp column widths
    int m_vListHeight;
    int m_filesListHeight;  //genBTC FilesList.cpp height
    int m_origColumnWidths[LIST_COLUMNS];
    int m_columnWidths[LIST_COLUMNS];

    wxFont *m_vListFont;
    wxFont *m_filesListFont;  //genBTC FilesList.cpp font

    wxString   *m_title;
    wxToolBar  *m_toolBar;
    wxMenuBar  *m_menuBar;
    wxMenuItem *m_subMenuWhenDone;
    wxMenuItem *m_subMenuLanguage;
    wxMenuItem *m_subMenuBootConfig;
    wxMenuItem *m_subMenuSortingConfig;
    wxMenuItem *m_subMenuDebug;
    wxMenuItem *m_subMenuUpgrade;
    wxMenu     *m_menuLanguage;

    wxNotebook *m_notebook1;        //genBTC New Tabs Notebook
    wxPanel *m_panel1;              //genBTC Tab1
    wxSplitterWindow *m_splitter1;  //genBTC New Tabs Splitter
    wxPanel *m_panel2;              //genBTC Tab2
    wxPanel *m_panel3;              //genBTC Tab3
    wxPanel *m_panel4;              //genBTC Tab4

    wxSplitterWindow *m_splitter;
    DrivesList       *m_vList;
    ClusterMap       *m_cMap;
    FilesList        *m_filesList;  //genBTC FilesList.cpp

    bool m_btdEnabled;
    BtdThread *m_btdThread;

    ConfigThread    *m_configThread;
    ListThread      *m_listThread;
    UpgradeThread   *m_upgradeThread;

    RefreshDrivesInfoThread *m_rdiThread;

    DECLARE_EVENT_TABLE()
};

class Utils {
public:
    static bool CheckAdminRights(void);
    static bool DownloadFile(const wxString& url, const wxString& path);
    static void GaRequest(const wxString& path, const wxString& id);
    static wxBitmap LoadPngResource(const wchar_t *name);
    static int MessageDialog(wxFrame *parent,
        const wxString& caption, const wxArtID& icon,
        const wxString& text1, const wxString& text2,
        const wxString& format, ...);
    static void OpenHandbook(const wxString& page,
        const wxString& anchor = wxEmptyString);
    static bool SetProcessPriority(int priority);
    static void ShellExec(const wxString& file,
        const wxString& action = wxT("open"),
        const wxString& parameters = wxEmptyString,
        int show = SW_SHOW, int flags = 0);
    static void ShowError(const wxString& format, ...);
    static void createDirectoryRecursively(const std::wstring &directory);
    static void extendfiltertext(wxString itemtext,wxString *extfiltertext);
    static wxString makefiltertext(wxString itemtext);
};

// =======================================================================
//                           Global variables
// =======================================================================

extern MainFrame *g_mainFrame;
extern wxLocale *g_locale;
extern double g_scaleFactor;
extern int g_iconSize;
extern HANDLE g_synchEvent;
extern bool g_refreshDrivesInfo;
extern const wchar_t* g_colornames[SPACE_STATES];   //genbtc
extern COLORREF g_colors[SPACE_STATES];             //genbtc
#endif /* _UDEFRAG_GUI_MAIN_H_ */
