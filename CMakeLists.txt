cmake_minimum_required(VERSION 3.10.0)
project(WebITG VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_EXECUTABLE_SUFFIX ".js")
set(CMAKE_CXX_COMPILER em++)
set(CMAKE_C_COMPILER emcc)
set(CMAKE_AR emar)
set(CMAKE_RANLIB emranlib)
set(CMAKE_CXX_FLAGS_RELEASE "-Os")

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/archive)
set(NDEBUG 0)
set(WITHOUT_NETWORKING 1)
set(HAVE_FFMPEG 1)
set(HAVE_CONFIG_H 1)
set(HAVE_SDL 0) # i dont like SDL, too complicated rn
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --use-port=vorbis --use-port=ogg --use-port=libpng --use-port=libjpeg")
# --closure 1
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -DDEBUG -sWASM_ASYNC_COMPILATION -g -O0 -sWASM -sALLOW_MEMORY_GROWTH  -sGL_UNSAFE_OPTS -sINVOKE_RUN=1 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2 -sUSE_GLFW=1 -sFULL_ES3 -sEXPORT_ALL=1 -sASSERTIONS=0") #  -sTOTAL_STACK=512mb
# CRASH_HANDLER
add_compile_definitions(HAVE_FFMPEG HAS_USBDRIVER_IMPL_WINUSB=0 HAS_USBDRIVER_IMPL_LIBUSB=0)

# could  make this a target definition instead of it being global
if(NDEBUG)
add_compile_definitions(NDEBUG FPM_64BIT)
endif()

if(HAVE_CONFIG_H)
add_compile_definitions(HAVE_CONFIG_H)
endif()

set(CMAKE_MAKE_PROGRAM="emmake make -j")

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --use-port=contrib.glfw3")
    file(GLOB SOURCES APPEND
    # thinking of renaming these from "Emscripten" to "GLFW"
    src/arch/LoadingWindow/LoadingWindow_Emscripten.cpp
    src/arch/InputHandler/InputHandler_Emscripten.cpp
    src/arch/LowLevelWindow/LowLevelWindow_GLFW.cpp
    )
endif()

# src/arch/Lights/LightsDriver_LinuxWeedTech.cpp src/arch/Lights/LightsDriver_LinuxParallel.cpp

