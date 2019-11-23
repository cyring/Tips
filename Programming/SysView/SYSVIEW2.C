#define INCL_PM
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#define INCL_DOSSEMAPHORES
#define INCL_DOSMODULEMGR
#define INCL_DOSDEVICES
#define INCL_DOSERRORS

#include <OS2.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

#include "SysView2.h"
#include "SysProc2.h"

#define WINS    5

typedef struct
    {
    HSWITCH  hswitch;
    ULONG    pid;
    ULONG    sid;
    HWND     hwnd;
    CHAR     title[256];
    }     TasksList_t;

struct
    {
    INT             Thread;
    HAB             Hab;
    HMQ             Hmq;
    QMSG            Qmsg;
    HWND            Frame;
    ULONG           fl;
    HWND            Main;
    HWND            Icons;
    USHORT          Button;
    HWND            User;
    HWND            hPopMenu;
    BOOL            isPopingUp;
    INT             Tid;
    HMTX            Sem;
    ULONG           Signal;
    TasksList_t     *TasksList;
    ProcsList_t     *ProcsList;
    ThreadsList_t   *ThreadsList;
    }
        Win[WINS] = {0};

struct
    {
    CHAR    version[8];
    CHAR    bootDrive;
    CHAR    cdrom;
    CHAR    threads[6];
    CHAR    priority[16];
    CHAR    swapFile[256];
    }
        Config = {0};

