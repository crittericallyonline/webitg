cmake_minimum_required(VERSION 3.10.0)
project(WebITG VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_EXECUTABLE_SUFFIX ".js")
set(CMAKE_CXX_COMPILER em++)
set(CMAKE_C_COMPILER emcc)
set(NDEBUG 0)
set(WITHOUT_NETWORKING 1)
set(HAVE_FFMPEG 1)
set(HAVE_SDL 0) # i dont like SDL, too complicated rn
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
if(NDEBUG)
add_compile_definitions(NDEBUG CRASH_HANDLER)
endif()

set(CMAKE_MAKE_PROGRAM="make -j6")

if(WITHOUT_NETWORKING)
    add_compile_definitions(WITHOUT_NETWORKING)
else()
    file(GLOB SOURCES APPEND src/ScreenGameplayMultiplayer.cpp src/ScreenSMOnlineLogin.cpp)
endif()

if(HAVE_FFMPEG)
    add_compile_definitions(HAVE_FFMPEG)
    file(GLOB SOURCES APPEND src/arch/MovieTexture/MovieTexture_FFMpeg.cpp)
endif(HAVE_FFMPEG)

if(HAVE_SDL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_SDL=1")
    file(GLOB SOURCES APPEND
    src/arch/LoadingWindow/LoadingWindow_SDL.cpp
    src/SDL_utils.cpp
    src/arch/InputHandler/InputHandler_SDL.cpp
    src/arch/LowLevelWindow/LowLevelWindow_SDL.cpp
    )
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --use-port=contrib.glfw3 --use-port=libpng")
    file(GLOB SOURCES APPEND
    src/arch/LoadingWindow/LoadingWindow_Emscripten.cpp
    src/arch/InputHandler/InputHandler_Emscripten.cpp
    src/arch/LowLevelWindow/LowLevelWindow_Emscripten.cpp
    )
endif()

# src/arch/Lights/LightsDriver_LinuxWeedTech.cpp src/arch/Lights/LightsDriver_LinuxParallel.cpp

add_executable(WebITG
    src/Screen.cpp src/ScreenArcadeDiagnostics.cpp src/ScreenArcadePatch.cpp src/ScreenArcadeStart.cpp src/ScreenAttract.cpp src/ScreenBookkeeping.cpp src/ScreenBranch.cpp src/ScreenCenterImage.cpp src/ScreenCredits.cpp src/ScreenDebugOverlay.cpp src/ScreenDemonstration.cpp src/ScreenEdit.cpp src/ScreenEditCoursesMenu.cpp src/EditCoursesSongMenu.cpp src/ScreenEditMenu.cpp src/ScreenEnding.cpp src/ScreenEndlessBreak.cpp src/ScreenEvaluation.cpp src/ScreenExit.cpp src/ScreenNetEvaluation.cpp src/ScreenNetSelectMusic.cpp src/ScreenNetSelectBase.cpp src/ScreenNetRoom.cpp src/ScreenEz2SelectMusic.cpp src/ScreenEz2SelectPlayer.cpp src/ScreenGameplay.cpp src/ScreenHowToPlay.cpp src/ScreenInstructions.cpp src/ScreenJukebox.cpp src/ScreenLogo.cpp src/ScreenMapControllers.cpp src/ScreenMessage.cpp src/ScreenMiniMenu.cpp src/ScreenMusicScroll.cpp src/ScreenNameEntry.cpp src/ScreenNameEntryTraditional.cpp src/ScreenOptions.cpp src/ScreenOptionsMaster.cpp src/ScreenOptionsMasterPrefs.cpp src/ScreenPackages.cpp src/ScreenPlayerOptions.cpp src/ScreenNetworkOptions.cpp src/ScreenPlayLights.cpp src/ScreenProfileOptions.cpp src/ScreenPrompt.cpp src/ScreenRanking.cpp src/ScreenReloadSongs.cpp src/ScreenSandbox.cpp src/ScreenSaveSync.cpp src/ScreenSelect.cpp src/ScreenSelectCharacter.cpp src/ScreenSelectDifficulty.cpp src/ScreenSelectGroup.cpp src/ScreenSelectMaster.cpp src/ScreenSelectMode.cpp src/ScreenSelectMusic.cpp src/ScreenSelectStyle.cpp src/ScreenSyncOverlay.cpp src/ScreenSystemLayer.cpp src/ScreenSetTime.cpp src/ScreenSongOptions.cpp src/ScreenSplash.cpp src/ScreenStage.cpp src/ScreenTest.cpp src/ScreenTestFonts.cpp src/ScreenTestInput.cpp src/ScreenTestLights.cpp src/ScreenTestSound.cpp src/ScreenTextEntry.cpp src/ScreenTitleMenu.cpp src/ScreenUnlock.cpp src/ScreenUserPacks.cpp src/ScreenWithMenuElements.cpp src/ActorCommands.cpp src/Attack.cpp src/AutoKeysounds.cpp src/BackgroundUtil.cpp src/BannerCache.cpp src/CatalogXml.cpp src/Character.cpp src/CodeDetector.cpp src/Command.cpp src/CommonMetrics.cpp src/DiagnosticsUtil.cpp src/Difficulty.cpp src/EnumHelper.cpp src/Course.cpp src/CourseUtil.cpp src/DateTime.cpp src/Font.cpp src/FontCharAliases.cpp src/FontCharmaps.cpp src/Game.cpp src/GameCommand.cpp src/GameConstantsAndTypes.cpp src/GameInput.cpp src/Grade.cpp src/HighScore.cpp src/Inventory.cpp src/LightsMapper.cpp src/LuaReference.cpp src/LyricsLoader.cpp src/NoteData.cpp src/NoteDataUtil.cpp src/NoteDataWithScoring.cpp src/NoteFieldPositioning.cpp src/NoteTypes.cpp src/NotesLoader.cpp src/NotesLoaderBMS.cpp src/NotesLoaderDWI.cpp src/NotesLoaderKSF.cpp src/NotesLoaderSM.cpp src/NotesWriterDWI.cpp src/NotesWriterSM.cpp src/OptionRowHandler.cpp src/PlayerAI.cpp src/PlayerNumber.cpp src/PlayerOptions.cpp src/PlayerStageStats.cpp src/PlayerState.cpp src/Preference.cpp src/Profile.cpp src/RandomSample.cpp src/RadarValues.cpp src/RoomWheel.cpp src/ScreenDimensions.cpp src/ScoreKeeperMAX2.cpp src/ScoreKeeperRave.cpp src/Song.cpp src/SongCacheIndex.cpp src/SongOptions.cpp src/SongUtil.cpp src/StageStats.cpp src/Steps.cpp src/StepsUtil.cpp src/Style.cpp src/StyleUtil.cpp src/TimingData.cpp src/Trail.cpp src/TrailUtil.cpp src/TitleSubstitution.cpp src/io/USBDevice_Libusb.cpp src/IniFile.cpp src/MsdFile.cpp src/XmlFile.cpp src/StepMania.cpp src/global.cpp src/arch/LoadingWindow/LoadingWindow.cpp src/arch/Sound/RageSoundDriver.cpp src/arch/Sound/RageSoundDriver_Null.cpp src/arch/Sound/RageSoundDriver_Generic_Software.cpp src/arch/ArchHooks/ArchHooks.cpp src/arch/ArchHooks/ArchHooks_Emscripten.cpp src/arch/InputHandler/InputHandler.cpp src/arch/InputHandler/InputHandler_MonkeyKeyboard.cpp src/arch/InputHandler/InputHandler_PIUIO.cpp src/arch/InputHandler/InputHandler_PIUIO_Helper.cpp src/arch/InputHandler/InputHandler_Iow.cpp src/arch/InputHandler/InputHandler_MiniMaid.cpp src/arch/InputHandler/InputHandler_P3IO.cpp src/arch/MovieTexture/MovieTexture.cpp src/arch/MovieTexture/MovieTexture_Null.cpp src/arch/MovieTexture/MovieTexture_FFMpeg.cpp src/arch/MovieTexture/FFMpeg_Helper.cpp src/arch/Lights/LightsDriver.cpp src/arch/Lights/LightsDriver_SystemMessage.cpp src/arch/Lights/LightsDriver_ActorMessage.cpp src/arch/Lights/LightsDriver_External.cpp src/arch/Lights/LightsDriver_PacDrive.cpp src/arch/Lights/LightsDriver_G15.cpp src/arch/Lights/LightsDriver_EXTIO.cpp src/arch/Lights/LightsDriver_Satellite.cpp src/arch/MemoryCard/MemoryCardDriver.cpp src/arch/LowLevelWindow/LowLevelWindow.cpp src/archutils/Emscripten/AssertionHandler.cpp src/archutils/Emscripten/GetSysInfo.cpp src/archutils/Emscripten/SignalHandler.cpp src/archutils/Emscripten/EmscriptenThreadHelpers.cpp src/archutils/Emscripten/RunningUnderValgrind.cpp src/archutils/Emscripten/EmergencyShutdown.cpp src/arch/Dialog/Dialog.cpp src/arch/Dialog/DialogDriver.cpp src/arch/Threads/Threads_Pthreads.cpp src/arch/USB/USBDriver_Impl.cpp src/arch/RageDriver.cpp src/arch/ACIO/ACIO.cpp src/arch/COM/serial.cc src/arch/COM/impl/unix.cc src/arch/COM/impl/list_ports/list_ports_linux.cc src/BGAnimation.cpp src/BGAnimationLayer.cpp src/Banner.cpp src/DifficultyIcon.cpp src/MeterDisplay.cpp src/StreamDisplay.cpp src/Transition.cpp src/ActiveAttackList.cpp src/BPMDisplay.cpp src/ComboGraph.cpp src/CourseContentsList.cpp src/CourseEntryDisplay.cpp src/DifficultyDisplay.cpp src/DifficultyList.cpp src/DifficultyMeter.cpp src/DifficultyRating.cpp src/DualScrollBar.cpp src/EditCoursesMenu.cpp src/EditMenu.cpp src/FadingBanner.cpp src/GradeDisplay.cpp src/GraphDisplay.cpp src/GrooveGraph.cpp src/GrooveRadar.cpp src/GroupList.cpp src/HelpDisplay.cpp src/LinkedOptionsMenu.cpp src/MemoryCardDisplay.cpp src/MenuTimer.cpp src/ModeSwitcher.cpp src/MusicBannerWheel.cpp src/MusicList.cpp src/MusicSortDisplay.cpp src/MusicWheel.cpp src/MusicWheelItem.cpp src/OptionIcon.cpp src/OptionIconRow.cpp src/OptionsList.cpp src/OptionRow.cpp src/OptionsCursor.cpp src/PaneDisplay.cpp src/ScrollBar.cpp src/ScrollingList.cpp src/SnapDisplay.cpp src/TextBanner.cpp src/WheelBase.cpp src/WheelItemBase.cpp src/WheelNotifyIcon.cpp src/ArrowEffects.cpp src/AttackDisplay.cpp src/Background.cpp src/BeginnerHelper.cpp src/CombinedLifeMeterTug.cpp src/Combo.cpp src/DancingCharacters.cpp src/Foreground.cpp src/GhostArrow.cpp src/GhostArrowRow.cpp src/HoldGhostArrow.cpp src/HoldJudgment.cpp src/Judgment.cpp src/LifeMeterBar.cpp src/LifeMeterBattery.cpp src/LifeMeterTime.cpp src/LyricDisplay.cpp src/NoteDisplay.cpp src/NoteField.cpp src/PercentageDisplay.cpp src/Player.cpp src/ReceptorArrow.cpp src/ReceptorArrowRow.cpp src/ScoreDisplay.cpp src/ScoreDisplayBattle.cpp src/ScoreDisplayCalories.cpp src/ScoreDisplayLifeTime.cpp src/ScoreDisplayNormal.cpp src/ScoreDisplayOni.cpp src/ScoreDisplayPercentage.cpp src/ScoreDisplayRave.cpp src/pcre/get.c src/pcre/maketables.c src/pcre/pcre.c src/pcre/study.c src/RageFileBasic.cpp src/RageFile.cpp src/RageFileDriver.cpp src/RageFileManager.cpp src/RageFileDriverDirect.cpp src/RageFileDriverDirectHelpers.cpp src/RageFileDriverMemory.cpp src/RageFileDriverZip.cpp src/RageFileDriverDeflate.cpp src/RageFileDriverSlice.cpp src/RageFileDriverTimeout.cpp src/RageFileDriverCrypt.cpp src/RageFileDriverCrypt_ITG2.cpp src/RageFileDriverProbe.cpp src/RageSoundReader_WAV.cpp src/RageSoundReader_Vorbisfile.cpp src/RageSoundReader_MP3.cpp src/RageBitmapTexture.cpp src/RageDisplay.cpp src/RageDisplay_OGL.cpp src/RageDisplay_GLFW.cpp src/RageDisplay_Null.cpp src/RageException.cpp src/RageInput.cpp src/RageInputDevice.cpp src/RageLog.cpp src/RageMath.cpp src/RageModelGeometry.cpp src/RageSound.cpp src/RageSoundManager.cpp src/RageSoundUtil.cpp src/RageSoundMixBuffer.cpp src/RageSoundPosMap.cpp src/RageSoundReader_FileReader.cpp src/RageSoundReader_Preload.cpp src/RageSoundReader_Resample.cpp src/RageSoundReader_Resample_Fast.cpp src/RageSoundReader_Resample_Good.cpp src/RageSoundReader_Chain.cpp src/RageSoundResampler.cpp src/RageSurface.cpp src/RageSurfaceUtils.cpp src/RageSurfaceUtils_Dither.cpp src/RageSurface_Save_JPEG.cpp src/RageSurfaceUtils_Palettize.cpp src/RageSurfaceUtils_Zoom.cpp src/RageSurface_Load.cpp src/RageSurface_Load_PNG.cpp src/RageSurface_Load_JPEG.cpp src/RageSurface_Load_GIF.cpp src/RageSurface_Load_BMP.cpp src/RageSurface_Load_XPM.cpp src/RageTexture.cpp src/RageSurface_Save_BMP.cpp src/RageTextureID.cpp src/RageTextureManager.cpp src/RageThreads.cpp src/RageTimer.cpp src/RageUtil.cpp src/RageUtil_CharConversions.cpp src/RageUtil_BackgroundLoader.cpp src/RageUtil_FileDB.cpp src/RageUtil_WorkerThread.cpp src/Actor.cpp src/ActorFrame.cpp src/ActorScroller.cpp src/ActorUtil.cpp src/AutoActor.cpp src/BitmapText.cpp src/Model.cpp src/ModelManager.cpp src/ModelTypes.cpp src/Quad.cpp src/RollingNumbers.cpp src/Sprite.cpp src/AnnouncerManager.cpp src/Bookkeeper.cpp src/CryptManager.cpp src/FontManager.cpp src/GameSoundManager.cpp src/GameManager.cpp src/GameState.cpp src/InputFilter.cpp src/InputMapper.cpp src/InputQueue.cpp src/LuaManager.cpp src/LightsManager.cpp src/MemoryCardManager.cpp src/MessageManager.cpp src/NetworkSyncManager.cpp src/NetworkSyncServer.cpp src/NoteSkinManager.cpp src/PrefsManager.cpp src/ProfileManager.cpp src/ScreenManager.cpp src/SongManager.cpp src/StatsManager.cpp src/ThemeManager.cpp src/UnlockManager.cpp src/UserPackManager.cpp src/ProductInfo.cpp src/ibutton/owerr.c src/ibutton/ownetu.c src/ibutton/owllu.c src/ibutton/owsesu.c src/ibutton/owtrnu.c src/ibutton/crcutil.c src/ibutton/ds2480ut.c src/ibutton/linuxlnk.c src/ibutton/sha18.c src/ibutton/shaib.c src/iButton.cpp src/aes/aescrypt.c src/aes/aeskey.c src/aes/aestab.c src/crypto/CryptBn.cpp src/crypto/CryptMD5.cpp src/crypto/CryptNoise.cpp src/crypto/CryptPrime.cpp src/crypto/CryptRSA.cpp src/crypto/CryptRand.cpp src/crypto/CryptSH512.cpp src/crypto/CryptSHA.cpp src/CryptHelpers.cpp src/timer/Timer.cpp
)



# screens

# add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(WebITG BEFORE PRIVATE
    src/
    src/mad-0.15.1b
    src/lua-5.0/include/
    src/libusb/include
    src/libtomcrypt/src/headers
    extern/FFmpeg/include
    extern/FFmpeg/
)


include(CTest)
enable_testing()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

