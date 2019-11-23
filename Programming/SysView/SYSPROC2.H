typedef struct
    {
    USHORT  PID;
    USHORT  PPID;
    ULONG   PTYPE;
    ULONG   STATUS;
    ULONG   SID;
    USHORT  HMODULE;
    USHORT  THREADS;
    ULONG   CPU;
    USHORT  PRIORITY;
    } ProcsList_t;

typedef struct
    {
    USHORT  TID;
    USHORT  TSID;
    ULONG   BLKID;
    ULONG   PRIORITY;
    ULONG   SYSCPU;
    ULONG   USRCPU;
    UCHAR   STATUS;
    } ThreadsList_t;

//  See PMSHL.H
static  const   CHAR    *PTYPES[] = { "SYS", "VDM", "VIO", "PM", "DET", "GRP", "DLL", "WIN" };

static  const   CHAR    *TSTATUS[] = { NULL, "Ready", "Blocked", NULL, "CritSec",
                                       "Running", NULL, NULL, NULL, "Loaded" };

VOID    TimeStr( ULONG time, CHAR *buffer );
ULONG   SysThreads( VOID );
VOID    SysProcTree( ProcsList_t **pProcsList, ULONG *INDEXES );
VOID    SysThreadsList( ThreadsList_t **pThreadsList, ULONG *INDEXES, USHORT pid );

