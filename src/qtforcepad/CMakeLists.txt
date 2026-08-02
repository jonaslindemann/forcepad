include_directories(../paintview ../common ../ivf2d ../calfem ../defs ../fplog)

add_definitions(-DQT_NO_KEYWORDS -DUSE_QT)

set(SOURCES
    main.cpp
    MainWindow.cpp
    QtPaintView.cpp
    ModelUrlLoader.cpp
    CalcSettingsDialog.cpp
    GeneralSettingsDialog.cpp
    AboutDialog.cpp
    OptimisationSettingsDialog.cpp
    ../paintview/PaintView.cpp
    ../common/FemGridSolver2.cpp
)

if(WIN32)
    list(APPEND SOURCES app.rc)
endif()

if(APPLE)
    list(APPEND SOURCES
        MacFileDialog.mm
    )
    set(_icns "${CMAKE_SOURCE_DIR}/images/forcepad.icns")
    set_source_files_properties("${_icns}"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    list(APPEND SOURCES "${_icns}")
endif()

set(HEADERS
    MainWindow.h
    QtPaintView.h
    MacFileDialog.h
    ModelUrlLoader.h
    CalcSettingsDialog.h
    GeneralSettingsDialog.h
    AboutDialog.h
    OptimisationSettingsDialog.h
)

qt_add_executable(qtforcepad ${SOURCES} ${HEADERS})

set(_about_logo "../../images/logo_1080.png")
set_source_files_properties(${_about_logo}
    PROPERTIES QT_RESOURCE_ALIAS "icons/logo_1080.png")

qt_add_resources(qtforcepad "qtforcepad_resources"
    PREFIX "/"
    FILES
        ../../images/splash.png
        ${_about_logo}
)

set_target_properties(qtforcepad PROPERTIES
    DEBUG_POSTFIX d
    WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
)

if(APPLE)
    target_link_libraries(qtforcepad PRIVATE
        "-framework AppKit"
        "-framework Foundation"
        "-framework UniformTypeIdentifiers"
    )

    set(FORCEPAD_VERSION "2.7")

    set_target_properties(qtforcepad PROPERTIES
        MACOSX_BUNDLE               TRUE
        OUTPUT_NAME                 "ForcePAD"
        DEBUG_POSTFIX               ""
        MACOSX_BUNDLE_GUI_IDENTIFIER "se.lth.byggmek.forcepad"
        MACOSX_BUNDLE_BUNDLE_NAME   "ForcePAD"
        MACOSX_BUNDLE_BUNDLE_VERSION "${FORCEPAD_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${FORCEPAD_VERSION}"
        MACOSX_BUNDLE_ICON_FILE     "forcepad"
        MACOSX_BUNDLE_INFO_PLIST    "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    )
endif()

target_link_libraries(qtforcepad PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Svg
    common
    ivf2d
    calfem
    fplog
    Eigen3::Eigen
    JPEG::JPEG
    PNG::PNG
    ZLIB::ZLIB
)

# System OpenGL only on desktop; the wasm build gets WebGL through Qt.
# Qt6::Network is desktop-only too: on wasm ModelUrlLoader uses emscripten's
# own XHR wrapper, which keeps the module from growing by a whole Qt module.
if(NOT EMSCRIPTEN)
    target_link_libraries(qtforcepad PRIVATE OpenGL::GL Qt6::Network)
endif()

if(OpenMP_FOUND)
    target_link_libraries(qtforcepad PRIVATE OpenMP::OpenMP_CXX)
endif()

# Copy SVG icons next to the executable.
# On macOS the bundle puts the executable in Contents/MacOS/, and
# resolveIconPath() searches relative to applicationDirPath(), so this
# lands them at Contents/MacOS/icons/ — exactly where the app looks.
# On wasm there is no host filesystem; the icons are baked into the
# Emscripten virtual FS via --preload-file instead (see the EMSCRIPTEN block).
if(NOT EMSCRIPTEN)
    add_custom_command(TARGET qtforcepad POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/images/svg"
            "$<TARGET_FILE_DIR:qtforcepad>/icons"
        COMMENT "Copying ForcePAD icons..."
    )
endif()

# Copy brushes/colormaps next to the executable, same reasoning as the icons
# copy above: PaintView::loadBrushes()/setColorMap() resolve "brushes"/
# "colormaps" relative to argv[0]'s directory, which on macOS is
# Contents/MacOS/ inside the bundle — one level below the committed
# bin/release/{brushes,colormaps} assets, so they're invisible unless copied
# in. Windows/Linux already have the exe next to those directories, so this
# is only needed on APPLE (wasm gets its own --preload-file copy below).
if(APPLE)
    add_custom_command(TARGET qtforcepad POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/bin/release/brushes"
            "$<TARGET_FILE_DIR:qtforcepad>/brushes"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/bin/release/colormaps"
            "$<TARGET_FILE_DIR:qtforcepad>/colormaps"
        COMMENT "Copying ForcePAD brushes and colormaps..."
    )
endif()

if(EMSCRIPTEN)
    # Qt generates <name>.html / qtloader.js / qtlogo.svg that load
    # "<name>.js"/"<name>.wasm" and call a JS entry function.
    # Qt6WasmMacros.cmake builds that shell from wasm_shell.html, substituting
    # @APPNAME@ from OUTPUT_NAME (falling back to the target name) and
    # @APPEXPORTNAME@ from QT_WASM_EXPORT_NAME. Setting both renames the
    # artifacts and the user-visible page title together, consistently.
    #
    # Note QT_WASM_EXPORT_NAME is used verbatim — the "_entry" suffix Qt appends
    # by default only applies when the property is unset. It is also what goes to
    # emscripten as -sEXPORT_NAME, so ForcePAD.js defines window.ForcePAD and the
    # generated shell calls exactly that.
    #
    # DEBUG_POSTFIX, by contrast, is NOT seen by that macro: it would rename the
    # .js/.wasm to ForcePADd.* while the generated html still asked for
    # ForcePAD.js, so it must stay empty here. Also collect the whole app
    # (html + js + wasm + data + loader) into one servable directory instead of
    # the split the legacy EXECUTABLE_OUTPUT_PATH would cause.
    set_target_properties(qtforcepad PROPERTIES
        DEBUG_POSTFIX ""
        OUTPUT_NAME "ForcePAD"
        QT_WASM_EXPORT_NAME "ForcePAD"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/wasm"
    )

    # Qt-for-WebAssembly link configuration.
    #   * MAX_WEBGL_VERSION=2 — allow a WebGL 2 (GLES 3.0) context; the renderer's
    #     shaders are #version 300 es, so WebGL 1 is not enough.
    #   * ALLOW_MEMORY_GROWTH — the FEM solver / Eigen allocate a lot; don't cap.
    #   * --preload-file — bake runtime assets into MEMFS:
    #       images/svg -> /icons   (toolbar icons; resolveIconPath / "/icons")
    #       brushes    -> /brushes (loadBrushes() opens the relative
    #                               "brushes/*.rgb" fallback; CWD is "/")
    #
    # NOTE: blocking nested event loops (QDialog::exec(), QMessageBox, the
    # solver's processEvents() loop) do NOT work on wasm's single browser thread
    # and render modal dialogs grey/unclosable. Emscripten -sASYNCIFY was tried
    # but conflicts with Qt's own wasm event loop ("Cannot have multiple async
    # operations in flight at once" aborts) and bloats the module ~40 MB, so it
    # is intentionally NOT used. The fix is to convert the dialogs to
    # non-blocking open()/show()+signals (a dedicated task, alongside the async
    # file open/save refactor).
    target_link_options(qtforcepad PRIVATE
        "SHELL:-s MAX_WEBGL_VERSION=2"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/images/svg@/icons"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/bin/release/brushes@/brushes"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/bin/release/colormaps@/colormaps"
    )
endif()

if(WIN32)
    target_link_libraries(qtforcepad PRIVATE winmm comctl32 gdiplus)

    # Copy Qt plugin subdirectories next to the executable.
    # vcpkg's applocal.ps1 handles the Qt DLLs; we copy the plugin dirs it misses.
    set(_vcpkg_debug   "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/debug")
    set(_vcpkg_release "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows")
    set(_vcpkg    "$<IF:$<CONFIG:Debug>,${_vcpkg_debug},${_vcpkg_release}>")
    set(_qt_plugins "${_vcpkg}/Qt6/plugins")
    add_custom_command(TARGET qtforcepad POST_BUILD
        # Plugin subdirectories (applocal.ps1 only copies top-level DLLs)
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${_qt_plugins}/platforms"
            "$<TARGET_FILE_DIR:qtforcepad>/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${_qt_plugins}/styles"
            "$<TARGET_FILE_DIR:qtforcepad>/styles"
        # Qt6Svg is a direct link dep but applocal.ps1 sometimes misses it
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${_vcpkg}/bin/$<IF:$<CONFIG:Debug>,Qt6Svgd.dll,Qt6Svg.dll>"
            "$<TARGET_FILE_DIR:qtforcepad>/"
        COMMENT "Copying Qt plugins and missing DLLs..."
    )
endif()

if(APPLE)
    # Bundle Qt frameworks and plugins into the .app so it runs standalone.
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt
        HINTS "${Qt6_DIR}/../../../tools/Qt6/bin"
        REQUIRED
    )

    # Three signing/packaging channels, independent of the Debug/Release
    # config:
    #   dev        - ad-hoc, or a local "Apple Development" identity if one
    #                is installed. Sandboxed entitlements. What CLAUDE.md's
    #                App-Sandbox/Powerbox testing notes describe. Not
    #                suitable for handing the .app to another machine.
    #   standalone - "Developer ID Application" identity, hardened runtime,
    #                no App Sandbox (sandboxing is optional outside the Mac
    #                App Store, and dropping it avoids the sandboxed
    #                Open/Save Powerbox caveats documented below). Produces
    #                a notarizable .app; see the qtforcepad_dmg /
    #                qtforcepad_notarize targets below.
    #   appstore   - "Apple Distribution" identity, sandboxed entitlements
    #                (same file as dev), for submission through App Store
    #                Connect. See the qtforcepad_pkg target below. NOTE: an
    #                embedded provisioning profile is also required for an
    #                actual submission to install — see
    #                FORCEPAD_PROVISION_PROFILE.
    set(FORCEPAD_MACOS_DISTRIBUTION "dev" CACHE STRING
        "macOS signing/packaging channel: dev, standalone (Developer ID, notarized .dmg), or appstore (Apple Distribution, .pkg)")
    set_property(CACHE FORCEPAD_MACOS_DISTRIBUTION PROPERTY STRINGS dev standalone appstore)
    message(STATUS "qtforcepad macOS distribution channel: ${FORCEPAD_MACOS_DISTRIBUTION}")

    execute_process(
        COMMAND security find-identity -v -p codesigning
        OUTPUT_VARIABLE _forcepad_codesign_identities
        ERROR_QUIET
    )

    # Falls back to ad-hoc ("-") if the channel's identity isn't found (e.g.
    # CI, or a machine without the right certificate) so the build stays
    # portable — but ad-hoc output can't be notarized or submitted, hence the
    # warnings below.
    set(_forcepad_sign_identity "-")
    set(_forcepad_entitlements "${CMAKE_CURRENT_SOURCE_DIR}/qtforcepad.entitlements")
    set(_forcepad_hardened_runtime FALSE)

    if(FORCEPAD_MACOS_DISTRIBUTION STREQUAL "standalone")
        if(_forcepad_codesign_identities MATCHES "\"(Developer ID Application: [^\"]+)\"")
            set(_forcepad_sign_identity "${CMAKE_MATCH_1}")
        else()
            message(WARNING "FORCEPAD_MACOS_DISTRIBUTION=standalone but no 'Developer ID Application' "
                             "identity was found via `security find-identity -v -p codesigning`; "
                             "falling back to ad-hoc signing, which cannot be notarized.")
        endif()
        # No entitlements file: hardened runtime alone doesn't require one
        # unless requesting a specific exception (JIT, disabled library
        # validation, etc.), and this app needs none of those.
        set(_forcepad_entitlements "")
        set(_forcepad_hardened_runtime TRUE)
    elseif(FORCEPAD_MACOS_DISTRIBUTION STREQUAL "appstore")
        # "Apple Distribution" is the current (2019+) cert name; "3rd Party
        # Mac Developer Application" is the older name for the same purpose
        # and may be what an existing Apple Developer account still has.
        if(_forcepad_codesign_identities MATCHES "\"(Apple Distribution: [^\"]+)\"")
            set(_forcepad_sign_identity "${CMAKE_MATCH_1}")
        elseif(_forcepad_codesign_identities MATCHES "\"(3rd Party Mac Developer Application: [^\"]+)\"")
            set(_forcepad_sign_identity "${CMAKE_MATCH_1}")
        else()
            message(WARNING "FORCEPAD_MACOS_DISTRIBUTION=appstore but no 'Apple Distribution' or "
                             "'3rd Party Mac Developer Application' identity was found via "
                             "`security find-identity -v -p codesigning`; falling back to ad-hoc "
                             "signing, which cannot be submitted.")
        endif()
        set(_forcepad_hardened_runtime TRUE)
    else()
        # dev: prefer a real "Apple Development" identity over ad-hoc when
        # one is installed - ad-hoc signing has no stable Team ID, which
        # matters for local App-Sandbox/Powerbox testing (see CLAUDE.md).
        if(_forcepad_codesign_identities MATCHES "\"(Apple Development: [^\"]+)\"")
            set(_forcepad_sign_identity "${CMAKE_MATCH_1}")
        endif()
    endif()
    message(STATUS "qtforcepad codesign identity: ${_forcepad_sign_identity}")

    set(_forcepad_codesign_flags --force --sign "${_forcepad_sign_identity}")
    if(_forcepad_hardened_runtime)
        list(APPEND _forcepad_codesign_flags --options runtime)
    endif()

    set(_forcepad_outer_codesign_args ${_forcepad_codesign_flags})
    if(_forcepad_entitlements)
        list(APPEND _forcepad_outer_codesign_args --entitlements "${_forcepad_entitlements}")
    endif()
    list(APPEND _forcepad_outer_codesign_args "$<TARGET_BUNDLE_DIR:qtforcepad>")

    # Sign in two passes, the Apple-documented order for complex bundles:
    #   1. --deep signs every nested framework/plugin with a plain signature
    #      (no entitlements — they're libraries, not launched processes).
    #   2. A second, non-deep pass re-signs *only* the outer app bundle with
    #      the channel's entitlements (if any), without touching the nested
    #      signatures pass 1 already applied (a non-deep codesign just reads
    #      nested frameworks as opaque resources for the outer CodeResources
    #      seal).
    # Blanket-applying --entitlements under a single --deep pass signs every
    # nested framework/plugin with the same entitlements too, which is
    # incorrect and was confirmed to break the sandboxed Open/Save panel's
    # Powerbox grant even with a real (non-ad-hoc) signing identity: `log
    # stream` showed com.apple.appkit.xpc.openAndSavePanelService and
    # com.apple.scopedbookmarksagent.xpc both running normally, yet the
    # kernel still logged "Sandbox: qtforcepad(...) deny(1) file-write-create"
    # for the exact path just chosen in the native Save panel.
    add_custom_command(TARGET qtforcepad POST_BUILD
        COMMAND "${MACDEPLOYQT_EXECUTABLE}"
            "$<TARGET_BUNDLE_DIR:qtforcepad>"
            -always-overwrite
        COMMAND codesign --deep ${_forcepad_codesign_flags}
            "$<TARGET_BUNDLE_DIR:qtforcepad>"
        COMMAND codesign ${_forcepad_outer_codesign_args}
        COMMENT "Running macdeployqt and signing (${_forcepad_sign_identity}, channel=${FORCEPAD_MACOS_DISTRIBUTION})..."
        VERBATIM
    )

    # Same "bin/release" / "bin/debug" convention as EXECUTABLE_OUTPUT_PATH
    # above, so packaged output lands next to where you'd look for the
    # config you just built - $<CONFIG> resolves from CMAKE_BUILD_TYPE on
    # this project's single-config Makefiles/Ninja generators, same as it
    # would for a multi-config generator.
    set(_forcepad_release_dir "${CMAKE_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>")

    if(FORCEPAD_MACOS_DISTRIBUTION STREQUAL "standalone")
        # qtforcepad_dmg / qtforcepad_notarize are separate targets (not
        # POST_BUILD steps on qtforcepad itself) because notarization takes
        # anywhere from under a minute to tens of minutes and shouldn't run
        # on every incremental build. Build with:
        #   cmake --build build-release --target qtforcepad_notarize
        set(FORCEPAD_NOTARY_PROFILE "" CACHE STRING
            "xcrun notarytool keychain-profile name for qtforcepad_notarize. Create once with: xcrun notarytool store-credentials <profile> --apple-id <id> --team-id <TEAMID> --password <app-specific-password>")

        set(_forcepad_dmg_staging "${CMAKE_BINARY_DIR}/dmg_staging")
        set(_forcepad_dmg_path "${_forcepad_release_dir}/ForcePAD-${FORCEPAD_VERSION}.dmg")

        add_custom_target(qtforcepad_dmg
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${_forcepad_dmg_staging}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_forcepad_dmg_staging}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_forcepad_release_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "$<TARGET_BUNDLE_DIR:qtforcepad>" "${_forcepad_dmg_staging}/ForcePAD.app"
            COMMAND ${CMAKE_COMMAND} -E create_symlink /Applications "${_forcepad_dmg_staging}/Applications"
            COMMAND hdiutil create
                -volname "ForcePAD ${FORCEPAD_VERSION}"
                -srcfolder "${_forcepad_dmg_staging}"
                -fs HFS+ -format UDZO -ov
                "${_forcepad_dmg_path}"
            DEPENDS qtforcepad
            BYPRODUCTS "${_forcepad_dmg_path}"
            COMMENT "Building ${_forcepad_dmg_path}"
            VERBATIM
        )

        add_custom_target(qtforcepad_notarize
            COMMAND ${CMAKE_COMMAND} -E echo
                "Submitting ${_forcepad_dmg_path} for notarization (keychain profile: ${FORCEPAD_NOTARY_PROFILE})"
            COMMAND xcrun notarytool submit "${_forcepad_dmg_path}"
                --keychain-profile "${FORCEPAD_NOTARY_PROFILE}" --wait
            COMMAND xcrun stapler staple "${_forcepad_dmg_path}"
            DEPENDS qtforcepad_dmg
            COMMENT "Notarizing and stapling ${_forcepad_dmg_path} (requires FORCEPAD_NOTARY_PROFILE)"
            VERBATIM
        )
        if(NOT FORCEPAD_NOTARY_PROFILE)
            message(STATUS "qtforcepad_notarize: FORCEPAD_NOTARY_PROFILE is not set - run "
                            "`xcrun notarytool store-credentials <profile> --apple-id <id> --team-id <TEAMID> "
                            "--password <app-specific-password>` once, then reconfigure with "
                            "-DFORCEPAD_NOTARY_PROFILE=<profile>")
        endif()
    endif()

    if(FORCEPAD_MACOS_DISTRIBUTION STREQUAL "appstore")
        set(FORCEPAD_PROVISION_PROFILE "${CMAKE_CURRENT_SOURCE_DIR}/embedded.provisionprofile" CACHE FILEPATH
            "Mac App Store provisioning profile embedded as Contents/embedded.provisionprofile. Not needed to build the .app, but needed before a Store-signed .pkg will actually install.")

        if(EXISTS "${FORCEPAD_PROVISION_PROFILE}")
            add_custom_command(TARGET qtforcepad POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                    "${FORCEPAD_PROVISION_PROFILE}"
                    "$<TARGET_BUNDLE_DIR:qtforcepad>/Contents/embedded.provisionprofile"
                COMMENT "Embedding Mac App Store provisioning profile..."
            )
        else()
            message(WARNING "appstore channel: FORCEPAD_PROVISION_PROFILE (${FORCEPAD_PROVISION_PROFILE}) "
                             "not found - the .app/.pkg will still build and sign locally, but App Store "
                             "Connect will reject it without an embedded provisioning profile.")
        endif()

        set(_forcepad_pkg_path "${_forcepad_release_dir}/ForcePAD-${FORCEPAD_VERSION}.pkg")
        set(_forcepad_installer_identity "-")
        if(_forcepad_codesign_identities MATCHES "\"(3rd Party Mac Developer Installer: [^\"]+)\"")
            set(_forcepad_installer_identity "${CMAKE_MATCH_1}")
        elseif(_forcepad_codesign_identities MATCHES "\"(Mac Installer Distribution: [^\"]+)\"")
            set(_forcepad_installer_identity "${CMAKE_MATCH_1}")
        endif()

        add_custom_target(qtforcepad_pkg
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_forcepad_release_dir}"
            COMMAND productbuild --component
                "$<TARGET_BUNDLE_DIR:qtforcepad>" /Applications
                --sign "${_forcepad_installer_identity}"
                "${_forcepad_pkg_path}"
            DEPENDS qtforcepad
            BYPRODUCTS "${_forcepad_pkg_path}"
            COMMENT "Building ${_forcepad_pkg_path} (installer identity: ${_forcepad_installer_identity})"
            VERBATIM
        )
        if(_forcepad_installer_identity STREQUAL "-")
            message(WARNING "appstore channel: no 'Mac Installer Distribution' / '3rd Party Mac Developer "
                             "Installer' identity found; qtforcepad_pkg will fail to sign until one is "
                             "installed in the keychain.")
        endif()
    endif()
endif()