VOID             DisplayMessage( PCH str );
VOID             SysOpenView( INT idx );
MRESULT EXPENTRY SysView2main( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY SysView2icons( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY SysView2about( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY SysView2minimized( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY SysView2info( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
        VOID     SystemTasks( INT idx, HWND hTasksList );
MRESULT EXPENTRY SysView2tasks( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY SysView2vm( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
VOID    EXPENTRY SystemProcs( INT idx, HWND hList );
MRESULT EXPENTRY SysView2procs( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
VOID    EXPENTRY SystemThreads( INT idx, USHORT pid, HWND hList );
MRESULT EXPENTRY SysView2threads( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );


INT     GetTID( VOID )
{
    PTIB    pptib = NULL;
    PPIB    pppib = NULL;

    DosGetInfoBlocks( &pptib, &pppib );
    return( (INT) pptib->tib_ptib2->tib2_ultid );
}

ULONG   GetPID( VOID )
{
    PTIB    pptib = NULL;
    PPIB    pppib = NULL;

    DosGetInfoBlocks( &pptib, &pppib );
    return( pppib->pib_ulpid );
}


INT     FindWindow( INT thread )
{
    INT     idx = 0;
    INT     wdx = -1;

    DosEnterCritSec();

    for( idx = 0; idx < WINS; idx++ )
        if( Win[idx].Thread == thread )
            {
            wdx = idx;
            break;
            }

    DosExitCritSec();

    return( wdx );
}

INT     OpenWindow( INT thread )
{
    INT     idx = 0;
    INT     wdx = -1;
    UCHAR   semaphore[] = "\\SEM32\\SYSVIEW#.###";

    DosEnterCritSec();

    for( idx = 0; idx < WINS; idx++ )
        if( Win[idx].Thread == 0 )
            {
            Win[idx].Thread = thread;
            sprintf( semaphore, "\\SEM32\\SYSVIEW%1d.%03d", idx, GetPID() );
            DosCreateMutexSem( semaphore, &(Win[idx].Sem), 0, 0 );
            wdx = idx;
            break;
            }

    DosExitCritSec();

    return( wdx );
}

VOID    CloseWindow( INT idx )
{
    DosEnterCritSec();

    DosCloseMutexSem( Win[idx].Sem );
    memset( &(Win[idx]), 0, sizeof(Win[idx]) );

    DosExitCritSec();
}

VOID    NewWindow( VOID *dummy )
{
    INT     idx = OpenWindow( GetTID() );

    if( idx != -1 )
        {
        SysOpenView(idx);
        CloseWindow(idx);
        }
    else
        WinAlarm( HWND_DESKTOP, WA_WARNING );
}

BOOL    PostSignal( INT idx, ULONG sigVal )
{
    BOOL    fRet = FALSE;

    if( !DosRequestMutexSem( Win[idx].Sem, 1000 ) )
      {
      Win[idx].Signal = sigVal;

      DosReleaseMutexSem( Win[idx].Sem );
      fRet = TRUE;
      }
    else
      fRet = FALSE;

    return(fRet);
}

BOOL    TestSignal( INT idx, ULONG sigVal )
{
    BOOL    fRet = FALSE;

    if( !DosRequestMutexSem( Win[idx].Sem, 1000 ) )
      {
      if( Win[idx].Signal == sigVal )
        fRet = TRUE;
      else
        fRet = FALSE;

      DosReleaseMutexSem( Win[idx].Sem );
      }
    else
      fRet = FALSE;

    return(fRet);
}

BOOL    WaitSignal( INT idx, ULONG sigVal )
{
    INT     tries = 0;
    BOOL    fRet = FALSE;

    for( tries = 0; (tries < 10) && !fRet; tries++, DosSleep(100) )
      if( !DosRequestMutexSem( Win[idx].Sem, 1000 ) )
        {
        if( Win[idx].Signal == sigVal )
          fRet = TRUE;
        else
          fRet = FALSE;

        DosReleaseMutexSem( Win[idx].Sem );
        }
      else
        fRet = FALSE;

    return(fRet);
}

VOID    DisplayMessage( PCH str )
{
    WinMessageBox( HWND_DESKTOP, Win[0].Main, (PCH) str,
                   (PCH) "Information", 98,
                   MB_OK | MB_SYSTEMMODAL | MB_MOVEABLE | MB_ERROR );
}

VOID    WinPostWindowText( HWND hwnd, CHAR *str )
{
    WNDPARAMS   params = {0};

    params.fsStatus = WPM_TEXT;
    params.cchText  = strlen( str );
    params.pszText  = str;

    WinPostMsg( hwnd, WM_SETWINDOWPARAMS, &params, NULL );
}

VOID    WinPostItemText( HWND hwnd, SHORT index, CHAR *str )
{
    WinPostMsg( hwnd, LM_SETITEMTEXT, (MPARAM) index, (MPARAM) str );
}

VOID    IconsResize( INT idx )
{
    RECTL   rectl = {0};
    LONG    height;

    WinQueryWindowRect( Win[idx].Icons, &rectl );
    height = rectl.yTop - rectl.yBottom;
    WinQueryWindowRect( Win[idx].Main, &rectl );
    WinSetWindowPos( Win[idx].Icons, HWND_TOP, rectl.xLeft, rectl.yTop - height, rectl.xRight - rectl.xLeft, height, SWP_MOVE|SWP_SIZE );
}

VOID    UserResize( INT idx )
{
    RECTL   rectl = {0};
    LONG    height;

    WinQueryWindowRect( Win[idx].Icons, &rectl );
    height = rectl.yTop - rectl.yBottom;
    WinQueryWindowRect( Win[idx].Main, &rectl );
    WinSetWindowPos( Win[idx].User, HWND_TOP, rectl.xLeft, rectl.yBottom, rectl.xRight - rectl.xLeft, rectl.yTop - height, SWP_MOVE|SWP_SIZE );
}

VOID    SysOpenView( INT idx )
{
    CHAR    Title[] = "System View (0)";
    ULONG   flCreate = FCF_MENU|FCF_TITLEBAR|FCF_SYSMENU|FCF_MINMAX|FCF_SIZEBORDER|FCF_SHELLPOSITION|FCF_TASKLIST;
    LONG    rgbIcons = 0;

    sprintf( Title, "System View (%d)", idx );
    Win[idx].Hab = WinInitialize( 0 );
    Win[idx].Hmq = WinCreateMsgQueue( Win[idx].Hab, 0 );

    WinRegisterClass( Win[idx].Hab, "SystemView", SysView2main, CS_SIZEREDRAW, 0 );

    Win[idx].Frame = WinCreateStdWindow( HWND_DESKTOP, WS_ANIMATE, &flCreate, "SystemView", \
                                         Title, 0L, NULLHANDLE, ID_MAINWND, &Win[idx].Main );

    Win[idx].Icons = WinLoadDlg(Win[idx].Main, Win[idx].Main, SysView2icons, 0L, ID_BUTTONS, NULL);

    rgbIcons = WinQuerySysColor(HWND_DESKTOP, SYSCLR_ACTIVETITLETEXTBGND, 0);
    WinSetPresParam( Win[idx].Icons, PP_BACKGROUNDCOLOR, (ULONG) sizeof(rgbIcons), (PVOID) &rgbIcons );

    WinSendMsg( Win[idx].Icons, WM_COMMAND, (MPARAM) 102, NULL );

    WinShowWindow( Win[idx].Frame, TRUE );

    while( WinGetMsg( Win[idx].Hab, &(Win[idx].Qmsg), 0, 0, 0 ) )
        WinDispatchMsg( Win[idx].Hab, &(Win[idx].Qmsg) );

    WinDestroyWindow( Win[idx].Icons );
    WinDestroyWindow( Win[idx].Frame );
    WinDestroyMsgQueue( Win[idx].Hmq );
    WinTerminate( Win[idx].Hab );
}


MRESULT EXPENTRY SysView2main( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    INT     idx = FindWindow( GetTID() );
    PSWP    pSwp = NULL;
    RECTL   rectl = {0};
    HPS     hps;

    switch( msg )
    {
    case WM_MINMAXFRAME:
        pSwp = (PSWP) mp1;

        if( pSwp->hwnd == Win[idx].Frame )
            {
            if( pSwp->fl & SWP_MINIMIZE )
                {
                if( Win[idx].User ) WinDestroyWindow( Win[idx].User );
                Win[idx].User = WinLoadDlg(Win[idx].Main, Win[idx].Main, SysView2minimized, 0L, ID_ICONWINDOW, NULL);
                }
            else if( (pSwp->fl & (SWP_MAXIMIZE | SWP_RESTORE)) && (Win[idx].fl & SWP_MINIMIZE) )
                WinSendMsg( Win[idx].Icons, WM_COMMAND, (MPARAM) Win[idx].Button, NULL );

            Win[idx].fl = pSwp->fl;
            }
    break;

    case WM_SIZE:
        if( !(Win[idx].fl & SWP_MINIMIZE) )
            {
            IconsResize(idx);
            UserResize(idx);
            }
        else
            {
            WinQueryWindowRect( Win[idx].Main, &rectl );
            WinSetWindowPos( Win[idx].User, HWND_TOP, rectl.xLeft, rectl.yBottom, rectl.xRight - rectl.xLeft, rectl.yTop - rectl.yBottom, SWP_MOVE|SWP_SIZE );
            }

    break;

    case WM_CONTROLPOINTER:
        if( Win[idx].isPopingUp )
            {
            Win[idx].isPopingUp = FALSE;
            WinSendMsg( Win[idx].hPopMenu, MM_ENDMENUMODE, (MPARAM) TRUE, (MPARAM) NULL );
            }
        return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );

    break;

    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
        case ID_CLOSE:
          WinSendMsg( Win[idx].Frame, WM_CLOSE, NULL, NULL );
        break;

        case ID_EXIT:
          WinPostMsg( Win[0].Frame, WM_QUIT, NULL, NULL );
        break;

        case ID_NEW:
          _beginthread( NewWindow, NULL, 8192, NULL );
        break;

        case ID_COPYRIGHT:
          WinDlgBox( HWND_DESKTOP, Win[idx].Frame, SysView2about, (HMODULE)0, ID_ABOUT, NULL);
        break;

        default:
          return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    default:
      return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
    }

    return( (MRESULT) FALSE );
}

MRESULT EXPENTRY SysView2icons( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    INT     idx = FindWindow( GetTID() );
    USHORT  pid = 0;

    switch( msg )
    {
    case WM_INITDLG:
      Win[idx].hPopMenu = WinLoadMenu( HWND_DESKTOP, NULLHANDLE, ID_POPMENU );
    break;

    case WM_DESTROY:
    case WM_CLOSE:
      WinDestroyWindow( Win[idx].User );
      Win[idx].User = NULLHANDLE;
    break;

    case WM_CONTROLPOINTER:
      {
      HPOINTER  hPtr = NULLHANDLE;
      SWP       swpIcon = {0};
      SWP       swpUser = {0};
      SWP       swpMenu = {0};

      switch( SHORT1FROMMP( mp1 ) )
        {
        case 102:
          if( !Win[idx].isPopingUp )
            {
            Win[idx].isPopingUp = TRUE;
            WinQueryWindowPos( WinWindowFromID(Win[idx].Icons, 102), &swpIcon );
            WinQueryWindowPos( Win[idx].User, &swpUser );
            WinSendMsg( Win[idx].hPopMenu, MM_SETITEMTEXT, (MPARAM) ID_POPITEM, (MPARAM) "Information" );
            WinQueryWindowPos( Win[idx].hPopMenu, &swpMenu );
            WinPopupMenu( Win[idx].User, Win[idx].Icons, Win[idx].hPopMenu, swpIcon.x, swpUser.cy - swpMenu.cy - 4, 0, 0 );
            }
        break;
        case 103:
          if( !Win[idx].isPopingUp )
            {
            Win[idx].isPopingUp = TRUE;
            WinQueryWindowPos( WinWindowFromID(Win[idx].Icons, 103), &swpIcon );
            WinQueryWindowPos( Win[idx].User, &swpUser );
            WinSendMsg( Win[idx].hPopMenu, MM_SETITEMTEXT, (MPARAM) ID_POPITEM, (MPARAM) "Tasks List" );
            WinQueryWindowPos( Win[idx].hPopMenu, &swpMenu );
            WinPopupMenu( Win[idx].User, Win[idx].Icons, Win[idx].hPopMenu, swpIcon.x, swpUser.cy - swpMenu.cy - 4, 0, 0 );
            }
        break;
        case 104:
          if( !Win[idx].isPopingUp )
            {
            Win[idx].isPopingUp = TRUE;
            WinQueryWindowPos( WinWindowFromID(Win[idx].Icons, 104), &swpIcon );
            WinQueryWindowPos( Win[idx].User, &swpUser );
            WinSendMsg( Win[idx].hPopMenu, MM_SETITEMTEXT, (MPARAM) ID_POPITEM, (MPARAM) "Virtual Memory" );
            WinQueryWindowPos( Win[idx].hPopMenu, &swpMenu );
            WinPopupMenu( Win[idx].User, Win[idx].Icons, Win[idx].hPopMenu, swpIcon.x, swpUser.cy - swpMenu.cy - 4, 0, 0 );
            }
        break;
        case 105:
          if( !Win[idx].isPopingUp )
            {
            Win[idx].isPopingUp = TRUE;
            WinQueryWindowPos( WinWindowFromID(Win[idx].Icons, 105), &swpIcon );
            WinQueryWindowPos( Win[idx].User, &swpUser );
            WinSendMsg( Win[idx].hPopMenu, MM_SETITEMTEXT, (MPARAM) ID_POPITEM, (MPARAM) "Processes" );
            WinQueryWindowPos( Win[idx].hPopMenu, &swpMenu );
            WinPopupMenu( Win[idx].User, Win[idx].Icons, Win[idx].hPopMenu, swpIcon.x, swpUser.cy - swpMenu.cy - 4, 0, 0 );
            }
        break;
        case 106:
          if( !Win[idx].isPopingUp )
            {
            Win[idx].isPopingUp = TRUE;
            WinQueryWindowPos( WinWindowFromID(Win[idx].Icons, 106), &swpIcon );
            WinQueryWindowPos( Win[idx].User, &swpUser );
            WinSendMsg( Win[idx].hPopMenu, MM_SETITEMTEXT, (MPARAM) ID_POPITEM, (MPARAM) "Threads" );
            WinQueryWindowPos( Win[idx].hPopMenu, &swpMenu );
            WinPopupMenu( Win[idx].User, Win[idx].Icons, Win[idx].hPopMenu, swpIcon.x, swpUser.cy - swpMenu.cy - 4, 0, 0 );
            }
        break;
        default:
          if( Win[idx].isPopingUp )
            {
            Win[idx].isPopingUp = FALSE;
            WinSendMsg( Win[idx].hPopMenu, MM_ENDMENUMODE, (MPARAM) TRUE, (MPARAM) NULL );
            }
        break;
        }

      hPtr = (HPOINTER) mp2;
      return( (MRESULT) hPtr );
      }
    break;

    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
      case 102:
        if( Win[idx].User ) WinDestroyWindow( Win[idx].User );
        Win[idx].User = WinLoadDlg(Win[idx].Main, Win[idx].Main, SysView2info, 0L, ID_MACHINE, NULL);
        Win[idx].Button = SHORT1FROMMP( mp1 );
        UserResize(idx);
      break;

      case 103:
        if( Win[idx].User ) WinDestroyWindow( Win[idx].User );
        Win[idx].User = WinLoadDlg(Win[idx].Main, Win[idx].Main, SysView2tasks, 0L, ID_TASKS, NULL);
        Win[idx].Button = SHORT1FROMMP( mp1 );
        UserResize(idx);
        SystemTasks( idx, WinWindowFromID( Win[idx].User, ID_TASKSLIST ) );
      break;

      case 104:
        if( Win[idx].User ) WinDestroyWindow( Win[idx].User );
        Win[idx].User = WinLoadDlg(Win[idx].Main, Win[idx].Main, SysView2vm, 0L, ID_VIRTUALMEM, NULL);
        Win[idx].Button = SHORT1FROMMP( mp1 );
        UserResize(idx);
      break;

      case 105:
        if( Win[idx].User ) WinDestroyWindow( Win[idx].User );
        Win[idx].User = WinLoadDlg(Win[idx].Main, Win[idx].Main, SysView2procs, 0L, ID_PROCS, NULL);
        Win[idx].Button = SHORT1FROMMP( mp1 );
        UserResize(idx);
        SystemProcs( idx, WinWindowFromID(Win[idx].User, ID_PROCSLIST) );
      break;

      case 1066:
        pid = SHORT1FROMMP( mp2 );
      case 106:
        if( Win[idx].User ) WinDestroyWindow( Win[idx].User );
        Win[idx].User = WinLoadDlg(Win[idx].Main, Win[idx].Main, SysView2threads, 0L, ID_THREADS, NULL);
        Win[idx].Button = SHORT1FROMMP( mp1 );
        UserResize(idx);
        SystemThreads( idx, pid, WinWindowFromID(Win[idx].User, ID_THREADSLIST) );
      break;

      case DID_CANCEL:
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    default:
      return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
    }

    return( (MRESULT) FALSE );
}

MRESULT EXPENTRY SysView2about( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch( msg )
    {
    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
      case DID_OK:
        WinDismissDlg( hwnd, TRUE );
      break;

      default:
      break;
      }
    break;

    default:
      return WinDefDlgProc( hwnd, msg, mp1, mp2 );
    }

    return( (MRESULT) FALSE );
}

VOID    SystemOS2( INT idx )
{
    HWND       hItem = WinWindowFromID( Win[idx].User, ID_ICONITEM );
    HWND       hData = WinWindowFromID( Win[idx].User, ID_ICONDATA );
    USHORT     count = 0, loop = 0;
    ULONG      QSV[2] = {0};
    CHAR       item[8] = {0}, data[8] = {0};

    PostSignal( idx, SIG_USED );
    while( !TestSignal( idx, SIG_TERM ) )
      {
      for( count = 0; (count < 12) && (loop != 0xFFFF) && !TestSignal( idx, SIG_TERM ); count++ )
         {
         switch( loop )
            {
            case 0:
                  sprintf( item, "RAM" );
                  DosQuerySysInfo( QSV_TOTPHYSMEM, QSV_TOTRESMEM, QSV, 8 );
                  sprintf( data, "%d", (QSV[0] / 1024) - (QSV[1] / 1024) );
            break;
            case 1:
                  sprintf( item, "THREAD" );
                  sprintf( data, "%4d", SysThreads() );
            break;
            case 2:
                  sprintf( item, "MEM" );
                  DosQuerySysInfo( QSV_TOTAVAILMEM, QSV_TOTAVAILMEM, QSV, 4 );
                  sprintf( data, "%d", QSV[0] / 1024 );
            break;
            default:
                  loop = 0xFFFF;
            break;
            }
         if( loop != 0xFFFF)
            {
            WinPostWindowText( hItem, item );
            WinPostWindowText( hData, data );
            DosSleep( 250 );
            }
         }
      loop++ ;
      }

    PostSignal( idx, SIG_NULL );
}

MRESULT EXPENTRY SysView2minimized( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    INT     idx = FindWindow( GetTID() );

    switch( msg )
    {

    case WM_INITDLG:
      if( WaitSignal( idx, SIG_NULL ) )
        Win[idx].Tid = _beginthread( SystemOS2, NULL, 8192, idx );
      Win[idx].Tid = 0;
    break;

    case WM_DESTROY:
    case WM_CLOSE:
      PostSignal( idx, SIG_TERM );
    break;

    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
      case DID_CANCEL:
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
    }

    return( (MRESULT) FALSE );
}

VOID    SystemInfo( INT idx )
{
    HWND        hList = WinWindowFromID( Win[idx].User, ID_MACHINELIST );
    ULONG       QSV[2] = {0};
    ULONG       upSec = 0;
    ULONG       upMin = 0;
    ULONG       upHou = 0;
    ULONG       upDay = 0;
    CHAR        bufferL0[80] = {0}, bufferL1[80] = {0}, bufferL2[80] = {0}, buffer[80] = {0};
    USHORT      count = 0;
    ULONG       driveNumber = 0;
    ULONG       logicalDriveMap = 0;
    ULONG       drive = 0;
    INT         every10sec = 9;
    CHAR        deviceName[3] = {0};
    FSQBUFFER2  fsqBuffer2 = {0};
    ULONG       fsqBufLen = 0;

    struct
        {
        ULONG   volNumber;
        BYTE    lbLength;
        CHAR    label[12];
        } FSinfo = {0};
    struct
        {
        ULONG   FSidentifier;
        ULONG   sectorsPerUnit;
        ULONG   totalUnits;
        ULONG   availUnits;
        USHORT  bytesPerSector;
        } FSalloc = {0};

    PostSignal( idx, SIG_USED );

    for( count = 0; count < 28; count++ )
      WinPostMsg( hList, LM_INSERTITEM, (MPARAM) 0, (MPARAM) "" );

    WinPostMsg( hList, LM_SETITEMHEIGHT, (MPARAM) 24, (MPARAM) NULL );

    while( !TestSignal( idx, SIG_TERM ) )
      {
      DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, QSV, 4 );
      upSec = QSV[0] / 1000;
      upMin = QSV[0] / 60000;
      upHou = QSV[0] / 3600000;
      upDay = QSV[0] / 86400000;
      sprintf( bufferL0, "OS/2 release= %6s                       UpTime= %dd %dh %dm %ds",
                        Config.version, upDay, upHou % 24, upMin % 60, upSec % 60 );
      WinPostItemText( hList, 0, bufferL0 );

      DosQuerySysInfo( QSV_TOTPHYSMEM, QSV_TOTRESMEM, QSV, 8 );
      sprintf( bufferL1, "Physical memory (KB)= %d / %d", (QSV[0] / 1024) - (QSV[1] / 1024), (QSV[0] / 1024) );
      WinPostItemText( hList, 1, bufferL1 );

      sprintf( bufferL2, "Threads= %4d / %-4s                       Priority= %s",
                        SysThreads(), Config.threads, Config.priority );
      WinPostItemText( hList, 2, bufferL2 );

      every10sec++ ;
      if( every10sec > 10 )
        {
        every10sec = 1;
        DosQueryCurrentDisk( &driveNumber, &logicalDriveMap );

        for( drive = 2, count = 4; drive < 26; drive++ )
          if( logicalDriveMap & (1 << drive) )
            {
            memset( &FSinfo, 0, sizeof(FSinfo) );
            memset( &FSalloc, 0, sizeof(FSalloc) );

            DosQueryFSInfo( drive + 1, FSIL_VOLSER, &FSinfo, sizeof(FSinfo) );
            DosQueryFSInfo( drive + 1, FSIL_ALLOC, &FSalloc, sizeof(FSalloc) );

            memset( &fsqBuffer2, 0, sizeof(FSQBUFFER2) );
            sprintf( deviceName, "%c:", (char) ('A' + drive) );
            DosQueryFSAttach( deviceName, 0, FSAIL_QUERYNAME, &fsqBuffer2, &fsqBufLen );

            sprintf( buffer, "Drive %c%c[% 11s] = % 7d / %7d (KB)  %s", (char) ('A' + drive),   \
                   (Config.bootDrive == (char) ('A' + drive)) ? '*' :                           \
                            (Config.cdrom == (char) ('A' + drive)) ? '@' : ' ', FSinfo.label,   \
                   FSalloc.availUnits * FSalloc.sectorsPerUnit * FSalloc.bytesPerSector / 1024, \
                   FSalloc.totalUnits * FSalloc.sectorsPerUnit * FSalloc.bytesPerSector / 1024, \
                   (fsqBuffer2.iType == FSAT_REMOTEDRV) ? "remote" : "local " );

            WinPostItemText( hList, count++, buffer );
            }

        while( count < 28 )
            WinPostItemText( hList, count++, "" );
        }

      for( count = 0; (count < 10) && !TestSignal( idx, SIG_TERM ); count++ )
        DosSleep( 100 );
      }

    PostSignal( idx, SIG_NULL );
}

MRESULT EXPENTRY SysView2info( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    CHAR    Title[80] = "System View (0) - Information";
    INT     idx = FindWindow( GetTID() );
    HWND    hList = WinWindowFromID( Win[idx].User, ID_MACHINELIST );
    PSWP    pSwp = NULL;
    RECTL   rectl = {0};

    switch( msg )
    {
    case WM_INITDLG:
      sprintf( Title, "System View (%d) - Information", idx );
      WinSetWindowText( Win[idx].Frame, Title );

      if( WaitSignal( idx, SIG_NULL ) )
        Win[idx].Tid = _beginthread( SystemInfo, NULL, 8192, idx );
    break;

    case WM_DESTROY:
    case WM_CLOSE:
      PostSignal( idx, SIG_TERM );
    break;

    case WM_WINDOWPOSCHANGED:
      pSwp = mp1;

      WinSetWindowPos( hList, HWND_TOP, pSwp->x, pSwp->y, pSwp->cx, pSwp->cy, SWP_MOVE|SWP_SIZE );
    break;

    case WM_COMMAND:
      switch( SHORT1FROMMP( mp1 ) )
      {
      case DID_CANCEL:
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
    }

    return( (MRESULT) FALSE );
}


VOID  GetTasksList( INT idx, TasksList_t **pList, ULONG *INDEXES )
{
    PSWBLOCK    listBlock = NULL;
    size_t      listSize = 0;
    ULONG       items = 0;
    ULONG       index = 0;
    TasksList_t *tList = NULL;

    items = WinQuerySwitchList( Win[idx].Hab, NULL, 0 );
    listSize = ( items * sizeof(SWENTRY) ) + sizeof(HSWITCH);
    listBlock = malloc( listSize );
    items = WinQuerySwitchList( Win[idx].Hab, listBlock, listSize );

    for( items = 0; items < listBlock->cswentry; items++ )
      if( listBlock->aswentry[items].swctl.idProcess )
        {
        index++ ;
        tList = realloc( tList, index * sizeof(TasksList_t) );
        if( tList != NULL )
          {
          tList[index-1].hswitch    = listBlock->aswentry[items].hswitch;
          tList[index-1].pid        = listBlock->aswentry[items].swctl.idProcess;
          tList[index-1].sid        = listBlock->aswentry[items].swctl.idSession;
          tList[index-1].hwnd       = listBlock->aswentry[items].swctl.hwnd;
          strncpy( tList[index-1].title, listBlock->aswentry[items].swctl.szSwtitle, 256 );
          }
        }
   *pList    = tList;
   *INDEXES = index;
   free( listBlock );
}

VOID    SystemTasks( INT idx, HWND hTasksList )
{
    ULONG   INDEXES = 0;
    LONG    index = 0;
    CHAR    item[256];

    WinSendMsg( hTasksList, LM_DELETEALL, NULL, NULL );
    if( Win[idx].TasksList )
      {
      free( Win[idx].TasksList );
      Win[idx].TasksList = NULL;
      }
    GetTasksList( idx, &(Win[idx].TasksList), &INDEXES );
    if( INDEXES > 0 )
      for( index = 0; index < INDEXES; index++ )
        {
        sprintf( item, "%5d %5d %8X  %s", Win[idx].TasksList[index].pid, Win[idx].TasksList[index].sid, Win[idx].TasksList[index].hwnd, Win[idx].TasksList[index].title );
        WinSendMsg( hTasksList, LM_INSERTITEM, LIT_END, item );
        }
}

MRESULT EXPENTRY SysView2tasks( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    PSWP    pSwp = NULL;
    RECTL   rectl = {0};
    LONG    width = 0, X = 0;
    LONG    height = 0, Y = 0, hTitle = 0;
    LONG    index = 0;
    CHAR    Title[80] = "System View (0) - Tasks list";
    INT     idx = FindWindow( GetTID() );
    HWND    hRefreshTask = WinWindowFromID(Win[idx].User,ID_REFRESHTASK);
    HWND    hSwitchTask = WinWindowFromID(Win[idx].User,ID_SWITCHTASK);
    HWND    hCloseTask = WinWindowFromID(Win[idx].User,ID_CLOSETASK);
    HWND    hQuitTask = WinWindowFromID(Win[idx].User,ID_QUITTASK);
    HWND    hKillTask = WinWindowFromID(Win[idx].User,ID_KILLTASK);
    HWND    hTitleTask = WinWindowFromID(Win[idx].User,ID_TITLETASK);
    HWND    hTasksList = WinWindowFromID(Win[idx].User, ID_TASKSLIST);

    switch( msg )
    {
    case WM_INITDLG:
      sprintf( Title, "System View (%d) - Tasks list", idx );
      WinSetWindowText( Win[idx].Frame, Title );
      Win[idx].Tid = 0;
    break;

    case WM_DESTROY:
    case WM_CLOSE:
      WinSendMsg( hTasksList, LM_DELETEALL, NULL, NULL );
      if( Win[idx].TasksList )
        {
        free( Win[idx].TasksList );
        Win[idx].TasksList = NULL;
        }
    break;

    case WM_WINDOWPOSCHANGED:
      pSwp = mp1;

      X = pSwp->x + 16;
      Y = pSwp->y + 4;

      WinQueryWindowRect( hRefreshTask, &rectl );
      width  = rectl.xRight - rectl.xLeft;
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hRefreshTask, HWND_TOP, X, Y, width, height, SWP_MOVE );
      X += (width + 16);

      WinQueryWindowRect( hSwitchTask, &rectl );
      width  = rectl.xRight - rectl.xLeft;
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hSwitchTask, HWND_TOP, X, Y, width, height, SWP_MOVE );
      X += (width + 16);

      WinQueryWindowRect( hCloseTask, &rectl );
      width  = rectl.xRight - rectl.xLeft;
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hCloseTask, HWND_TOP, X, Y, width, height, SWP_MOVE );
      X += (width + 16);

      WinQueryWindowRect( hQuitTask, &rectl );
      width  = rectl.xRight - rectl.xLeft;
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hQuitTask, HWND_TOP, X, Y, width, height, SWP_MOVE );
      X += (width + 16);

      WinQueryWindowRect( hKillTask, &rectl );
      width  = rectl.xRight - rectl.xLeft;
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hKillTask, HWND_TOP, X, Y, width, height, SWP_MOVE );

      sprintf( Title, "% 5s % 5s % 8s    %s", "PID", "SID", "HWINDOW", "Title" );
      WinSetWindowText( hTitleTask, Title );
      WinQueryWindowRect( hTitleTask, &rectl );
      hTitle = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hTitleTask, HWND_TOP, pSwp->x, pSwp->cy - hTitle, pSwp->cx, hTitle, SWP_MOVE|SWP_SIZE );

      WinSetWindowPos( hTasksList, HWND_TOP, pSwp->x, Y + height + 4, pSwp->cx, pSwp->cy - hTitle - height - 8, SWP_MOVE|SWP_SIZE );
    break;

    case WM_COMMAND:
      switch( SHORT1FROMMP(mp1) )
      {
      case ID_REFRESHTASK:
        SystemTasks( idx, hTasksList );
        break;

      case ID_SWITCHTASK:
        index = (LONG) WinSendMsg( hTasksList, LM_QUERYSELECTION, LIT_FIRST, NULL );
        while( index != LIT_NONE )
          {
          WinSwitchToProgram( Win[idx].TasksList[index].hswitch );
          index = (LONG) WinSendMsg( hTasksList, LM_QUERYSELECTION, index, NULL );
          }
      break;

      case ID_CLOSETASK:
        index = (LONG) WinSendMsg( hTasksList, LM_QUERYSELECTION, LIT_FIRST, NULL );
        while( index != LIT_NONE )
          {
          WinPostMsg( Win[idx].TasksList[index].hwnd, WM_CLOSE, 0L, 0L );
          index = (LONG) WinSendMsg( hTasksList, LM_QUERYSELECTION, index, NULL );
          }
      break;

      case ID_QUITTASK:
        index = (LONG) WinSendMsg( hTasksList, LM_QUERYSELECTION, LIT_FIRST, NULL );
        while( index != LIT_NONE )
          {
          WinPostMsg( Win[idx].TasksList[index].hwnd, WM_QUIT, 0L, 0L );
          index = (LONG) WinSendMsg( hTasksList, LM_QUERYSELECTION, index, NULL );
          }
      break;

      case ID_KILLTASK:
        index = (LONG) WinSendMsg( hTasksList, LM_QUERYSELECTION, LIT_FIRST, NULL );
        while( index != LIT_NONE )
          {
          DosKillProcess( DKP_PROCESS, Win[idx].TasksList[index].pid );
          index = (LONG) WinSendMsg( hTasksList, LM_QUERYSELECTION, index, NULL );
          }
      break;

      case DID_CANCEL:
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    case WM_CONTROL:
      switch( SHORT1FROMMP(mp1) )
      {
      case ID_TASKSLIST:
        if( (SHORT2FROMMP(mp1) == LN_ENTER)
        &&( (index = (LONG) WinSendMsg( hTasksList, LM_QUERYSELECTION, LIT_FIRST, NULL)) != LIT_NONE ))
          WinSwitchToProgram( Win[idx].TasksList[index].hswitch );
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }

    default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
    }

    return( (MRESULT) FALSE );
}

ULONG   GetSwapFileSize( VOID )
{
    FILESTATUS3 fileStatus = {0};

    DosQueryPathInfo( Config.swapFile, FIL_STANDARD, &fileStatus, sizeof(FILESTATUS3) );

    return( fileStatus.cbFile );
}

VOID    SystemVirtualMem( INT idx )
{
    HWND    hVmList = WinWindowFromID(Win[idx].User, ID_VMLIST);
    ULONG   QSV = 0, vm = 0;
    CHAR    virtualMem[64] = {0};
    SHORT   index = 0, count = 0;

    PostSignal( idx, SIG_USED );

    DosQuerySysInfo( QSV_TOTAVAILMEM, QSV_TOTAVAILMEM, &vm, 4 );
    sprintf( virtualMem, "%10d %+9d %10d", vm, 0, GetSwapFileSize() );
    WinPostMsg( hVmList, LM_INSERTITEM, LIT_END, virtualMem );

    while( !TestSignal( idx, SIG_TERM ) )
      {
      DosQuerySysInfo( QSV_TOTAVAILMEM, QSV_TOTAVAILMEM, &QSV, 4 );

      if( vm != QSV )
        {
        sprintf( virtualMem, "%10d %+9d %10d", QSV, (QSV - vm), GetSwapFileSize() );
        WinPostMsg( hVmList, LM_INSERTITEM, LIT_END, virtualMem );
        vm = QSV;

        if( index > 128 )
          {
          WinPostMsg( hVmList, LM_DELETEALL, 0, 0 );
          index = 0;
          }
        else
          {
          WinPostMsg( hVmList, LM_SETTOPINDEX, index, 0 );
          index++ ;
          }
        }

      for( count = 0; (count < 10) && !TestSignal( idx, SIG_TERM ); count++ )
        DosSleep( 100 );
      }
    PostSignal( idx, SIG_NULL );
}

MRESULT EXPENTRY SysView2vm( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    PSWP    pSwp = NULL;
    RECTL   rectl = {0};
    LONG    height = 0;
    CHAR    Title[80] = "System View (0) - Virtual memory";
    INT     idx = FindWindow( GetTID() );
    HWND    hVmTitle = WinWindowFromID(Win[idx].User, ID_VMTITLE);
    HWND    hVmList = WinWindowFromID(Win[idx].User, ID_VMLIST);

    switch( msg )
    {
    case WM_INITDLG:
      sprintf( Title, "System View (%d) - Virtual memory", idx );
      WinSetWindowText( Win[idx].Frame, Title );

      if( WaitSignal( idx, SIG_NULL ) )
        Win[idx].Tid = _beginthread( SystemVirtualMem, NULL, 8192, idx );
    break;

    case WM_DESTROY:
    case WM_CLOSE:
      PostSignal( idx, SIG_TERM );
    break;

    case WM_WINDOWPOSCHANGED:
      pSwp = mp1;

      sprintf( Title, "% 10s % 9s % 10s", "Available", "Delta", "SwapSize" );
      WinSetWindowText( hVmTitle, Title );
      WinQueryWindowRect( hVmTitle, &rectl );
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hVmTitle, HWND_TOP, pSwp->x, pSwp->cy - height, pSwp->cx, height, SWP_MOVE|SWP_SIZE );

      WinSetWindowPos( hVmList, HWND_TOP, pSwp->x, pSwp->y, pSwp->cx, pSwp->cy - height, SWP_MOVE|SWP_SIZE );
    break;

    case WM_COMMAND:
      switch( SHORT1FROMMP(mp1) )
      {
      case DID_CANCEL:
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
    }

    return( (MRESULT) FALSE );
}


VOID    EXPENTRY SystemProcs( INT idx, HWND hList )
{
    ULONG   INDEXES = 0;
    LONG    index = 0;
    CHAR    item[256], PNAME[64], PEXT[_MAX_EXT], PTIME[12], PTITLE[256];

    WinSendMsg( hList, LM_DELETEALL, NULL, NULL );
    if( Win[idx].ProcsList )
      {
      free( Win[idx].ProcsList );
      Win[idx].ProcsList = NULL;
      }
    SysProcTree( &(Win[idx].ProcsList), &INDEXES );
    if( Win[idx].ProcsList && (INDEXES > 0) )
      for( index = 0; index < INDEXES; index++ )
        {
        TimeStr( Win[idx].ProcsList[index].CPU, PTIME );

        if( Win[idx].ProcsList[index].PTYPE == 1 )
          {
          WinQueryTaskTitle( Win[idx].ProcsList[index].SID, PTITLE, sizeof(PTITLE) );

          sprintf( PNAME, "%s [%s]", PTYPES[ Win[idx].ProcsList[index].PTYPE ], PTITLE );
          }
        else
            {
            CHAR    drive[_MAX_DRIVE];
            CHAR    dir[_MAX_DIR];

            DosQueryModuleName( Win[idx].ProcsList[index].HMODULE, sizeof(PTITLE), PTITLE );
            _splitpath( PTITLE, drive, dir, PNAME, PEXT);
            strcat( PNAME, PEXT );
            }

        sprintf( item, "%5hd %5hd %5d %-4s %5hd %5x%12s  %s",
                        Win[idx].ProcsList[index].PID,
                        Win[idx].ProcsList[index].PPID,
                        Win[idx].ProcsList[index].SID,
                        PTYPES[ Win[idx].ProcsList[index].PTYPE ],
                        Win[idx].ProcsList[index].THREADS,
                        Win[idx].ProcsList[index].PRIORITY,
                        PTIME,
                        PNAME );
        WinSendMsg( hList, LM_INSERTITEM, LIT_END, item );
        }
}

MRESULT EXPENTRY SysView2procs( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    PSWP    pSwp = NULL;
    RECTL   rectl = {0};
    LONG    width = 0, X = 0;
    LONG    height = 0, Y = 0, hTitle = 0;
    LONG    index = 0;
    CHAR    Title[80] = "System View (0) - Processes list";
    INT     idx = FindWindow( GetTID() );
    HWND    hRefreshProc = WinWindowFromID(Win[idx].User,ID_REFRESHPROC);
    HWND    hDetailProc = WinWindowFromID(Win[idx].User,ID_DETAILPROC);
    HWND    hKillProc = WinWindowFromID(Win[idx].User,ID_KILLPROC);
    HWND    hTitleProc = WinWindowFromID(Win[idx].User, ID_TITLEPROC);
    HWND    hProcsList = WinWindowFromID(Win[idx].User, ID_PROCSLIST);

    switch( msg )
    {
    case WM_INITDLG:
      sprintf( Title, "System View (%d) - Processes list", idx );
      WinSetWindowText( Win[idx].Frame, Title );
      Win[idx].Tid = 0;
    break;

    case WM_DESTROY:
    case WM_CLOSE:
      WinSendMsg( hProcsList, LM_DELETEALL, NULL, NULL );
      if( Win[idx].ProcsList )
        {
        free( Win[idx].ProcsList );
        Win[idx].ProcsList = NULL;
        }
    break;

    case WM_WINDOWPOSCHANGED:
      pSwp = mp1;

      X = pSwp->x + 16;
      Y = pSwp->y + 4;

      WinQueryWindowRect( hRefreshProc, &rectl );
      width  = rectl.xRight - rectl.xLeft;
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hRefreshProc, HWND_TOP, X, Y, width, height, SWP_MOVE );
      X += (width + 16);

      WinQueryWindowRect( hDetailProc, &rectl );
      width  = rectl.xRight - rectl.xLeft;
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hDetailProc, HWND_TOP, X, Y, width, height, SWP_MOVE );
      X += (width + 16);

      WinQueryWindowRect( hKillProc, &rectl );
      width  = rectl.xRight - rectl.xLeft;
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hKillProc, HWND_TOP, X, Y, width, height, SWP_MOVE );

      sprintf( Title, "%6s %5s %9s %5s %5s %11s    Name", "PID", "PPID", "Session", "Thr", "Prio", "CPU Time" );
      WinSetWindowText( hTitleProc, Title );
      WinQueryWindowRect( hTitleProc, &rectl );
      hTitle = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hTitleProc, HWND_TOP, pSwp->x, pSwp->cy - hTitle, pSwp->cx, hTitle, SWP_MOVE|SWP_SIZE );

      WinSetWindowPos( hProcsList, HWND_TOP, pSwp->x, Y + height + 4, pSwp->cx, pSwp->cy - hTitle - height - 8, SWP_MOVE|SWP_SIZE );
    break;

    case WM_COMMAND:
      switch( SHORT1FROMMP(mp1) )
      {
      case ID_REFRESHPROC:
        SystemProcs( idx, hProcsList );
        break;

      case ID_DETAILPROC:
        if( (index = (LONG) WinSendMsg( hProcsList, LM_QUERYSELECTION, LIT_FIRST, NULL)) != LIT_NONE )
            WinSendMsg( Win[idx].Icons, WM_COMMAND, (MPARAM) 1066, (MPARAM) Win[idx].ProcsList[index].PID );
      break;

      case ID_KILLPROC:
        index = (LONG) WinSendMsg( hProcsList, LM_QUERYSELECTION, LIT_FIRST, NULL );
        while( index != LIT_NONE )
          {
          DosKillProcess( DKP_PROCESS, Win[idx].ProcsList[index].PID );
          index = (LONG) WinSendMsg( hProcsList, LM_QUERYSELECTION, index, NULL );
          }
      break;

      case DID_CANCEL:
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    case WM_CONTROL:
      switch( SHORT1FROMMP(mp1) )
      {
      case ID_PROCSLIST:
        if( (SHORT2FROMMP(mp1) == LN_ENTER)
        &&( (index = (LONG) WinSendMsg( hProcsList, LM_QUERYSELECTION, LIT_FIRST, NULL)) != LIT_NONE ))
          WinSendMsg( Win[idx].Icons, WM_COMMAND, (MPARAM) 1066, (MPARAM) Win[idx].ProcsList[index].PID );
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
    }

    return( (MRESULT) FALSE );
}


