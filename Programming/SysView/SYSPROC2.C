#define INCL_PM
#define INCL_DOSPROCESS
#define INCL_DOSMODULEMGR
#define INCL_DOSMEMMGR
#include <OS2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SysProc2.h"

#if defined(__32BIT__)
  #define PTR(ptr, ofs)  ((void *) ((char *) (ptr) + (ofs)))
#else
  #define DosQueryModuleName DosGetModName
  #define APIENTRY16 APIENTRY
  #if defined(M_I86SM) || defined(M_I86MM)
    #define PTR(ptr, ofs)  ((void *) ((char *) (ptr) + (ofs)))
  #else
    #define PTR(ptr, ofs)  ((void *) ((char *) (((ULONG) procstat & 0xFFFF0000) | (USHORT) (ptr)) + (ofs)))
    /* kludge to transform 0:32 into 16:16 pointer in this special case */
  #endif
#endif



/* DosQProcStatus() = DOSCALLS.154 */
USHORT APIENTRY16 DosQProcStatus(PVOID pBuf, USHORT cbBuf);

/* DosGetPrty = DOSCALLS.9 */
USHORT APIENTRY16 DosGetPrty(USHORT usScope, PUSHORT pusPriority, USHORT pid);


struct procstat
    {
    ULONG  summary;
    ULONG  processes;
    ULONG  semaphores;
    ULONG  unknown1;
    ULONG  sharedmemory;
    ULONG  modules;
    ULONG  unknown2;
    ULONG  unknown3;
    };

struct summary
    {
    ULONG  cThreads;
    ULONG  unknown1;
    ULONG  unknown2;
    };


struct process
    {
    ULONG  type;
    ULONG  threadlist;
    USHORT processid;
    USHORT parentid;
    ULONG  sessiontype;
    ULONG  status;      // see status #define's below
    ULONG  sessionid;
    USHORT modulehandle;
    USHORT threads;
    ULONG  reserved1;
    ULONG  reserved2;
    USHORT semaphores;
    USHORT dlls;
    USHORT shrmems;
    USHORT reserved3;
    ULONG  semlist;
    ULONG  dlllist;
    ULONG  shrmemlist;
    ULONG  reserved4;
    };

#define STAT_EXITLIST 0x01
#define STAT_EXIT1    0x02
#define STAT_EXITALL  0x04
#define STAT_PARSTAT  0x10
#define STAT_SYNCH    0x20
#define STAT_DYING    0x40
#define STAT_EMBRYO   0x80


struct thread
    {
    ULONG  type;
    USHORT threadid;
    USHORT threadslotid;
    ULONG  blockid;
    ULONG  priority;
    ULONG  systime;
    ULONG  usertime;
    UCHAR  status;      // see status #define's below
    UCHAR  reserved1;
    USHORT reserved2;
};

#define TSTAT_READY   1
#define TSTAT_BLOCKED 2
#define TSTAT_RUNNING 5
#define TSTAT_LOADED  9


struct module
    {
    ULONG  nextmodule;
    USHORT modhandle;
    USHORT modtype;
    ULONG  submodules;
    ULONG  segments;
    ULONG  reserved;
    ULONG  namepointer;
    USHORT submodule[1];    // varying, see member submodules
    };


struct semaphore
    {
    ULONG  nextsem;
    USHORT owner;
    UCHAR  flag;
    UCHAR  refs;
    UCHAR  requests;
    UCHAR  reserved1;
    USHORT reserved2;
    USHORT index;
    USHORT dummy;
    UCHAR  name[1];     // varying
    };


struct shmem
    {
    ULONG  nextseg;
    USHORT handle;
    USHORT selector;
    USHORT refs;
    UCHAR  name[1];     // varying
    };



VOID    TimeStr( ULONG time, CHAR *buffer )
{
    ULONG seconds, hundredths;

    seconds = time / 32;
    hundredths = (time % 32) * 100 / 32;

    sprintf( buffer, "%ld:%02ld.%02ld", seconds / 60, seconds % 60, hundredths );
}


ULONG   SysThreads( VOID )
{
    struct  procstat *pProcStat = NULL;
    struct  summary *pSummary = NULL;
    ULONG   cThreads = 0;

    pProcStat = malloc(0x8000);
    DosQProcStatus( pProcStat, 0x8000 );
    pSummary = PTR( pProcStat->summary, 0 );
    cThreads = pSummary->cThreads;
    free( pProcStat );

    return( cThreads );
}

