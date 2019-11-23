# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=CallSim - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to CallSim - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CallSim - Win32 Release" && "$(CFG)" !=\
 "CallSim - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "CallSim.mak" CFG="CallSim - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CallSim - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CallSim - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "CallSim - Win32 Debug"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "CallSim - Win32 Release"

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

ALL : "$(OUTDIR)\CallSim.exe"

CLEAN : 
	-@erase ".\Release\CallSim.exe"
	-@erase ".\Release\CallGen.obj"
	-@erase ".\Release\CallSim.obj"
	-@erase ".\Release\CallSim.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /Fr /YX
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
RSC_PROJ=/l 0x40c /fo"$(INTDIR)/CallSim.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/CallSim.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /version:0.11 /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /version:0.11 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/CallSim.pdb" /machine:I386\
 /out:"$(OUTDIR)/CallSim.exe" 
LINK32_OBJS= \
	"$(INTDIR)/CallGen.obj" \
	"$(INTDIR)/CallSim.obj" \
	"$(INTDIR)/CallSim.res"

"$(OUTDIR)\CallSim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CallSim - Win32 Debug"

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

ALL : "$(OUTDIR)\CallSim.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\CallSim.exe"
	-@erase ".\Debug\CallGen.obj"
	-@erase ".\Debug\CallSim.obj"
	-@erase ".\Debug\CallSim.res"
	-@erase ".\Debug\CallSim.ilk"
	-@erase ".\Debug\CallSim.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /Fr /YX
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
RSC_PROJ=/l 0x40c /fo"$(INTDIR)/CallSim.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/CallSim.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /version:0.11 /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /verbose /pdb:none /incremental:no
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /version:0.11 /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/CallSim.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/CallSim.exe" 
LINK32_OBJS= \
	"$(INTDIR)/CallGen.obj" \
	"$(INTDIR)/CallSim.obj" \
	"$(INTDIR)/CallSim.res"

"$(OUTDIR)\CallSim.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "CallSim - Win32 Release"
# Name "CallSim - Win32 Debug"

!IF  "$(CFG)" == "CallSim - Win32 Release"

!ELSEIF  "$(CFG)" == "CallSim - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\CallSim.rc
DEP_RSC_CALLS=\
	".\CallSim.ico"\
	

"$(INTDIR)\CallSim.res" : $(SOURCE) $(DEP_RSC_CALLS) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CallSim.c

!IF  "$(CFG)" == "CallSim - Win32 Release"

DEP_CPP_CALLSI=\
	".\CallGen.h"\
	".\CallWIN.h"\
	

"$(INTDIR)\CallSim.obj" : $(SOURCE) $(DEP_CPP_CALLSI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "CallSim - Win32 Debug"

DEP_CPP_CALLSI=\
	".\CallGen.h"\
	".\CallWIN.h"\
	
NODEP_CPP_CALLSI=\
	".\SB_LINEUP"\
	".\break"\
	".\;"\
	".\hwndScrollBar"\
	".\hdlg"\
	

"$(INTDIR)\CallSim.obj" : $(SOURCE) $(DEP_CPP_CALLSI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CallGen.c
DEP_CPP_CALLG=\
	".\CallWIN.h"\
	{$(INCLUDE)}"\sys\TYPES.H"\
	".\CallGen.h"\
	
NODEP_CPP_CALLG=\
	".\CallMG2.h"\
	

"$(INTDIR)\CallGen.obj" : $(SOURCE) $(DEP_CPP_CALLG) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
