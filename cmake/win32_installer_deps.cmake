set(GUI_ZIP_URL "https://builds.italonet.dev/italo-project/italo-network-control-panel/master/italonet-gui-windows-32bit-20201106T142720Z-b92e5fd10.zip")
set(GUI_ZIP_HASH SHA256=52868f7bf6d1f4fc7ca587cc79449fefd8000a485bb7917acbc29fdefdd55839)
set(TUNTAP_URL "https://build.openvpn.net/downloads/releases/latest/tap-windows-latest-stable.exe")
set(TUNTAP_EXE "${CMAKE_BINARY_DIR}/tuntap-install.exe")
set(BOOTSTRAP_URL "https://seed.italonet.org/italonet.signed")
set(BOOTSTRAP_FILE "${CMAKE_BINARY_DIR}/bootstrap.signed")

file(DOWNLOAD
    ${TUNTAP_URL}
    ${TUNTAP_EXE})

file(DOWNLOAD
    ${BOOTSTRAP_URL}
    ${BOOTSTRAP_FILE})

file(DOWNLOAD
    ${GUI_ZIP_URL}
    ${CMAKE_BINARY_DIR}/italonet-gui.zip
    EXPECTED_HASH ${GUI_ZIP_HASH})

execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_BINARY_DIR}/italonet-gui.zip
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

install(DIRECTORY ${CMAKE_BINARY_DIR}/gui DESTINATION share COMPONENT gui)
install(PROGRAMS ${TUNTAP_EXE} DESTINATION bin)
install(FILES ${BOOTSTRAP_FILE} DESTINATION share)

set(CPACK_PACKAGE_INSTALL_DIRECTORY "Italonet")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/win32-setup/italonet.ico")
set(CPACK_NSIS_DEFINES "RequestExecutionLevel admin")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '$INSTDIR\\\\bin\\\\tuntap-install.exe /S'\\nExecWait '$INSTDIR\\\\bin\\\\italonet.exe --install'\\nExecWait '$INSTDIR\\\\bin\\\\italonet.exe -g C:\\\\ProgramData\\\\italonet\\\\italonet.ini'\\nCopyFiles '$INSTDIR\\\\share\\\\bootstrap.signed' C:\\\\ProgramData\\\\italonet\\\\bootstrap.signed")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "ExecWait '$INSTDIR\\\\bin\\\\italonet.exe --remove'\\nRMDir /r /REBOOTOK C:\\\\ProgramData\\\\italonet")
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Italonet.lnk' '$INSTDIR\\\\share\\\\gui\\\\italonet-gui.exe'"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$START_MENU\\\\Italonet.lnk'"
)