ProcsList_t *SysProcTree2( ProcsList_t *PL, ULONG *INDEXES, struct procstat *procstat, USHORT pid )
{
    ProcsList_t     *PS = PL;
    struct  process *proc = NULL;
    struct  thread  *pThread = NULL;
    INT     t = 0;

    for(proc = PTR(procstat->processes, 0);
        proc->type != 3;    // not sure if there isn't another termination method
        proc = PTR(proc->threadlist, proc->threads * sizeof(struct thread)) )
        {
        if( (proc->parentid == pid) && (proc->type != 0) )
          {
          (*INDEXES)++ ;

          PL = (ProcsList_t*) realloc( PL, sizeof(ProcsList_t) * (*INDEXES) );

          if( PL != NULL )
            {
            PL[(*INDEXES) - 1].PID      = proc->processid;
            PL[(*INDEXES) - 1].PPID     = proc->parentid;
            PL[(*INDEXES) - 1].PTYPE    = proc->sessiontype;
            PL[(*INDEXES) - 1].STATUS   = proc->status;
            PL[(*INDEXES) - 1].SID      = proc->sessionid;
            PL[(*INDEXES) - 1].HMODULE  = proc->modulehandle;
            PL[(*INDEXES) - 1].THREADS  = proc->threads;

            DosGetPrty( PRTYS_PROCESS, &(PL[(*INDEXES) - 1].PRIORITY), proc->processid );

            for( PL[(*INDEXES) - 1].CPU = 0, t = 0, pThread = PTR(proc->threadlist, 0);
                 t < proc->threads;
                 t++, pThread++ )
                    PL[(*INDEXES) - 1].CPU += (pThread->systime + pThread->usertime);
            }

          proc->type = 0;  // kludge, mark it as already printed !

          PS = SysProcTree2( PL, INDEXES, procstat, proc->processid );
          PL = PS;
          }
        }

    if ( pid != 0 )
      return( PS );

  /* if at the root level, check for those processes that have lost *
   * their parent process and show them as if they were childs of 0 */

    for( proc = PTR(procstat->processes, 0);
         proc->type != 3;
         proc = PTR(proc->threadlist, proc->threads * sizeof(struct thread)) )
        {
        if( proc->type != 0 )
          {
          (*INDEXES)++ ;

          PL = (ProcsList_t*) realloc( PL, sizeof(ProcsList_t) * (*INDEXES) );

          if( PL != NULL )
            {
            PL[(*INDEXES) - 1].PID      = proc->processid;
            PL[(*INDEXES) - 1].PPID     = proc->parentid;
            PL[(*INDEXES) - 1].PTYPE    = proc->sessiontype;
            PL[(*INDEXES) - 1].STATUS   = proc->status;
            PL[(*INDEXES) - 1].SID      = proc->sessionid;
            PL[(*INDEXES) - 1].HMODULE  = proc->modulehandle;
            PL[(*INDEXES) - 1].THREADS  = proc->threads;

            DosGetPrty( PRTYS_PROCESS, &(PL[(*INDEXES) - 1].PRIORITY), proc->processid );

            for( PL[(*INDEXES) - 1].CPU = 0, t = 0, pThread = PTR(proc->threadlist, 0);
                 t < proc->threads;
                 t++, pThread++ )
                    PL[(*INDEXES) - 1].CPU += (pThread->systime + pThread->usertime);
            }

          PS = SysProcTree2( PL, INDEXES, procstat, proc->processid );
          PL = PS;
          }

        proc->type = 1;    /* kludge, reset mark */
        }

    return( PS );
}

VOID    SysProcTree( ProcsList_t **pProcsList, ULONG *INDEXES )
{
    struct  procstat *procstat = NULL;

    procstat = malloc(0x8000);
    DosQProcStatus( procstat, 0x8000 );

    (*INDEXES) = 0 ;
    *pProcsList = SysProcTree2( NULL, INDEXES, procstat, 0 );

    free( procstat );
}


ThreadsList_t *SysThreadsList2( ULONG *INDEXES, struct process *proc )
{
    ThreadsList_t   *TL = NULL;
    struct thread   *thread = NULL;
    UCHAR  systime[32], usertime[32], *status;
    USHORT count;

    for ( count = 0, thread = PTR(proc->threadlist, 0); count < proc->threads; count++, thread++ )
      {
      (*INDEXES)++ ;
      TL = (ThreadsList_t*) realloc( TL, sizeof(ThreadsList_t) * (*INDEXES) );

          if( TL != NULL )
            {
            TL[(*INDEXES) - 1].TID      = thread->threadid;
            TL[(*INDEXES) - 1].TSID     = thread->threadslotid;
            TL[(*INDEXES) - 1].BLKID    = thread->blockid;
            TL[(*INDEXES) - 1].PRIORITY = thread->priority;
            TL[(*INDEXES) - 1].SYSCPU   = thread->systime;
            TL[(*INDEXES) - 1].USRCPU   = thread->usertime;
            TL[(*INDEXES) - 1].STATUS   = thread->status;
            }
      }

    return( TL );
}