VOID    EXPENTRY SystemPidTid( INT idx, USHORT pid, HWND hList )
{
    ULONG   INDEXES = 0;
    LONG    index = 0;
    CHAR    item[256], PNAME[64], PEXT[_MAX_EXT], PTITLE[256];

    WinSendMsg( hList, LM_DELETEALL, NULL, NULL );
    if( Win[idx].ProcsList )
      {
      free( Win[idx].ProcsList);
      Win[idx].ProcsList= NULL;
      }
    SysProcTree( &(Win[idx].ProcsList), &INDEXES );
    if( Win[idx].ProcsList && (INDEXES > 0) )
      for( index = 0; index < INDEXES; index++ )
        {
        if( Win[idx].ProcsList[index].PTYPE == 1 )
          WinQueryTaskTitle( Win[idx].ProcsList[index].SID, PNAME, sizeof(PNAME) );
        else
            {
            CHAR    drive[_MAX_DRIVE];
            CHAR    dir[_MAX_DIR];

            DosQueryModuleName( Win[idx].ProcsList[index].HMODULE, sizeof(PTITLE), PTITLE );
            _splitpath( PTITLE, drive, dir, PNAME, PEXT);
            strcat( PNAME, PEXT );
            }

        sprintf( item, "%5hd %s", Win[idx].ProcsList[index].PID, PNAME );
        WinSendMsg( hList, LM_INSERTITEM, LIT_END, item );

        if( Win[idx].ProcsList[index].PID == pid )
          WinSendMsg( hList, LM_SELECTITEM, (MPARAM) index, (MPARAM) TRUE );
        }
}

