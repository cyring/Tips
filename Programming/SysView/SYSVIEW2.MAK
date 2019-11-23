# Created by IBM WorkFrame/2 MakeMake at 12:45:53 on 05/04/97
#
# This makefile should be run in the following directory:
#   D:\OS2DEV\SYSVIEW
#
# The actions included in this makefile are:
#   BIND::Resource Bind
#   COMPILE::CLC C++
#   COMPILE::Resource Compile
#   LINK::CLC Link

.all: \
  .\SYSVIEW2.EXE

.SUFFIXES:

.SUFFIXES: .C .RC

.RC.res:
      @echo WD::COMPILE::Resource Compile
      rc.exe -r %s %|fF.RES

.C.obj:
      @echo WD::COMPILE::CLC C++
      icc.exe /Tdc /Ss /Q /Wclscmpcndcnscnvcpydcleffenuextgengnrinilanobsordparporppcpptprorettrdtruunduniusevft /Ti /W2 /Gm /Gd /G4 /C %s

.\SYSVIEW2.EXE: \
    .\SYSPROC2.obj \
    .\SYSVIEW2.obj \
    .\SYSVIEW2.res \
    {$(LIB)}SYSVIEW2.def \
    SYSVIEW2.MAK
      @echo WD::LINK::CLC Link
      icc.exe @<<
 /B" /de /st:0x7FFF /pmtype:pm /nologo"
 /FeSYSVIEW2.EXE
 SYSVIEW2.def
 .\SYSPROC2.obj
 .\SYSVIEW2.obj
<<
      @echo WD::BIND::Resource Bind
      rc.exe .\SYSVIEW2.res SYSVIEW2.EXE

.\SYSVIEW2.res: \
    D:\OS2DEV\SYSVIEW\SYSVIEW2.RC \
    {$(INCLUDE)}threads.ico \
    {$(INCLUDE)}process.ico \
    {$(INCLUDE)}ram.ico \
    {$(INCLUDE)}windows.ico \
    {$(INCLUDE)}os2sys.ico \
    {$(INCLUDE)}cyril.bmp \
    {$(INCLUDE)}SysView2.h \
    SYSVIEW2.MAK

.\SYSPROC2.obj: \
    D:\OS2DEV\SYSVIEW\SYSPROC2.C \
    {D:\OS2DEV\SYSVIEW;$(INCLUDE);}SysProc2.h \
    SYSVIEW2.MAK

.\SYSVIEW2.obj: \
    D:\OS2DEV\SYSVIEW\SYSVIEW2.C \
    {D:\OS2DEV\SYSVIEW;$(INCLUDE);}SysView2.h \
    {D:\OS2DEV\SYSVIEW;$(INCLUDE);}SysProc2.h \
    SYSVIEW2.MAK