add_executable(WebITG
    ${SOURCES}
    src/arch/ArchHooks/ArchHooks_Emscripten.cpp
    src/arch/LowLevelWindow/LowLevelWindow_Null.cpp
    src/io/ITGIO.cpp
    src/io/USBDriver.cpp
    src/io/USBDevice.cpp
    src/arch/USB/USBDriver_Impl.cpp
    # src/arch/InputHandler/InputHandler_PIUIO.cpp
    # src/arch/InputHandler/InputHandler_PIUIO_Helper.cpp
    src/arch/Lights/LightsDriver.cpp
    src/arch/Lights/LightsDriver_SystemMessage.cpp
    src/arch/Lights/LightsDriver_ActorMessage.cpp
    src/arch/Lights/LightsDriver_External.cpp # src/arch/Lights/LightsDriver_PacDrive.cpp src/arch/Lights/LightsDriver_G15.cpp src/arch/Lights/LightsDriver_EXTIO.cpp
    src/arch/Lights/LightsDriver_Satellite.cpp
    src/Screen.cpp src/ScreenArcadeDiagnostics.cpp src/ScreenArcadePatch.cpp src/ScreenArcadeStart.cpp src/ScreenAttract.cpp src/ScreenBookkeeping.cpp src/ScreenBranch.cpp src/ScreenCenterImage.cpp src/ScreenCredits.cpp src/ScreenDebugOverlay.cpp src/ScreenDemonstration.cpp src/ScreenEdit.cpp src/ScreenEditCoursesMenu.cpp src/EditCoursesSongMenu.cpp src/ScreenEditMenu.cpp src/ScreenEnding.cpp src/ScreenEndlessBreak.cpp src/ScreenEvaluation.cpp src/ScreenExit.cpp src/ScreenNetEvaluation.cpp src/ScreenNetSelectMusic.cpp src/ScreenNetSelectBase.cpp src/ScreenNetRoom.cpp src/ScreenEz2SelectMusic.cpp src/ScreenEz2SelectPlayer.cpp src/ScreenGameplay.cpp src/ScreenHowToPlay.cpp src/ScreenInstructions.cpp src/ScreenJukebox.cpp src/ScreenLogo.cpp src/ScreenMapControllers.cpp src/ScreenMessage.cpp src/ScreenMiniMenu.cpp src/ScreenMusicScroll.cpp src/ScreenNameEntry.cpp src/ScreenNameEntryTraditional.cpp src/ScreenOptions.cpp src/ScreenOptionsMaster.cpp src/ScreenOptionsMasterPrefs.cpp src/ScreenPackages.cpp src/ScreenPlayerOptions.cpp src/ScreenNetworkOptions.cpp src/ScreenPlayLights.cpp src/ScreenProfileOptions.cpp src/ScreenPrompt.cpp src/ScreenRanking.cpp src/ScreenReloadSongs.cpp src/ScreenSandbox.cpp src/ScreenSaveSync.cpp src/ScreenSelect.cpp src/ScreenSelectCharacter.cpp src/ScreenSelectDifficulty.cpp src/ScreenSelectGroup.cpp src/ScreenSelectMaster.cpp src/ScreenSelectMode.cpp src/ScreenSelectMusic.cpp src/ScreenSelectStyle.cpp src/ScreenSyncOverlay.cpp src/ScreenSystemLayer.cpp src/ScreenSetTime.cpp src/ScreenSongOptions.cpp src/ScreenSplash.cpp src/ScreenStage.cpp src/ScreenTest.cpp src/ScreenTestFonts.cpp src/ScreenTestInput.cpp src/ScreenTestLights.cpp src/ScreenTestSound.cpp src/ScreenTextEntry.cpp src/ScreenTitleMenu.cpp src/ScreenUnlock.cpp src/ScreenUserPacks.cpp src/ScreenWithMenuElements.cpp src/ActorCommands.cpp src/Attack.cpp src/AutoKeysounds.cpp src/BackgroundUtil.cpp src/BannerCache.cpp src/CatalogXml.cpp src/Character.cpp src/CodeDetector.cpp src/Command.cpp src/CommonMetrics.cpp src/DiagnosticsUtil.cpp src/Difficulty.cpp src/EnumHelper.cpp src/Course.cpp src/CourseUtil.cpp src/DateTime.cpp src/Font.cpp src/FontCharAliases.cpp src/FontCharmaps.cpp src/Game.cpp src/GameCommand.cpp src/GameConstantsAndTypes.cpp src/GameInput.cpp src/Grade.cpp src/HighScore.cpp src/Inventory.cpp src/LightsMapper.cpp src/LuaReference.cpp src/LyricsLoader.cpp src/NoteData.cpp src/NoteDataUtil.cpp src/NoteDataWithScoring.cpp src/NoteFieldPositioning.cpp src/NoteTypes.cpp src/NotesLoader.cpp src/NotesLoaderBMS.cpp src/NotesLoaderDWI.cpp src/NotesLoaderKSF.cpp src/NotesLoaderSM.cpp src/NotesWriterDWI.cpp src/NotesWriterSM.cpp src/OptionRowHandler.cpp src/PlayerAI.cpp src/PlayerNumber.cpp src/PlayerOptions.cpp src/PlayerStageStats.cpp src/PlayerState.cpp src/Preference.cpp src/Profile.cpp src/RandomSample.cpp src/RadarValues.cpp src/RoomWheel.cpp src/ScreenDimensions.cpp src/ScoreKeeperMAX2.cpp src/ScoreKeeperRave.cpp src/Song.cpp src/SongCacheIndex.cpp src/SongOptions.cpp src/SongUtil.cpp src/StageStats.cpp src/Steps.cpp src/StepsUtil.cpp src/Style.cpp src/StyleUtil.cpp src/TimingData.cpp src/Trail.cpp src/TrailUtil.cpp src/TitleSubstitution.cpp src/IniFile.cpp src/MsdFile.cpp src/XmlFile.cpp src/StepMania.cpp src/global.cpp src/arch/LoadingWindow/LoadingWindow.cpp src/arch/Sound/RageSoundDriver.cpp src/arch/Sound/RageSoundDriver_Null.cpp src/arch/Sound/RageSoundDriver_Generic_Software.cpp src/arch/ArchHooks/ArchHooks.cpp src/arch/InputHandler/InputHandler.cpp src/arch/InputHandler/InputHandler_MonkeyKeyboard.cpp src/arch/InputHandler/InputHandler_Iow.cpp src/arch/MovieTexture/MovieTexture.cpp src/arch/MovieTexture/MovieTexture_Null.cpp src/arch/MovieTexture/MovieTexture_FFMpeg.cpp src/arch/MovieTexture/FFMpeg_Helper.cpp src/arch/MemoryCard/MemoryCardDriver.cpp src/arch/LowLevelWindow/LowLevelWindow.cpp src/archutils/Emscripten/AssertionHandler.cpp src/archutils/Emscripten/GetSysInfo.cpp src/archutils/Emscripten/SignalHandler.cpp src/archutils/Emscripten/EmscriptenThreadHelpers.cpp src/archutils/Emscripten/RunningUnderValgrind.cpp src/archutils/Emscripten/EmergencyShutdown.cpp src/arch/Dialog/Dialog.cpp src/arch/Dialog/DialogDriver.cpp src/arch/Threads/Threads_Pthreads.cpp src/arch/RageDriver.cpp src/arch/ACIO/ACIO.cpp src/arch/COM/serial.cc src/arch/COM/impl/unix.cc src/arch/COM/impl/list_ports/list_ports_linux.cc src/BGAnimation.cpp src/BGAnimationLayer.cpp src/Banner.cpp src/DifficultyIcon.cpp src/MeterDisplay.cpp src/StreamDisplay.cpp src/Transition.cpp src/ActiveAttackList.cpp src/BPMDisplay.cpp src/ComboGraph.cpp src/CourseContentsList.cpp src/CourseEntryDisplay.cpp src/DifficultyDisplay.cpp src/DifficultyList.cpp src/DifficultyMeter.cpp src/DifficultyRating.cpp src/DualScrollBar.cpp src/EditCoursesMenu.cpp src/EditMenu.cpp src/FadingBanner.cpp src/GradeDisplay.cpp src/GraphDisplay.cpp src/GrooveGraph.cpp src/GrooveRadar.cpp src/GroupList.cpp src/HelpDisplay.cpp src/LinkedOptionsMenu.cpp src/MemoryCardDisplay.cpp src/MenuTimer.cpp src/ModeSwitcher.cpp src/MusicBannerWheel.cpp src/MusicList.cpp src/MusicSortDisplay.cpp src/MusicWheel.cpp src/MusicWheelItem.cpp src/OptionIcon.cpp src/OptionIconRow.cpp src/OptionsList.cpp src/OptionRow.cpp src/OptionsCursor.cpp src/PaneDisplay.cpp src/ScrollBar.cpp src/ScrollingList.cpp src/SnapDisplay.cpp src/TextBanner.cpp src/WheelBase.cpp src/WheelItemBase.cpp src/WheelNotifyIcon.cpp src/ArrowEffects.cpp src/AttackDisplay.cpp src/Background.cpp src/BeginnerHelper.cpp src/CombinedLifeMeterTug.cpp src/Combo.cpp src/DancingCharacters.cpp src/Foreground.cpp src/GhostArrow.cpp src/GhostArrowRow.cpp src/HoldGhostArrow.cpp src/HoldJudgment.cpp src/Judgment.cpp src/LifeMeterBar.cpp src/LifeMeterBattery.cpp src/LifeMeterTime.cpp src/LyricDisplay.cpp src/NoteDisplay.cpp src/NoteField.cpp src/PercentageDisplay.cpp src/Player.cpp src/ReceptorArrow.cpp src/ReceptorArrowRow.cpp src/ScoreDisplay.cpp src/ScoreDisplayBattle.cpp src/ScoreDisplayCalories.cpp src/ScoreDisplayLifeTime.cpp src/ScoreDisplayNormal.cpp src/ScoreDisplayOni.cpp src/ScoreDisplayPercentage.cpp src/ScoreDisplayRave.cpp src/pcre/get.c src/pcre/maketables.c src/pcre/pcre.c src/pcre/study.c src/RageFileBasic.cpp src/RageFile.cpp src/RageFileDriver.cpp src/RageFileManager.cpp src/RageFileDriverDirect.cpp src/RageFileDriverDirectHelpers.cpp src/RageFileDriverMemory.cpp src/RageFileDriverZip.cpp src/RageFileDriverDeflate.cpp src/RageFileDriverSlice.cpp src/RageFileDriverTimeout.cpp src/RageFileDriverCrypt.cpp src/RageFileDriverCrypt_ITG2.cpp src/RageFileDriverProbe.cpp src/RageSoundReader_WAV.cpp src/RageSoundReader_Vorbisfile.cpp src/RageSoundReader_MP3.cpp src/RageBitmapTexture.cpp src/RageDisplay.cpp src/RageDisplay_OGL_Extensions.cpp src/RageDisplay_GLFW.cpp src/RageDisplay_Null.cpp src/RageException.cpp src/RageInput.cpp src/RageInputDevice.cpp src/RageLog.cpp src/RageMath.cpp src/RageModelGeometry.cpp src/RageSound.cpp src/RageSoundManager.cpp src/RageSoundUtil.cpp src/RageSoundMixBuffer.cpp src/RageSoundPosMap.cpp src/RageSoundReader_FileReader.cpp src/RageSoundReader_Preload.cpp src/RageSoundReader_Resample.cpp src/RageSoundReader_Resample_Fast.cpp src/RageSoundReader_Resample_Good.cpp src/RageSoundReader_Chain.cpp src/RageSoundResampler.cpp src/RageSurface.cpp src/RageSurfaceUtils.cpp src/RageSurfaceUtils_Dither.cpp src/RageSurface_Save_JPEG.cpp src/RageSurfaceUtils_Palettize.cpp src/RageSurfaceUtils_Zoom.cpp src/RageSurface_Load.cpp src/RageSurface_Load_PNG.cpp src/RageSurface_Load_JPEG.cpp src/RageSurface_Load_GIF.cpp src/RageSurface_Load_BMP.cpp src/RageSurface_Load_XPM.cpp src/RageTexture.cpp src/RageSurface_Save_BMP.cpp src/RageTextureID.cpp src/RageTextureManager.cpp src/RageThreads.cpp src/RageTimer.cpp src/RageUtil.cpp src/RageUtil_CharConversions.cpp src/RageUtil_BackgroundLoader.cpp src/RageUtil_FileDB.cpp src/RageUtil_WorkerThread.cpp src/Actor.cpp src/ActorFrame.cpp src/ActorScroller.cpp src/ActorUtil.cpp src/AutoActor.cpp src/BitmapText.cpp src/Model.cpp src/ModelManager.cpp src/ModelTypes.cpp src/Quad.cpp src/RollingNumbers.cpp src/Sprite.cpp src/AnnouncerManager.cpp src/Bookkeeper.cpp src/CryptManager.cpp src/FontManager.cpp src/GameSoundManager.cpp src/GameManager.cpp src/GameState.cpp src/InputFilter.cpp src/InputMapper.cpp src/InputQueue.cpp src/LuaManager.cpp src/LightsManager.cpp src/MemoryCardManager.cpp src/MessageManager.cpp src/NetworkSyncManager.cpp src/NetworkSyncServer.cpp src/NoteSkinManager.cpp src/PrefsManager.cpp src/ProfileManager.cpp src/ScreenManager.cpp src/SongManager.cpp src/StatsManager.cpp src/ThemeManager.cpp src/UnlockManager.cpp src/UserPackManager.cpp src/ProductInfo.cpp src/ibutton/owerr.c src/ibutton/ownetu.c src/ibutton/owllu.c src/ibutton/owsesu.c src/ibutton/owtrnu.c src/ibutton/crcutil.c src/ibutton/ds2480ut.c src/ibutton/linuxlnk.c src/ibutton/sha18.c src/ibutton/shaib.c src/iButton.cpp src/aes/aescrypt.c src/aes/aeskey.c src/aes/aestab.c src/crypto/CryptBn.cpp src/crypto/CryptMD5.cpp src/crypto/CryptNoise.cpp src/crypto/CryptPrime.cpp src/crypto/CryptRSA.cpp src/crypto/CryptRand.cpp src/crypto/CryptSH512.cpp src/crypto/CryptSHA.cpp src/CryptHelpers.cpp src/timer/Timer.cpp
)

