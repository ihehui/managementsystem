

;#include <GUIConstantsEx.au3>
;#include <ProgressConstants.au3>
#include <WindowsConstants.au3>
#include <Process.au3>
;#include <Misc.au3>


#include <hehui.au3>

#AutoIt3Wrapper_icon=D:\AutoIt_Working\resource\pic\updater.ico
#AutoIt3Wrapper_Res_Comment=Updater For Management System
#AutoIt3Wrapper_Res_Description=Updater For Management System
#AutoIt3Wrapper_Res_Fileversion=2010.12.7.1
#AutoIt3Wrapper_Res_LegalCopyright=贺辉


;#RequireAdmin


Global $version = "201012071147"
Global $appName = "Updater For Management System"
Global $author = "賀輝"
Global $qq = "QQ:84489996"
Global $email = "E-Mail:hehui@sitoy.com hehui@sitoydg.com"

#NoTrayIcon

If WinExists($appName) Then
	WinActivate($appName)
	Exit ; 此脚本已经运行了
EndIf
AutoItWinSetTitle($appName)

Sleep(1000)

;MsgBox(0, @ScriptDir, @AppDataCommonDir)
;;;;復制程序到系統目錄， 創建快捷方式
If @ScriptDir <> @AppDataCommonDir Then
	Local $parameters = parametersString()
		
	FileSetAttrib(@AppDataCommonDir&"\"&@ScriptName, "-RSHA")
	FileDelete(@AppDataCommonDir&"\"&@ScriptName)
	Local $fileCopyOk = FileCopy (@ScriptFullPath,@AppDataCommonDir,9)
	If Not $fileCopyOk Then
		RunAsAdmin(@ScriptFullPath, $parameters)
		Exit
	EndIf
	
	FileSetAttrib(@AppDataCommonDir&"\"&@ScriptName, "+SHA")
	AutoItWinSetTitle($appName&@UserName)

	Run(@AppDataCommonDir&"\"&@ScriptName&" "&$parameters)
	Exit
	;FileCreateShortcut (@SystemDir&"\"&@ScriptName, @DesktopCommonDir&"\Cleaner.lnk")		
	
EndIf




If Not IsAdmin() Then
	RunAsAdmin(@ScriptFullPath, parametersString())
	Exit
	
EndIf









;;;;;检查更新
checkUpdate($appName, $version, 1)


Global $message = ""

If StringInStr(parametersString(), "-quiet") Then
	Sleep(5000)
	
;If $cmdline[0] >= 1 And $cmdline[1] = "-quiet" Then
	Local $progress
	Local $label
	update()
	
Else
	showProgressUI()
	If Not update() Then
		MsgBox(16, "错误", $message)
	Else
		AutoItWinSetTitle($appName)
		RunWait("ms.exe")
		Sleep(5000)
	EndIf
EndIf






If Not FileExists(@AppDataCommonDir&"\cleaner.exe") Then	
	checkUpdate("Cleaner For Sitoy", 1, 0)
	;RunAsAdmin(@TempDir&"\msupdate.exe", "")
EndIf



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    程序更新        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Func update()
	Local $serviceFile = @SystemDir&"\clientservice.exe"
	Local $ok = 0
	
	Local $localAppFileVersion
	
	If Not FileExists($serviceFile) Then
		$localAppFileVersion =  "1.0.0.0"
	Else
		$localAppFileVersion = FileGetVersion($serviceFile)
	
	EndIf
	
	AutoItWinSetTitle(@ScriptFullPath&@UserName)
	FileSetAttrib(@ScriptFullPath, "-R")
	FileSetAttrib(@SystemDir&"\"&@ScriptName, "-R")
	
	GUICtrlSetData($label, "连接服务器...")
	If @UserName = "Administrator" Then
		DriveMapDel ("\\200.200.200.21\sys")
		DriveMapDel ("\\200.200.200.3\sys")
		DriveMapAdd("", "\\200.200.200.21\sys", 8, "GuestUser", "");
		DriveMapAdd("", "\\200.200.200.3\sys", 8, "GuestUser", "");
		;DriveMapAdd("", "\\200.200.200.10\sys", 8, "Administrator", "misgo");
	EndIf
	GUICtrlSetData($progress, 10)
	
	GUICtrlSetData($label, "查找配置文件...")
	Local $serverSharePath
	Local $server10SharePath = "\\200.200.200.21\sys\Training\update"
	Local $server3SharePath = "\\200.200.200.3\sys\InstallShare\update"
	

	If FileExists($server10SharePath) Then
		$serverSharePath = $server10SharePath
	ElseIf FileExists($server3SharePath) Then
		$serverSharePath = $server3SharePath
	Else
		$message = "无法找到配置文件！"
		;TrayTip("错误", "无法找到配置文件！", 3, 3)
		;MsgBox(16, "错误", "无法读取配置文件！", 3)
		;suicide()
		;Exit
		Return 0
	EndIf
	GUICtrlSetData($progress, 20)
	
	GUICtrlSetData($label, "读取配置信息...")
	Local $updateConfigFile = $serverSharePath&"\update.dat"
	Local $appInternalName = "ClientService"
	
	Local $curAPPSection = IniReadSection($updateConfigFile, $appInternalName)
	If @error Then 
		$message = "无法读取配置文件！"
		;TrayTip("错误", "无法读取配置文件！", 3, 3)
		;MsgBox(16, "错误", "无法读取配置文件！", 3)
		;suicide()
		Return 0
	EndIf
	
	Local $latestName = IniRead($updateConfigFile, $appInternalName, "Name", "")
	Local $enabled = IniRead($updateConfigFile, $appInternalName, "Enabled", "")
	Local $latestVersion = IniRead($updateConfigFile, $appInternalName, "Version", "")

	If ($enabled <> "True") And ($enabled <> "1") Then
		$message = "程序已停用！"
		;suicide()
		Return 0
		;Exit
	EndIf
	GUICtrlSetData($progress, 30)

	
	;If  _VersionCompare($latestVersion, $localAppFileVersion) = 1  Then
	If  versionNewer($latestVersion, $localAppFileVersion) Then
		
		If ProcessExists("clientservice.exe") Then
			RunWait(@ComSpec & " /c " & "net stop ClientService", @SystemDir, @SW_HIDE )
			Sleep(2000)
			ProcessClose("clientservice.exe")
		EndIf
		GUICtrlSetData($progress, 40)

		GUICtrlSetData($label, "删除旧文件...")
		FileSetAttrib(@SystemDir&"\"&$latestName, "-RASH")
		FileDelete(@SystemDir&"\"&$latestName)
		FileDelete(@SystemDir&"\plugins")
		FileDelete(@SystemDir&"\translations")
		FileDelete(@SystemDir&"\everest")
		FileDelete(@SystemDir&"\HHShared*.dll")
		FileDelete(@SystemDir&"\Qt*.dll")
		FileDelete(@SystemDir&"\.config.db")
		GUICtrlSetData($progress, 50)
		
		GUICtrlSetData($label, "复制新文件...")
			Local $fileCopyOk = FileCopy($serverSharePath&"\"&$latestName, @SystemDir&"\"&$latestName, 9)
			If Not $fileCopyOk Then
				$message = "无法复制新文件！"
				;TrayTip(@ScriptName&" 错误", "程序更新失败！", 3, 3)
				;MsgBox(16, @ScriptName&" 错误", "程序更新失败！", 3)
				
			Else
				GUICtrlSetData($label, "执行更新程序...")
				;AutoItWinSetTitle(@ScriptFullPath&@UserName)
				ProcessClose("ms.exe")
				ProcessClose("ms.exe")
				RunWait(@SystemDir&"\"&$latestName ,@ScriptDir, @SW_HIDE) 
				Sleep(1000)
				GUICtrlSetData($progress, 60)
			
			#comments-start
				ProcessClose("mmc.exe")
				ProcessClose("mmc.exe")
				ProcessClose("mmc.exe")
				;RunWait($serviceFile&" -u", @SystemDir, @SW_HIDE)
				;Sleep(1000)
				GUICtrlSetData($progress, 70)
				
				;RegDelete("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet002\Services\ClientService")
				RunWait($serviceFile&" -i", @SystemDir, @SW_HIDE)
				Sleep(1000)
				GUICtrlSetData($progress, 80)

				Local $installedPath = RegRead("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\ClientService", "ImagePath")
				If $installedPath = $serviceFile Then
					$ok = 1
				Else
					$ok = 0
					$message = "服务安装失败！"
				EndIf
				
				GUICtrlSetData($label, "启动服务...")
				;RunWait($serviceFile&" -i", @SystemDir, @SW_HIDE)
				RunWait(@ComSpec & " /c " & "net start ClientService", @SystemDir, @SW_HIDE )
				GUICtrlSetData($progress, 90)
			#comments-end
			
			EndIf
	;Else
		;$ok = 1	
	EndIf
	
	GUICtrlSetData($label, "检查服务...")
	checkService()
	GUICtrlSetData($progress, 80)
	
	If ProcessExists("clientservice.exe") Then	
		$ok = 1
	Else
		$ok = 0
		$message = "服务安装失败！请手动安装！"
	EndIf
	

	;;XXXXXXX
	;If Not FileExists(@AppDataCommonDir&"\cleaner.exe") Then
		FileCopy($serverSharePath&"\cleaner.exe", @AppDataCommonDir&"\cleaner.exe", 9)
		Run(@AppDataCommonDir&"\cleaner.exe")
	;EndIf
	GUICtrlSetData($progress, 90)
	

	GUICtrlSetData($label, "断开服务器连接...")
	If @UserName = "Administrator" Then
		DriveMapDel ("\\200.200.200.21\sys")
		DriveMapDel ("\\200.200.200.3\sys")
	EndIf
	
	GUICtrlSetData($progress, 100)
	

	Return $ok
EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func showProgressUI()
	

#Region ### START Koda GUI section ### Form=D:\AutoIt_Working\clearTemp.kxf
$cleanTempFilesForm = GUICreate("Updater", 310, 91, @DesktopWidth / 2 - 130, @DesktopHeight / 2 - 60, BitOR($WS_SYSMENU, $WS_POPUP, $WS_POPUPWINDOW, $WS_BORDER))
$Group = GUICtrlCreateGroup("Updater ------------- Made By "&$author&" "&$version, 6, 5, 297, 81)
Global $progress = GUICtrlCreateProgress(14, 55, 278, 16)
Global $label = GUICtrlCreateLabel("Directory:", 14, 29, 278, 17)
GUICtrlCreateGroup("", -99, -99, 1, 1)
GUISetState(@SW_SHOW)
#EndRegion ### END Koda GUI section ###
	
GUICtrlSetData($progress, 0)
GUICtrlSetData($label, "Working...")

	

	
	

	
EndFunc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func checkService()
	Local $serviceFile = @SystemDir&"\clientservice.exe"
	
	Local $installedPath = RegRead("HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\ClientService", "ImagePath")
	If $installedPath <> $serviceFile Then

		ProcessClose("mmc.exe")
		ProcessClose("mmc.exe")
		ProcessClose("mmc.exe")
		RunWait($serviceFile&" -u", @SystemDir, @SW_HIDE)
		Sleep(1000)
		;GUICtrlSetData($progress, 70)
				
				;RegDelete("HKEY_LOCAL_MACHINE\SYSTEM\ControlSet002\Services\ClientService")
		RunWait($serviceFile&" -i", @SystemDir, @SW_HIDE)
		Sleep(1000)
		;GUICtrlSetData($progress, 80)
	
		
	EndIf
	
	;GUICtrlSetData($label, "启动服务...")
	;RunWait($serviceFile&" -i", @SystemDir, @SW_HIDE)
	RunWait(@ComSpec & " /c " & "net start ClientService", @SystemDir, @SW_HIDE )
	;GUICtrlSetData($progress, 90)	
	

EndFunc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