VOID    SysThreadsList( ThreadsList_t **pThreadsList, ULONG *INDEXES, USHORT pid )
{
    struct  procstat *procstat = NULL;
    struct  process  *proc = NULL;

    procstat = malloc(0x8000);
    DosQProcStatus( procstat, 0x8000 );

    for( proc = PTR(procstat->processes, 0);
         proc -> type != 3;
         proc = PTR(proc->threadlist, proc->threads * sizeof(struct thread))
       )
      if( proc->processid == pid )
        {
        (*INDEXES) = 0 ;
        *pThreadsList = SysThreadsList2( INDEXES, proc );
        }
    free( procstat );
}

/*
void modlist(int imports)
{
    struct  procstat *procstat = NULL;
  struct module *mod;
  UCHAR name[256];
  ULONG count;

  for ( mod = PTR(procstat -> modules, 0); ;
        mod = PTR(mod -> nextmodule, 0)
      )
  {
    if ( !imports || mod -> submodules )
      if ( imports )
      {

//        printf("%04X  %s\n", mod -> modhandle, PTR(mod -> namepointer, 0));

        for ( count = 0; count < mod -> submodules; count++ )
        {
          DosQueryModuleName(mod -> submodule[count], sizeof(name), name);

//          printf("  %04X  %s\n", mod -> submodule[count], name);

        }
      }
      else

//        printf(" %04X   %2d  %7ld  %6ld  %s\n", mod -> modhandle,
//               mod -> modtype ? 32 : 16, mod -> segments,
//               mod -> submodules, PTR(mod -> namepointer, 0));


    if ( mod -> nextmodule == 0L )
      break;
  }
}


void semlist(void)
{
    struct  procstat *procstat = NULL;
  struct semaphore *sem;

  for ( sem = PTR(procstat -> semaphores, 16);
        sem -> nextsem != 0L;
        sem = PTR(sem -> nextsem, 0)
      ) ;

//    printf("%3d  %3d   %02X  %4d   %04X  \\SEM%s\n",
//           sem -> refs, sem -> requests, sem -> flag,
//           sem -> owner, sem -> index, sem -> name);

}


void shmlist(void)
{
    struct  procstat *procstat = NULL;
  struct shmem *shmem;
  UCHAR name[256];
  ULONG size;
#if defined(__32BIT__)
  ULONG attrib;
  PVOID base;
#else
  SEL sel;
#endif

  for ( shmem = PTR(procstat -> sharedmemory, 0);
        shmem -> nextseg != 0L;
        shmem = PTR(shmem -> nextseg, 0)
      )
  {
    strcpy(name, "\\SHAREMEM\\");
    strcat(name, shmem -> name);

#if defined(__32BIT__)
    DosGetNamedSharedMem(&base, name, PAG_READ);
    size = -1;
    attrib = PAG_SHARED | PAG_READ;
    if ( DosQueryMem(base, &size, &attrib) )
      size = 0;
    DosFreeMem(base);
#else
    DosGetShrSeg(name, &sel);
    if ( DosSizeSeg(sel, &size) )
      size = 0;
    DosFreeSeg(sel);
#endif

//    printf(" %04X   %04X   %08lX  %4d  %s\n", shmem -> handle,
//           shmem -> selector, size, shmem -> refs, name);

  }
}



void main(void)
{
  procstat = malloc(0x8000);
  DosQProcStatus(procstat, 0x8000);

  printf("\n¯ Process list\n");
  printf("\n PID    PPID  SESS THRD PRIO    TIME    PROGRAM");
  printf("\n------ ------ ---- ---- ---- ---------- ------->\n");

  proctree(0, 0);

  printf("\n¯ Thread list\n");
  printf("\n TID    TSID  PRIO  STATE  BLOCKID   SYSTIME    USERTIME");
  printf("\n------ ------ ---- ------- -------- ---------- ----------\n");

  threadlist();

  printf("\n¯ Module list\n");
  printf("\nHANDLE TYPE SEGMENTS IMPORTS PATH");
  printf("\n------ ---- -------- ------- ---->\n");

  modlist(0);

  printf("\n¯ Module tree\n");
  printf("\n--------------\n");

  modlist(1);

  printf("\n¯ Semaphore list\n");
  printf("\nREF  REQ  FLAG OWNER INDEX  NAME");
  printf("\n---- ---- ---- ----- ------ ---->\n");

  semlist();

  printf("\n¯ Shared memory list\n");
  printf("\nHANDLE  SEL    SEGSIZE    REF  NAME");
  printf("\n------ ------ ---------- ----- ---->\n");

  shmlist();

  exit(0);
}
*/