add_library(lua STATIC
    src/lua-5.0/src/lopcodes.c
    src/lua-5.0/src/lstate.c
    src/lua-5.0/src/lstring.c
    src/lua-5.0/src/ldebug.c
    src/lua-5.0/src/lcode.c
    src/lua-5.0/src/ldo.c
    src/lua-5.0/src/lapi.c
    src/lua-5.0/src/lib/liolib.c
    src/lua-5.0/src/lib/lauxlib.c
    src/lua-5.0/src/lib/ltablib.c
    src/lua-5.0/src/lib/loadlib.c
    src/lua-5.0/src/lib/lstrlib.c
    src/lua-5.0/src/lib/lmathlib.c
    src/lua-5.0/src/lib/ldblib.c
    src/lua-5.0/src/lib/lbaselib.c
    src/lua-5.0/src/lib/LibLuaLib.vcxproj
    src/lua-5.0/src/ltm.c
    src/lua-5.0/src/lvm.c
    src/lua-5.0/src/lgc.c
    src/lua-5.0/src/llex.c
    src/lua-5.0/src/ltable.c
    src/lua-5.0/src/lparser.c
    src/lua-5.0/src/lmem.c
    src/lua-5.0/src/lundump.c
    src/lua-5.0/src/ldump.c
    src/lua-5.0/src/ltests.c
    src/lua-5.0/src/lzio.c
    src/lua-5.0/src/lfunc.c
    src/lua-5.0/src/lobject.c
)
target_include_directories(lua PUBLIC src/lua-5.0/include/)
target_link_options(lua PRIVATE "-pthread")

