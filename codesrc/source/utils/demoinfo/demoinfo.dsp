# Microsoft Developer Studio Project File - Name="demoinfo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=demoinfo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "demoinfo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "demoinfo.mak" CFG="demoinfo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "demoinfo - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "demoinfo - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/utils/demoinfo", XPQDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "demoinfo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /O2 /I "..\common" /I "..\..\public" /I "..\..\public\tier1" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\lib\public"
# Begin Custom Build
TargetPath=.\Release\demoinfo.exe
InputPath=.\Release\demoinfo.exe
SOURCE="$(InputPath)"

"..\..\..\game\bin\demoinfo.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist  ..\..\..\game\bin\demoinfo.exe attrib -r ..\..\..\game\bin\demoinfo.exe 
	if exist $(TargetPath) copy $(TargetPath) ..\..\..\game\bin\demoinfo.exe 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "demoinfo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /Zi /Od /I "..\..\public" /I "..\..\public\tier1" /I "..\common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /map /debug /machine:I386 /libpath:"..\..\lib\public" /fixed:no
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build
TargetPath=.\Debug\demoinfo.exe
InputPath=.\Debug\demoinfo.exe
SOURCE="$(InputPath)"

"..\..\..\game\bin\demoinfo.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist  ..\..\..\game\bin\demoinfo.exe attrib -r ..\..\..\game\bin\demoinfo.exe 
	if exist $(TargetPath) copy $(TargetPath) ..\..\..\game\bin\demoinfo.exe 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "demoinfo - Win32 Release"
# Name "demoinfo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\tier1\characterset.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.h
# End Source File
# Begin Source File

SOURCE=.\demoinfo.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_init.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\filesystem_tools.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\tier1\interface.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\tier1\KeyValues.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tooldemofile.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlbuffer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlsymbol.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Public\basetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\commonmacros.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\dbg.h
# End Source File
# Begin Source File

SOURCE=..\..\public\demofile\demoformat.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\fasttimer.h
# End Source File
# Begin Source File

SOURCE=..\common\filesystem_tools.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\MATHLIB.H
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memdbgon.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\public\protected_things.h
# End Source File
# Begin Source File

SOURCE=..\..\public\string_t.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\strtools.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\studio.h
# End Source File
# Begin Source File

SOURCE=.\tooldemofile.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utldict.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlrbtree.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlsymbol.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlvector.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vector.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vector2d.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vector4d.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\vstdlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "DemoSmootherSampleSourceFiles"

# PROP Default_Filter "*.h;*.cpp"
# Begin Source File

SOURCE=.\demosmoothersamplesource.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\demosmoothersamplesource.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\demosmoothersamplesource.txt
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\vstdlib.lib
# End Source File
# End Target
# End Project
