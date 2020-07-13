# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BUILD_SOURCE_DIRS "//home/jason/Desktop/FileTransfer/chrono-dev-feature-FSI-release;//home/jason/Desktop/FileTransfer/chrono_fsi_release")
set(CPACK_BUILD_TYPE "Release")
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake-3.17/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "Chrono built using CMake")
set(CPACK_GENERATOR "TGZ")
set(CPACK_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_INSTALLED_DIRECTORIES "//home/jason/Desktop/FileTransfer/chrono-dev-feature-FSI-release;/")
set(CPACK_INSTALL_CMAKE_PROJECTS "")
set(CPACK_INSTALL_PREFIX "/usr/local")
set(CPACK_MODULE_PATH "//home/jason/Desktop/FileTransfer/chrono-dev-feature-FSI-release/cmake/")
set(CPACK_NSIS_DISPLAY_NAME "ChronoEngine")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
set(CPACK_NSIS_PACKAGE_NAME "ChronoEngine")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OUTPUT_CONFIG_FILE "//home/jason/Desktop/FileTransfer/chrono_fsi_release/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake-3.17/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ChronoEngine is a multibody-dynamics package")
set(CPACK_PACKAGE_FILE_NAME "ChronoEngine-4.0.0-Source")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ChronoEngine")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "ChronoEngine")
set(CPACK_PACKAGE_NAME "ChronoEngine")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "UWSBEL")
set(CPACK_PACKAGE_VERSION "4.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "4")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_RESOURCE_FILE_LICENSE "//home/jason/Desktop/FileTransfer/chrono-dev-feature-FSI-release/LICENSE")
set(CPACK_RESOURCE_FILE_README "//home/jason/Desktop/FileTransfer/chrono-dev-feature-FSI-release/README.md")
set(CPACK_RESOURCE_FILE_WELCOME "/usr/share/cmake-3.17/Templates/CPack.GenericWelcome.txt")
set(CPACK_RPM_PACKAGE_SOURCES "ON")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "//home/jason/Desktop/FileTransfer/chrono-dev-feature-FSI-release;/")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "//home/jason/Desktop/FileTransfer/chrono_fsi_release/CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "ChronoEngine-4.0.0-Source")
set(CPACK_SOURCE_STRIP_FILES "")
set(CPACK_SOURCE_TOPLEVEL_TAG "Linux-x86_64-Source")
set(CPACK_STRIP_FILES "")
set(CPACK_SYSTEM_NAME "Linux-x86_64")
set(CPACK_TOPLEVEL_TAG "Linux-x86_64-Source")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "//home/jason/Desktop/FileTransfer/chrono_fsi_release/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