add_library(aes STATIC
    src/aes/aeskey.c
    src/aes/aestab.c
    src/aes/aescrypt.c
)
target_include_directories(lua PUBLIC src/aes/)

add_library(libtomcrypt STATIC
src/libtomcrypt/src/ciphers/aes/aes.c src/libtomcrypt/src/hashes/md5.c src/libtomcrypt/src/hashes/sha1.c src/libtomcrypt/src/hashes/helper/hash_memory.c src/libtomcrypt/src/math/fp/ltc_ecc_fp_mulmod.c src/libtomcrypt/src/math/ltm_desc.c src/libtomcrypt/src/math/multi.c src/libtomcrypt/src/math/rand_prime.c src/libtomcrypt/src/misc/base64/base64_decode.c src/libtomcrypt/src/misc/base64/base64_encode.c src/libtomcrypt/src/misc/burn_stack.c src/libtomcrypt/src/misc/crypt/crypt.c src/libtomcrypt/src/misc/crypt/crypt_argchk.c src/libtomcrypt/src/misc/crypt/crypt_cipher_descriptor.c src/libtomcrypt/src/misc/crypt/crypt_cipher_is_valid.c src/libtomcrypt/src/misc/crypt/crypt_find_cipher.c src/libtomcrypt/src/misc/crypt/crypt_find_cipher_any.c src/libtomcrypt/src/misc/crypt/crypt_find_cipher_id.c src/libtomcrypt/src/misc/crypt/crypt_find_hash.c src/libtomcrypt/src/misc/crypt/crypt_find_hash_any.c src/libtomcrypt/src/misc/crypt/crypt_find_hash_id.c src/libtomcrypt/src/misc/crypt/crypt_find_hash_oid.c src/libtomcrypt/src/misc/crypt/crypt_find_prng.c src/libtomcrypt/src/misc/crypt/crypt_fsa.c src/libtomcrypt/src/misc/crypt/crypt_hash_descriptor.c src/libtomcrypt/src/misc/crypt/crypt_hash_is_valid.c src/libtomcrypt/src/misc/crypt/crypt_ltc_mp_descriptor.c src/libtomcrypt/src/misc/crypt/crypt_prng_descriptor.c src/libtomcrypt/src/misc/crypt/crypt_prng_is_valid.c src/libtomcrypt/src/misc/crypt/crypt_register_cipher.c src/libtomcrypt/src/misc/crypt/crypt_register_hash.c src/libtomcrypt/src/misc/crypt/crypt_register_prng.c src/libtomcrypt/src/misc/crypt/crypt_unregister_cipher.c src/libtomcrypt/src/misc/crypt/crypt_unregister_hash.c src/libtomcrypt/src/misc/crypt/crypt_unregister_prng.c src/libtomcrypt/src/misc/error_to_string.c src/libtomcrypt/src/misc/pkcs5/pkcs_5_1.c src/libtomcrypt/src/misc/pkcs5/pkcs_5_2.c src/libtomcrypt/src/misc/zeromem.c src/libtomcrypt/src/modes/cbc/cbc_decrypt.c src/libtomcrypt/src/modes/cbc/cbc_done.c src/libtomcrypt/src/modes/cbc/cbc_encrypt.c src/libtomcrypt/src/modes/cbc/cbc_getiv.c src/libtomcrypt/src/modes/cbc/cbc_setiv.c src/libtomcrypt/src/modes/cbc/cbc_start.c src/libtomcrypt/src/modes/cfb/cfb_decrypt.c src/libtomcrypt/src/modes/cfb/cfb_done.c src/libtomcrypt/src/modes/cfb/cfb_encrypt.c src/libtomcrypt/src/modes/cfb/cfb_getiv.c src/libtomcrypt/src/modes/cfb/cfb_setiv.c src/libtomcrypt/src/modes/cfb/cfb_start.c src/libtomcrypt/src/modes/ctr/ctr_decrypt.c src/libtomcrypt/src/modes/ctr/ctr_done.c src/libtomcrypt/src/modes/ctr/ctr_encrypt.c src/libtomcrypt/src/modes/ctr/ctr_getiv.c src/libtomcrypt/src/modes/ctr/ctr_setiv.c src/libtomcrypt/src/modes/ctr/ctr_start.c src/libtomcrypt/src/modes/ctr/ctr_test.c src/libtomcrypt/src/modes/ecb/ecb_decrypt.c src/libtomcrypt/src/modes/ecb/ecb_done.c src/libtomcrypt/src/modes/ecb/ecb_encrypt.c src/libtomcrypt/src/modes/ecb/ecb_start.c src/libtomcrypt/src/modes/ofb/ofb_decrypt.c src/libtomcrypt/src/modes/ofb/ofb_done.c src/libtomcrypt/src/pk/asn1/der/bit/der_decode_bit_string.c src/libtomcrypt/src/pk/asn1/der/bit/der_encode_bit_string.c src/libtomcrypt/src/pk/asn1/der/bit/der_length_bit_string.c src/libtomcrypt/src/pk/asn1/der/boolean/der_decode_boolean.c src/libtomcrypt/src/pk/asn1/der/boolean/der_encode_boolean.c src/libtomcrypt/src/pk/asn1/der/boolean/der_length_boolean.c src/libtomcrypt/src/pk/asn1/der/choice/der_decode_choice.c src/libtomcrypt/src/pk/asn1/der/ia5/der_decode_ia5_string.c src/libtomcrypt/src/pk/asn1/der/ia5/der_encode_ia5_string.c src/libtomcrypt/src/pk/asn1/der/ia5/der_length_ia5_string.c src/libtomcrypt/src/pk/asn1/der/integer/der_decode_integer.c src/libtomcrypt/src/pk/asn1/der/integer/der_encode_integer.c src/libtomcrypt/src/pk/asn1/der/integer/der_length_integer.c src/libtomcrypt/src/pk/asn1/der/object_identifier/der_decode_object_identifier.c src/libtomcrypt/src/pk/asn1/der/object_identifier/der_encode_object_identifier.c src/libtomcrypt/src/pk/asn1/der/object_identifier/der_length_object_identifier.c src/libtomcrypt/src/pk/asn1/der/octet/der_decode_octet_string.c src/libtomcrypt/src/pk/asn1/der/octet/der_encode_octet_string.c src/libtomcrypt/src/pk/asn1/der/octet/der_length_octet_string.c src/libtomcrypt/src/pk/asn1/der/printable_string/der_decode_printable_string.c src/libtomcrypt/src/pk/asn1/der/printable_string/der_encode_printable_string.c src/libtomcrypt/src/pk/asn1/der/printable_string/der_length_printable_string.c src/libtomcrypt/src/pk/asn1/der/sequence/der_decode_sequence_ex.c src/libtomcrypt/src/pk/asn1/der/sequence/der_decode_sequence_flexi.c src/libtomcrypt/src/pk/asn1/der/sequence/der_decode_sequence_multi.c src/libtomcrypt/src/pk/asn1/der/sequence/der_encode_sequence_ex.c src/libtomcrypt/src/pk/asn1/der/sequence/der_encode_sequence_multi.c src/libtomcrypt/src/pk/asn1/der/sequence/der_length_sequence.c src/libtomcrypt/src/pk/asn1/der/sequence/der_sequence_free.c src/libtomcrypt/src/pk/asn1/der/set/der_encode_set.c src/libtomcrypt/src/pk/asn1/der/set/der_encode_setof.c src/libtomcrypt/src/pk/asn1/der/short_integer/der_decode_short_integer.c src/libtomcrypt/src/pk/asn1/der/short_integer/der_encode_short_integer.c src/libtomcrypt/src/pk/asn1/der/short_integer/der_length_short_integer.c src/libtomcrypt/src/pk/asn1/der/utctime/der_decode_utctime.c src/libtomcrypt/src/pk/asn1/der/utctime/der_encode_utctime.c src/libtomcrypt/src/pk/asn1/der/utctime/der_length_utctime.c src/libtomcrypt/src/pk/asn1/der/utf8/der_decode_utf8_string.c src/libtomcrypt/src/pk/asn1/der/utf8/der_encode_utf8_string.c src/libtomcrypt/src/pk/asn1/der/utf8/der_length_utf8_string.c src/libtomcrypt/src/pk/dsa/dsa_decrypt_key.c src/libtomcrypt/src/pk/dsa/dsa_encrypt_key.c src/libtomcrypt/src/pk/dsa/dsa_export.c src/libtomcrypt/src/pk/dsa/dsa_free.c src/libtomcrypt/src/pk/dsa/dsa_import.c src/libtomcrypt/src/pk/dsa/dsa_make_key.c src/libtomcrypt/src/pk/dsa/dsa_shared_secret.c src/libtomcrypt/src/pk/dsa/dsa_sign_hash.c src/libtomcrypt/src/pk/dsa/dsa_verify_hash.c src/libtomcrypt/src/pk/dsa/dsa_verify_key.c src/libtomcrypt/src/pk/pkcs1/pkcs_1_i2osp.c src/libtomcrypt/src/pk/pkcs1/pkcs_1_mgf1.c src/libtomcrypt/src/pk/pkcs1/pkcs_1_oaep_decode.c src/libtomcrypt/src/pk/pkcs1/pkcs_1_oaep_encode.c src/libtomcrypt/src/pk/pkcs1/pkcs_1_os2ip.c src/libtomcrypt/src/pk/pkcs1/pkcs_1_pss_decode.c src/libtomcrypt/src/pk/pkcs1/pkcs_1_pss_encode.c src/libtomcrypt/src/pk/pkcs1/pkcs_1_v1_5_decode.c src/libtomcrypt/src/pk/pkcs1/pkcs_1_v1_5_encode.c src/libtomcrypt/src/pk/rsa/rsa_decrypt_key.c src/libtomcrypt/src/pk/rsa/rsa_encrypt_key.c src/libtomcrypt/src/pk/rsa/rsa_export.c src/libtomcrypt/src/pk/rsa/rsa_exptmod.c src/libtomcrypt/src/pk/rsa/rsa_free.c src/libtomcrypt/src/pk/rsa/rsa_import.c src/libtomcrypt/src/pk/rsa/rsa_make_key.c src/libtomcrypt/src/pk/rsa/rsa_sign_hash.c src/libtomcrypt/src/pk/rsa/rsa_verify_hash.c src/libtomcrypt/src/prngs/fortuna.c src/libtomcrypt/src/prngs/rng_get_bytes.c src/libtomcrypt/src/prngs/rng_make_prng.c src/libtomcrypt/src/prngs/sprng.c src/libtomcrypt/src/prngs/yarrow.c
src/libtomcrypt/src/hashes/sha2/sha512.c
)
target_include_directories(libtomcrypt PUBLIC
    src/libtomcrypt/src/headers
    src/libtomcrypt/src
    src/libtommath
)
target_compile_definitions(libtomcrypt PRIVATE LTC_SOURCE)

