# MsixStage.cmake - script-mode helper for the qtforcepad_msix target.
#
# Builds a clean MSIX payload layout in STAGE_DIR from the deployed
# executable directory. Run via `cmake -P`, not included into the project:
#
#   cmake -DSTAGE_DIR=... -DPAYLOAD_DIR=... -DEXE_NAME=qtforcepad.exe
#         -DMANIFEST=... -DASSETS_DIR=... [-DVCRUNTIME_DIR=...]
#         -P cmake/MsixStage.cmake
#
# This is a script rather than a series of `cmake -E copy` commands in the
# target because the DLL set has to be globbed at *build* time: vcpkg's
# applocal.ps1 drops the Qt/ICU/OpenSSL/... DLLs next to the executable as a
# post-build step, so a configure-time glob would come up empty or stale.
#
# It stages into a fresh directory instead of packing bin/release directly:
# that tree also holds forcepad.exe, kiosk/, test.fp2 and
# forcepad_kiosk_auto_restart.cmd, none of which should ship.

cmake_minimum_required(VERSION 3.20)

foreach(_required STAGE_DIR PAYLOAD_DIR EXE_NAME MANIFEST ASSETS_DIR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "MsixStage.cmake: -D${_required}=<value> is required")
    endif()
endforeach()

if(NOT EXISTS "${PAYLOAD_DIR}/${EXE_NAME}")
    message(FATAL_ERROR
        "MsixStage.cmake: ${PAYLOAD_DIR}/${EXE_NAME} not found - build the "
        "qtforcepad target for this configuration first.")
endif()

file(REMOVE_RECURSE "${STAGE_DIR}")
file(MAKE_DIRECTORY "${STAGE_DIR}")

file(COPY "${PAYLOAD_DIR}/${EXE_NAME}" DESTINATION "${STAGE_DIR}")

# Qt6*.dll, icu*.dll, libcrypto, freetype, ... - everything applocal.ps1 and
# the CMakeLists post-build step deposited beside the executable.
file(GLOB _fp_dlls "${PAYLOAD_DIR}/*.dll")
if(_fp_dlls)
    file(COPY ${_fp_dlls} DESTINATION "${STAGE_DIR}")
else()
    message(WARNING "MsixStage.cmake: no DLLs found in ${PAYLOAD_DIR} - the "
                    "packaged app will not start. Was the target actually built?")
endif()

# Same directory set the Inno Setup installer ships (install/win32/forcepad2.iss),
# minus the kiosk build. PaintView::loadBrushes()/setColorMap() and
# MainWindow's resolveIconPath() all resolve these relative to
# QCoreApplication::applicationDirPath(), which inside an installed MSIX is
# the package root - so this flat layout is exactly what they expect.
#
# The .pdb exclusion is not cosmetic: vcpkg ships debug symbols alongside the
# Qt plugin DLLs, and they accounted for 39 MB of a 43 MB package before this.
foreach(_fp_dir brushes colormaps icons platforms samples styles)
    if(EXISTS "${PAYLOAD_DIR}/${_fp_dir}")
        file(COPY "${PAYLOAD_DIR}/${_fp_dir}" DESTINATION "${STAGE_DIR}"
             PATTERN "*.pdb" EXCLUDE)
    else()
        message(WARNING "MsixStage.cmake: ${PAYLOAD_DIR}/${_fp_dir} is missing "
                        "and will not be packaged.")
    endif()
endforeach()

# The VC++ runtime has to travel app-local: an MSIX package has no equivalent
# of the installer's [Run] section, so VC_redist.x64.exe can never execute.
# Redistributing these DLLs next to the executable is explicitly permitted by
# the Visual C++ redistributable licence. A static CRT is not an alternative
# here - vcpkg's Qt is built against the dynamic one.
if(DEFINED VCRUNTIME_DIR AND NOT "${VCRUNTIME_DIR}" STREQUAL "")
    file(GLOB _fp_vcrt
        "${VCRUNTIME_DIR}/msvcp140*.dll"
        "${VCRUNTIME_DIR}/vcruntime140*.dll"
        "${VCRUNTIME_DIR}/concrt140.dll"
    )
    if(_fp_vcrt)
        file(COPY ${_fp_vcrt} DESTINATION "${STAGE_DIR}")
    else()
        message(WARNING "MsixStage.cmake: no VC++ runtime DLLs found in "
                        "${VCRUNTIME_DIR}; the package will only run on machines "
                        "that already have the redistributable installed.")
    endif()
else()
    message(WARNING "MsixStage.cmake: VCRUNTIME_DIR was not supplied; the "
                    "package will only run on machines that already have the "
                    "VC++ redistributable installed.")
endif()

# Store tile/logo assets. The whole scale-* set is copied so that makepri can
# index the high-DPI variants; the unqualified filenames the manifest points
# at are what Windows falls back to when no resources.pri is present.
file(GLOB _fp_assets "${ASSETS_DIR}/*.png")
if(NOT _fp_assets)
    message(FATAL_ERROR "MsixStage.cmake: no PNG assets found in ${ASSETS_DIR}")
endif()
file(MAKE_DIRECTORY "${STAGE_DIR}/Assets")
file(COPY ${_fp_assets} DESTINATION "${STAGE_DIR}/Assets")

file(COPY "${MANIFEST}" DESTINATION "${STAGE_DIR}")

message(STATUS "MSIX payload staged in ${STAGE_DIR}")
