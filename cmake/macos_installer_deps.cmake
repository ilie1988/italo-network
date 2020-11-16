# macos specific cpack stuff goes here

# Here we build italonet-network-control-panel into 'italonet-gui.app' in "extra/" where a postinstall
# script will then move it to /Applications/.

set(ITALONET_GUI_REPO "https://github.com/italo-project/italo-network-control-panel.git"
    CACHE STRING "Can be set to override the default italonet-gui git repository")
set(ITALONET_GUI_CHECKOUT "origin/master"
    CACHE STRING "Can be set to specify a particular branch or tag to build from ITALONET_GUI_REPO")
set(MACOS_SIGN_APP ""  # FIXME: it doesn't use a Apple Distribution key because WTF knows.
    CACHE STRING "enable codesigning of the stuff inside the .app and the italonet binary -- use a 'Apple Distribution' key (or description) from `security find-identity -v`")
set(MACOS_SIGN_PKG ""
    CACHE STRING "enable codesigning of the .pkg -- use a 'Developer ID Installer' key (or description) from `security find-identity -v`")
set(MACOS_NOTARIZE_USER ""
    CACHE STRING "set macos notarization username; can also set it in ~/.notarization.cmake")
set(MACOS_NOTARIZE_PASS ""
    CACHE STRING "set macos notarization password; can also set it in ~/.notarization.cmake")
set(MACOS_NOTARIZE_ASC ""
    CACHE STRING "set macos notarization asc provider; can also set it in ~/.notarization.cmake")

include(ExternalProject)

message(STATUS "Building ItalonetGUI.app from ${ITALONET_GUI_REPO} @ ${ITALONET_GUI_CHECKOUT}")

ExternalProject_Add(italonet-gui
    GIT_REPOSITORY "${ITALONET_GUI_REPO}"
    GIT_TAG "${ITALONET_GUI_CHECKOUT}"
    CMAKE_ARGS -DMACOS_APP=ON -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR} -DMACOS_SIGN=${MACOS_SIGN_APP}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DBUILD_STATIC_DEPS=ON -DBUILD_SHARED_LIBS=OFF
    )



install(PROGRAMS ${CMAKE_SOURCE_DIR}/contrib/macos/italonet_uninstall.sh
        DESTINATION "bin/"
        COMPONENT italonet)

install(DIRECTORY ${PROJECT_BINARY_DIR}/ItalonetGUI.app
        DESTINATION "../../Applications"
        USE_SOURCE_PERMISSIONS
        COMPONENT gui
        PATTERN "*"
        )

# copy files that will be later moved by the postinstall script to proper locations
install(FILES ${CMAKE_SOURCE_DIR}/contrib/macos/italonet_macos_daemon_script.sh
              ${CMAKE_SOURCE_DIR}/contrib/macos/network.italo.italonet.daemon.plist
        DESTINATION "extra/"
        COMPONENT italonet)

set(CPACK_COMPONENTS_ALL italonet gui)

set(CPACK_COMPONENT_ITALONET_DISPLAY_NAME "Italonet Service")
set(CPACK_COMPONENT_ITALONET_DESCRIPTION "Main Italonet runtime service, managed by Launchd")

set(CPACK_COMPONENT_GUI_DISPLAY_NAME "Italonet GUI")
set(CPACK_COMPONENT_GUI_DESCRIPTION "Small GUI which provides stats and limited runtime control of the Italonet service. Resides in the system tray.")

set(CPACK_GENERATOR "productbuild")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/italonet")
set(CPACK_POSTFLIGHT_ITALONET_SCRIPT ${CMAKE_SOURCE_DIR}/contrib/macos/postinstall)

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")

set(CPACK_PRODUCTBUILD_IDENTITY_NAME "${MACOS_SIGN_PKG}")

if(MACOS_SIGN_APP)
    add_custom_target(sign ALL
        echo "Signing italonet and italonet-vpn binaries"
        COMMAND codesign -s "${MACOS_SIGN_APP}" --strict --options runtime --force -vvv $<TARGET_FILE:italonet> $<TARGET_FILE:italonet-vpn>
        DEPENDS italonet italonet-vpn
        )
endif()

if(MACOS_SIGN_APP AND MACOS_SIGN_PKG)
    if(NOT MACOS_NOTARIZE_USER)
        if(EXISTS "$ENV{HOME}/.notarization.cmake")
            include("$ENV{HOME}/.notarization.cmake")
        endif()
    endif()
    if(MACOS_NOTARIZE_USER AND MACOS_NOTARIZE_PASS AND MACOS_NOTARIZE_ASC)
        message(STATUS "'notarization' target enabled")
        configure_file(${CMAKE_SOURCE_DIR}/contrib/macos/notarize.py.in ${CMAKE_CURRENT_BINARY_DIR}/contrib/notarize.py ESCAPE_QUOTES @ONLY)
        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/contrib/notarize.py DESTINATION ${PROJECT_BINARY_DIR} FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
        add_custom_target(notarize ./notarize.py)
    else()
        message(WARNING "Not enable 'notarization' target: signing is enabled but notarization info not provided. Create ~/.notarization.cmake or set cmake parameters directly")
    endif()
endif()