add_library(madlib STATIC
    src/mad-0.15.1b/frame.c
    src/mad-0.15.1b/synth.c
    src/mad-0.15.1b/stream.c
    src/mad-0.15.1b/minimad.c
    src/mad-0.15.1b/huffman.c
    src/mad-0.15.1b/layer3.c
    src/mad-0.15.1b/version.c
    src/mad-0.15.1b/bit.c
    src/mad-0.15.1b/layer12.c
    src/mad-0.15.1b/fixed.c
    src/mad-0.15.1b/timer.c
    src/mad-0.15.1b/decoder.c
)
target_compile_definitions(madlib PUBLIC FPM_64BIT)
target_include_directories(madlib PUBLIC src/mad-0.15.1b)

add_library(tommath STATIC
src/libtommath/bncore.c  src/libtommath/bn_mp_init.c  src/libtommath/bn_mp_clear.c  src/libtommath/bn_mp_exch.c  src/libtommath/bn_mp_grow.c  src/libtommath/bn_mp_shrink.c  src/libtommath/bn_mp_clamp.c  src/libtommath/bn_mp_zero.c  src/libtommath/bn_mp_set.c  src/libtommath/bn_mp_set_int.c  src/libtommath/bn_mp_init_size.c  src/libtommath/bn_mp_copy.c  src/libtommath/bn_mp_init_copy.c  src/libtommath/bn_mp_abs.c  src/libtommath/bn_mp_neg.c  src/libtommath/bn_mp_cmp_mag.c  src/libtommath/bn_mp_cmp.c  src/libtommath/bn_mp_cmp_d.c  src/libtommath/bn_mp_rshd.c  src/libtommath/bn_mp_lshd.c  src/libtommath/bn_mp_mod_2d.c  src/libtommath/bn_mp_div_2d.c  src/libtommath/bn_mp_mul_2d.c  src/libtommath/bn_mp_div_2.c  src/libtommath/bn_mp_mul_2.c  src/libtommath/bn_s_mp_add.c  src/libtommath/bn_s_mp_sub.c  src/libtommath/bn_fast_s_mp_mul_digs.c  src/libtommath/bn_s_mp_mul_digs.c  src/libtommath/bn_fast_s_mp_mul_high_digs.c  src/libtommath/bn_s_mp_mul_high_digs.c  src/libtommath/bn_fast_s_mp_sqr.c  src/libtommath/bn_s_mp_sqr.c  src/libtommath/bn_mp_add.c  src/libtommath/bn_mp_sub.c  src/libtommath/bn_mp_karatsuba_mul.c  src/libtommath/bn_mp_mul.c  src/libtommath/bn_mp_karatsuba_sqr.c  src/libtommath/bn_mp_sqr.c  src/libtommath/bn_mp_div.c  src/libtommath/bn_mp_mod.c  src/libtommath/bn_mp_add_d.c  src/libtommath/bn_mp_sub_d.c  src/libtommath/bn_mp_mul_d.c  src/libtommath/bn_mp_div_d.c  src/libtommath/bn_mp_mod_d.c  src/libtommath/bn_mp_expt_d.c  src/libtommath/bn_mp_addmod.c  src/libtommath/bn_mp_submod.c  src/libtommath/bn_mp_mulmod.c  src/libtommath/bn_mp_sqrmod.c  src/libtommath/bn_mp_gcd.c  src/libtommath/bn_mp_lcm.c  src/libtommath/bn_fast_mp_invmod.c  src/libtommath/bn_mp_invmod.c  src/libtommath/bn_mp_reduce.c  src/libtommath/bn_mp_montgomery_setup.c  src/libtommath/bn_fast_mp_montgomery_reduce.c  src/libtommath/bn_mp_montgomery_reduce.c  src/libtommath/bn_mp_exptmod_fast.c  src/libtommath/bn_mp_exptmod.c  src/libtommath/bn_mp_2expt.c  src/libtommath/bn_mp_n_root.c  src/libtommath/bn_mp_jacobi.c  src/libtommath/bn_reverse.c  src/libtommath/bn_mp_count_bits.c  src/libtommath/bn_mp_read_unsigned_bin.c  src/libtommath/bn_mp_read_signed_bin.c  src/libtommath/bn_mp_to_unsigned_bin.c  src/libtommath/bn_mp_to_signed_bin.c  src/libtommath/bn_mp_unsigned_bin_size.c  src/libtommath/bn_mp_signed_bin_size.c  src/libtommath/bn_mp_xor.c  src/libtommath/bn_mp_and.c  src/libtommath/bn_mp_or.c  src/libtommath/bn_mp_rand.c  src/libtommath/bn_mp_montgomery_calc_normalization.c  src/libtommath/bn_mp_prime_is_divisible.c  src/libtommath/bn_prime_tab.c  src/libtommath/bn_mp_prime_fermat.c  src/libtommath/bn_mp_prime_miller_rabin.c  src/libtommath/bn_mp_prime_is_prime.c  src/libtommath/bn_mp_prime_next_prime.c  src/libtommath/bn_mp_dr_reduce.c  src/libtommath/bn_mp_dr_is_modulus.c  src/libtommath/bn_mp_dr_setup.c  src/libtommath/bn_mp_reduce_setup.c  src/libtommath/bn_mp_toom_mul.c  src/libtommath/bn_mp_toom_sqr.c  src/libtommath/bn_mp_div_3.c  src/libtommath/bn_s_mp_exptmod.c  src/libtommath/bn_mp_reduce_2k.c  src/libtommath/bn_mp_reduce_is_2k.c  src/libtommath/bn_mp_reduce_2k_setup.c  src/libtommath/bn_mp_reduce_2k_l.c  src/libtommath/bn_mp_reduce_is_2k_l.c  src/libtommath/bn_mp_reduce_2k_setup_l.c  src/libtommath/bn_mp_radix_smap.c  src/libtommath/bn_mp_read_radix.c  src/libtommath/bn_mp_toradix.c  src/libtommath/bn_mp_radix_size.c  src/libtommath/bn_mp_fread.c  src/libtommath/bn_mp_fwrite.c  src/libtommath/bn_mp_cnt_lsb.c  src/libtommath/bn_error.c  src/libtommath/bn_mp_init_multi.c  src/libtommath/bn_mp_clear_multi.c  src/libtommath/bn_mp_exteuclid.c  src/libtommath/bn_mp_toradix_n.c  src/libtommath/bn_mp_prime_random_ex.c  src/libtommath/bn_mp_get_int.c  src/libtommath/bn_mp_sqrt.c  src/libtommath/bn_mp_is_square.c  src/libtommath/bn_mp_init_set.c  src/libtommath/bn_mp_init_set_int.c  src/libtommath/bn_mp_invmod_slow.c  src/libtommath/bn_mp_prime_rabin_miller_trials.c  src/libtommath/bn_mp_to_signed_bin_n.c  src/libtommath/bn_mp_to_unsigned_bin_n.c
)
target_include_directories(tommath PUBLIC src/libtommath)

add_library(libresample STATIC
    src/libresample/src/resamplesubs.c
    src/libresample/src/resample.c
    src/libresample/src/hackedresamplesubs.c
    src/libresample/src/filterkit.c
)
target_include_directories(libresample PUBLIC src/libresample/include)

target_link_directories(WebITG BEFORE PUBLIC archive/)
target_link_libraries(WebITG PUBLIC
    #precompiled archives
    lua
    aes
    libtomcrypt
    madlib
    libresample
    tommath
    libavcodec.a
    libavdevice.a
    libavfilter.a
    libavformat.a
    libavutil.a
    libswresample.a
    libswscale.a
)

target_include_directories(WebITG BEFORE PUBLIC
    src/
    src/mad-0.15.1b
    src/lua-5.0/include/
    src/libtomcrypt/src/headers
    src/libresample/include
    extern/FFmpeg/include
    extern/FFmpeg/
)

include(CTest)
enable_testing()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

