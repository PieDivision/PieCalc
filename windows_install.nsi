; 
; install_script.nsi
;

;--------------------------------
	!include "MUI.nsh"
	SetDatablockOptimize off

	!define NAME "PieCalc"
	!define VERSION "1.0"
	!define APPNAME "${NAME}-${VERSION}"
	!define COMPANYNAME "PieDivision"

	; The name of the installer
	Name "${APPNAME}"

	; The file to write
	OutFile "bin/${APPNAME}-setup.exe"

	; The icon
	Icon "data\icon_128x128.ico"

	; The default installation directory
	InstallDir $PROGRAMFILES\${APPNAME}

	; The text to prompt the user to enter a directory
	DirText "You are about to install this awesome Pie Calculator on your computer! Choose a directory:"

;--------------------------------
; Pages
;--------------------------------

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE "LICENSE"
	!insertmacro MUI_PAGE_DIRECTORY

  	;!insertmacro MUI_PAGE_STARTMENU Application $APPNAME

	!insertmacro MUI_PAGE_INSTFILES
 
   		# These indented statements modify settings for MUI_PAGE_FINISH
    	!define MUI_FINISHPAGE_NOAUTOCLOSE
    	!define MUI_FINISHPAGE_RUN
    	!define MUI_FINISHPAGE_RUN_NOTCHECKED
    	!define MUI_FINISHPAGE_RUN_TEXT "Launch PieCalc!"
    	!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchLink"
    	!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
    	!define MUI_FINISHPAGE_SHOWREADME $INSTDIR\README.txt

  	!insertmacro MUI_PAGE_FINISH

;--------------------------------
;Languages
;--------------------------------

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
Section "Install"
;--------------------------------

	; Set output path to the installation directory.
	SetOutPath $INSTDIR

	; Put files there
	File /r "bin\*.*"
	File /r "data\*.*"
	File /r "gtk\bin\*.dll"

	File LICENSE
	File README.txt

	; Tell the compiler to write an uninstaller and to look for a "Uninstall" section 
	WriteUninstaller $INSTDIR\Uninstall.exe

	; Create shortcuts
	;!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    	CreateDirectory "$SMPROGRAMS\${COMPANYNAME}"
    	createShortCut "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk" "$INSTDIR\piecalc.exe" "" "$INSTDIR\icon_128x128.ico"
    	CreateShortcut "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}-uninstall.lnk" "$INSTDIR\Uninstall.exe"
  	;!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
Section "Uninstall"
;--------------------------------

	; Delete files from installation
	Delete $INSTDIR\*
	Delete $INSTDIR\Uninstall.exe
	RMDir $INSTDIR

	; Remove shortcuts too
	Delete "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}"
	RMDIR "$SMPROGRAMS\${COMPANYNAME}"


SectionEnd ; end the uninstall section

;
Function LaunchLink
  ExecShell "" "$PROGRAMFILES\${APPNAME}\piecalc.exe"
FunctionEnd
