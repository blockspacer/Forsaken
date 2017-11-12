# Microsoft Developer Studio Project File - Name="motionmapper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=motionmapper - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "motionmapper.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "motionmapper.mak" CFG="motionmapper - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "motionmapper - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "motionmapper - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/utils/motionmapper", ZKGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "motionmapper - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Zi /O2 /I "..\common" /I "..\..\Public" /I "..\..\public\tier1" /I "..\nvtristriplib" /I "..\..\Game_Shared" /D "_CONSOLE" /D "PROTECTED_THINGS_DISABLE" /D "NDEBUG" /D "_WIN32" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:console /map /debug /machine:I386 /nodefaultlib:"libcd.lib" /libpath:"..\..\lib\common" /libpath:"..\..\lib\public"
# Begin Custom Build - copy to ..\..\..\game\bin\motionmapper.exe
TargetDir=.\Release
TargetPath=.\Release\motionmapper.exe
TargetName=motionmapper
InputPath=.\Release\motionmapper.exe
SOURCE="$(InputPath)"

"..\..\..\game\bin\motionmapper.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\game\bin\motionmapper.exe attrib -r ..\..\..\game\bin\motionmapper.exe 
	copy $(TargetPath) ..\..\..\game\bin\motionmapper.exe 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\game\bin\$(TargetName).map 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "motionmapper - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /ZI /Od /I "..\common" /I "..\..\Public" /I "..\..\public\tier1" /I "..\nvtristriplib" /I "..\..\Game_Shared" /D "_CONSOLE" /D "PROTECTED_THINGS_DISABLE" /D "_DEBUG" /D "_WIN32" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libc.lib" /libpath:"..\..\lib\common" /libpath:"..\..\lib\public"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - copy to ..\..\..\game\bin\motionmapper.exe
TargetDir=.\Debug
TargetPath=.\Debug\motionmapper.exe
TargetName=motionmapper
InputPath=.\Debug\motionmapper.exe
SOURCE="$(InputPath)"

"..\..\..\game\bin\motionmapper.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\game\bin\motionmapper.exe attrib -r ..\..\..\game\bin\motionmapper.exe 
	copy $(TargetPath) ..\..\..\game\bin\motionmapper.exe 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\game\bin\$(TargetName).map 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "motionmapper - Win32 Release"
# Name "motionmapper - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\tier1\characterset.cpp
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_init.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_init.h
# End Source File
# Begin Source File

SOURCE=..\common\filesystem_tools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\interface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\KeyValues.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\mathlib.cpp
# End Source File
# Begin Source File

SOURCE=.\motionmapper.cpp
# End Source File
# Begin Source File

SOURCE=..\common\scriplib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlsymbol.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\Public\amd3dx.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\basetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\characterset.h
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.h
# End Source File
# Begin Source File

SOURCE=..\..\public\Color.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\commonmacros.h
# End Source File
# Begin Source File

SOURCE=..\..\public\compressed_vector.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\dbg.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\fasttimer.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\FileSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.h
# End Source File
# Begin Source File

SOURCE=..\common\filesystem_tools.h
# End Source File
# Begin Source File

SOURCE=..\..\public\appframework\IAppSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\ICommandLine.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\IKeyValuesSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\interface.h
# End Source File
# Begin Source File

SOURCE=..\..\public\KeyValues.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\MATHLIB.H
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\mem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memalloc.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memdbgoff.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memdbgon.h
# End Source File
# Begin Source File

SOURCE=.\motionmapper.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\public\protected_things.h
# End Source File
# Begin Source File

SOURCE=..\common\scriplib.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\string_t.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\strtools.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\studio.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\UtlBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\utldict.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\utllinkedlist.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\UtlMemory.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\utlrbtree.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\utlsymbol.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\UtlVector.h
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
# Begin Source File

SOURCE=..\..\lib\public\vstdlib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\nvtristrip.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# End Target
# End Project