VOID    EXPENTRY SystemThreads( INT idx, USHORT pid, HWND hList )
{
    ULONG   INDEXES = 0;
    LONG    index = 0;
    CHAR    item[256], SYSTIME[12], USRTIME[12];

    SystemPidTid( idx, pid, WinWindowFromID(Win[idx].User, ID_PIDTIDLIST) );

    WinSendMsg( hList, LM_DELETEALL, NULL, NULL );
    if( Win[idx].ThreadsList )
      {
      free( Win[idx].ThreadsList );
      Win[idx].ThreadsList = NULL;
      }

    SysThreadsList( &(Win[idx].ThreadsList), &INDEXES, pid );
    if( Win[idx].ThreadsList && (INDEXES > 0) )
      for( index = 0; index < INDEXES; index++ )
        {
        TimeStr( Win[idx].ThreadsList[index].SYSCPU, SYSTIME );
        TimeStr( Win[idx].ThreadsList[index].USRCPU, USRTIME );

        sprintf( item, "%5hd %5hd  %8X%5X%12s%12s   %s",
                        Win[idx].ThreadsList[index].TID,
                        Win[idx].ThreadsList[index].TSID,
                        Win[idx].ThreadsList[index].BLKID,
                        Win[idx].ThreadsList[index].PRIORITY,
                        SYSTIME,
                        USRTIME,
                        TSTATUS[ Win[idx].ThreadsList[index].STATUS ] );
        WinSendMsg( hList, LM_INSERTITEM, LIT_END, item );
        }
}

