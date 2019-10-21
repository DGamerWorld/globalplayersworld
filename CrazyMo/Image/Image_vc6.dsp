# Microsoft Developer Studio Project File - Name="Image_vc6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Image_vc6 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Image_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Image_vc6.mak" CFG="Image_vc6 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Image_vc6 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Image_vc6 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Image_vc6 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Image_vc6.lib"

!ELSEIF  "$(CFG)" == "Image_vc6 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Image_vc6.lib"

!ENDIF 

# Begin Target

# Name "Image_vc6 - Win32 Release"
# Name "Image_vc6 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\atitc.cpp
# End Source File
# Begin Source File

SOURCE=.\etc1.cpp
# End Source File
# Begin Source File

SOURCE=.\s3tc.cpp
# End Source File
# Begin Source File

SOURCE=.\TGAlib.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\Include\basetsd.h
# End Source File
# Begin Source File

SOURCE=..\Image.h
# End Source File
# Begin Source File

SOURCE=..\PlatformConfig.h
# End Source File
# End Group
# Begin Group "jpeg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\jpeg\jaricom.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jcomapi.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdapimin.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdapistd.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdarith.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdatasrc.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdcoefct.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdcolor.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdct.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jddctmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdhuff.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdinput.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmainct.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmarker.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmaster.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmerge.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdpostct.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jdsample.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jerror.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jerror.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jfdctfst.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctflt.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctfst.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctint.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jinclude.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jmemansi.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jmemmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jmemsys.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jpegint.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\jpeg\jquant1.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jquant2.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jutils.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg\jversion.h
# End Source File
# End Group
# Begin Group "png"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\png\adler32.cpp
# End Source File
# Begin Source File

SOURCE=.\png\crc32.cpp
# End Source File
# Begin Source File

SOURCE=.\png\crc32.h
# End Source File
# Begin Source File

SOURCE=.\png\inffast.cpp
# End Source File
# Begin Source File

SOURCE=.\png\inffast.h
# End Source File
# Begin Source File

SOURCE=.\png\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\png\inflate.cpp
# End Source File
# Begin Source File

SOURCE=.\png\inflate.h
# End Source File
# Begin Source File

SOURCE=.\png\inftrees.cpp
# End Source File
# Begin Source File

SOURCE=.\png\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\png\png.cpp
# End Source File
# Begin Source File

SOURCE=.\png\png.h
# End Source File
# Begin Source File

SOURCE=.\png\pngconf.h
# End Source File
# Begin Source File

SOURCE=.\png\pngdebug.h
# End Source File
# Begin Source File

SOURCE=.\png\pngerror.cpp
# End Source File
# Begin Source File

SOURCE=.\png\pngget.cpp
# End Source File
# Begin Source File

SOURCE=.\png\pnginfo.h
# End Source File
# Begin Source File

SOURCE=.\png\pnglibconf.h
# End Source File
# Begin Source File

SOURCE=.\png\pngmem.cpp
# End Source File
# Begin Source File

SOURCE=.\png\pngpriv.h
# End Source File
# Begin Source File

SOURCE=.\png\pngread.cpp
# End Source File
# Begin Source File

SOURCE=.\png\pngrio.cpp
# End Source File
# Begin Source File

SOURCE=.\png\pngrtran.cpp
# End Source File
# Begin Source File

SOURCE=.\png\pngrutil.cpp
# End Source File
# Begin Source File

SOURCE=.\png\pngset.cpp
# End Source File
# Begin Source File

SOURCE=.\png\pngstruct.h
# End Source File
# Begin Source File

SOURCE=.\png\pngtrans.cpp
# End Source File
# Begin Source File

SOURCE=.\png\zconf.h
# End Source File
# Begin Source File

SOURCE=.\png\zlib.h
# End Source File
# Begin Source File

SOURCE=.\png\zutil.cpp
# End Source File
# Begin Source File

SOURCE=.\png\zutil.h
# End Source File
# End Group
# End Target
# End Project
