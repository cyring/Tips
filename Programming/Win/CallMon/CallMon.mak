# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=CallMon - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to CallMon - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CallMon - Win32 Release" && "$(CFG)" !=\
 "CallMon - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "CallMon.mak" CFG="CallMon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CallMon - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "CallMon - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "CallMon - Win32 Debug"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "CallMon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\CallMon.exe"

CLEAN : 
	-@erase ".\Release\CallMon.exe"
	-@erase ".\Release\CallMon.obj"
	-@erase ".\Release\CallMon.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
RSC_PROJ=/l 0x40c /fo"$(INTDIR)/CallMon.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/CallMon.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /version:3.39 /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /version:3.39 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/CallMon.pdb" /machine:I386\
 /out:"$(OUTDIR)/CallMon.exe" 
LINK32_OBJS= \
	"$(INTDIR)/CallMon.obj" \
	"$(INTDIR)/CallMon.res"

"$(OUTDIR)\CallMon.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CallMon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\CallMon.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\CallMon.exe"
	-@erase ".\Debug\CallMon.obj"
	-@erase ".\Debug\CallMon.res"
	-@erase ".\Debug\CallMon.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
RSC_PROJ=/l 0x40c /fo"$(INTDIR)/CallMon.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/CallMon.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /version:3.39 /subsystem:windows /incremental:no /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /version:3.39 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/CallMon.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/CallMon.exe" 
LINK32_OBJS= \
	"$(INTDIR)/CallMon.obj" \
	"$(INTDIR)/CallMon.res"

"$(OUTDIR)\CallMon.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "CallMon - Win32 Release"
# Name "CallMon - Win32 Debug"

!IF  "$(CFG)" == "CallMon - Win32 Release"

!ELSEIF  "$(CFG)" == "CallMon - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\CallMon.c

!IF  "$(CFG)" == "CallMon - Win32 Release"


"$(INTDIR)\CallMon.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "CallMon - Win32 Debug"

NODEP_CPP_CALLM=\
	".\data"\
	

"$(INTDIR)\CallMon.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CallMon.rc
DEP_RSC_CALLMO=\
	".\offline.bmp"\
	".\online.bmp"\
	".\dialing.bmp"\
	".\busy.bmp"\
	".\BYTEL1.BMP"\
	".\CALLMOFF.ICO"\
	".\callmon.ico"\
	

"$(INTDIR)\CallMon.res" : $(SOURCE) $(DEP_RSC_CALLMO) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