MRESULT EXPENTRY SysView2threads( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    PSWP    pSwp = NULL;
    RECTL   rectl = {0};
    LONG    width = 0, X = 0, wTitle = 0;
    LONG    height = 0, Y = 0, hTitle = 0;
    LONG    index = 0;
    CHAR    Title[80] = "System View (0) - Threads list";
    INT     idx = FindWindow( GetTID() );
    HWND    hRefreshThread = WinWindowFromID(Win[idx].User,ID_REFRESHTHREAD);
    HWND    hTitlePidTid = WinWindowFromID(Win[idx].User, ID_TITLEPIDTID);
    HWND    hPidTidList = WinWindowFromID(Win[idx].User, ID_PIDTIDLIST);
    HWND    hTitleThread = WinWindowFromID(Win[idx].User, ID_TITLETHREAD);
    HWND    hThreadsList = WinWindowFromID(Win[idx].User, ID_THREADSLIST);

    switch( msg )
    {
    case WM_INITDLG:
      sprintf( Title, "System View (%d) - Threads list", idx );
      WinSetWindowText( Win[idx].Frame, Title );
      Win[idx].Tid = 0;
    break;

    case WM_DESTROY:
    case WM_CLOSE:
      WinSendMsg( hPidTidList, LM_DELETEALL, NULL, NULL );
      WinSendMsg( hThreadsList, LM_DELETEALL, NULL, NULL );
      if( Win[idx].ProcsList )
        {
        free( Win[idx].ProcsList );
        Win[idx].ProcsList = NULL;
        }
      if( Win[idx].ThreadsList )
        {
        free( Win[idx].ThreadsList );
        Win[idx].ThreadsList = NULL;
        }
    break;

    case WM_WINDOWPOSCHANGED:
      pSwp = mp1;

      X = pSwp->x + 16;
      Y = pSwp->y + 4;

      WinQueryWindowRect( hRefreshThread, &rectl );
      width  = rectl.xRight - rectl.xLeft;
      height = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hRefreshThread, HWND_TOP, X, Y, width, height, SWP_MOVE );

      sprintf( Title, "%6s    Name", "PID" );
      WinSetWindowText( hTitlePidTid, Title );

      sprintf( Title, "%6s %5s %8s %5s %11s %11s   %s", "TID", "TSID", "Block ID", "Prio", "System CPU", "User CPU", "Status" );
      WinSetWindowText( hTitleThread, Title );

      WinQueryWindowRect( hTitlePidTid, &rectl );
      wTitle = rectl.xRight - rectl.xLeft;
      hTitle = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hTitlePidTid, HWND_TOP, pSwp->x, pSwp->cy - hTitle, wTitle, hTitle, SWP_MOVE|SWP_SIZE );

      WinQueryWindowRect( hTitleThread, &rectl );
      hTitle = rectl.yTop - rectl.yBottom;
      WinSetWindowPos( hTitleThread, HWND_TOP, wTitle, pSwp->cy - hTitle, pSwp->cx - wTitle, hTitle, SWP_MOVE|SWP_SIZE );

      WinSetWindowPos( hPidTidList, HWND_TOP, pSwp->x, Y + height + 4, wTitle, pSwp->cy - hTitle - height - 8, SWP_MOVE|SWP_SIZE );

      WinSetWindowPos( hThreadsList, HWND_TOP, wTitle, Y + height + 4, pSwp->cx - wTitle, pSwp->cy - hTitle - height - 8, SWP_MOVE|SWP_SIZE );
    break;

    case WM_CONTROL:
      switch( SHORT1FROMMP(mp1) )
      {
      case ID_PIDTIDLIST:
        if( (SHORT2FROMMP(mp1) == LN_ENTER)
        &&( (index = (LONG) WinSendMsg( hPidTidList, LM_QUERYSELECTION, LIT_FIRST, NULL)) != LIT_NONE ))
          SystemThreads( idx, Win[idx].ProcsList[index].PID, hThreadsList );
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    case WM_COMMAND:
      switch( SHORT1FROMMP(mp1) )
      {
      case ID_REFRESHTHREAD:
        if( (index = (LONG) WinSendMsg( hPidTidList, LM_QUERYSELECTION, LIT_FIRST, NULL)) != LIT_NONE )
          SystemThreads( idx, Win[idx].ProcsList[index].PID, hThreadsList );
        break;

      case DID_CANCEL:
      break;

      default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
      }
    break;

    default:
        return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
    }

    return( (MRESULT) FALSE );
}

VOID    ReadConfig( VOID )
{
    HFILE   FileHandle = (HFILE) -1;
    ULONG   Category;        /* Device category */
    ULONG   Function;        /* Device function */
    PVOID   ParmList;        /* Command-specific argument list */
    ULONG   ParmLengthMax;   /* Command arguments list max length */
    ULONG   ParmLengthInOut; /* Command arguments length (returned) */
    USHORT  DataArea[2]={0}; /* Data area */
    ULONG   DataLengthMax;   /* Data area maximum length */
    ULONG   DataLengthInOut; /* Data area length (returned) */
    ULONG   QSV[3] = {0};
    CHAR    CONFIG_SYS[16] = {0};
    FILE    *stream = NULL;
    CHAR    buffer[1024] = {0};
    CHAR    *pBuffer = NULL;
    APIRET  rc = NO_ERROR;

    if( (FileHandle = (HFILE) _open( "CD-ROM2$", O_RDONLY|O_BINARY, S_IREAD )) != (HFILE) -1 )
        {
        Category = 0x82;
        Function = 0x60;
        ParmList = 0;
        ParmLengthInOut = 0;
        ParmLengthMax = 0;
        DataLengthInOut = (ULONG) sizeof(DataArea);
        DataLengthMax = (ULONG) sizeof(DataArea);

        rc = DosDevIOCtl(   FileHandle, Category, Function, ParmList,
                            ParmLengthMax, &ParmLengthInOut, DataArea,
                            DataLengthMax, &DataLengthInOut );

        if( (rc == NO_ERROR) && (DataArea[0] > 0) )
            Config.cdrom = 'A' + DataArea[1];

        _close( (int) FileHandle );
        }

    DosQuerySysInfo( QSV_VERSION_MAJOR, QSV_VERSION_REVISION, QSV, 12 );
    sprintf( Config.version, "%d.%d%c", QSV[0], QSV[1], ('a' + QSV[2]) );

    DosQuerySysInfo( QSV_BOOT_DRIVE, QSV_DYN_PRI_VARIATION, QSV, 8 );
    Config.bootDrive = (CHAR) (--QSV[0] + 'A');
    sprintf( Config.priority, "%s", ( QSV[1] == 0 ) ? "ABSOLUTE" : "DYNAMIC" );

    sprintf( CONFIG_SYS, "%c:\\CONFIG.SYS", Config.bootDrive );

    stream = fopen( CONFIG_SYS, "r" );

    while( fscanf( stream, "%s", buffer ) != EOF )
      {
      pBuffer = strupr( buffer );

      if( !strlen(Config.swapFile) )
        {
        sscanf( pBuffer, "SWAPPATH=%s", Config.swapFile );
        sscanf( pBuffer, "SWAPPATH =%s", Config.swapFile );
        }
      if( !strlen(Config.threads) )
        {
        sscanf( pBuffer, "THREADS=%s", Config.threads );
        sscanf( pBuffer, "THREADS =%s", Config.threads );
        }
      }
    fclose( stream );

    if( strlen( Config.swapFile ) )
      sprintf( Config.swapFile, "%s\\SWAPPER.DAT", Config.swapFile );
    else
      sprintf( Config.swapFile, "%c:\\OS2\\SYSTEM\\SWAPPER.DAT", Config.bootDrive );
}


INT     main( VOID )
{
    DosError(FERR_DISABLEHARDERR|FERR_DISABLEEXCEPTION);
    ReadConfig();
    NewWindow(NULL);
    DosError(FERR_ENABLEHARDERR|FERR_ENABLEEXCEPTION);
    return(0);
}

