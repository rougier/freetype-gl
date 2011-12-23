#
# Try to find AntTweakBar library and include path.
# Once done this will define
#
# ANT_TWEAK_BAR_FOUND
# ANT_TWEAK_BAR_INCLUDE_PATH
# ANT_TWEAK_BAR_LIBRARY
# 

IF (WIN32)
	FIND_PATH( ANT_TWEAK_BAR_INCLUDE_PATH AntTweakBar.h
      PATHS
		$ENV{ANT_TWEAK_BAR_ROOT}/include
		DOC "The directory where AntTweakBar.h resides")

    FIND_LIBRARY( ANT_TWEAK_BAR_LIBRARY AntTweakBar
        PATHS
        $ENV{ANT_TWEAK_BAR_ROOT}/lib
        DOC "The AntTweakBar library")
ELSE (WIN32)
	# need to define for this platform
ENDIF (WIN32)

SET(ANT_TWEAK_BAR_FOUND "NO")
IF (ANT_TWEAK_BAR_INCLUDE_PATH AND ANT_TWEAK_BAR_LIBRARY)
	SET(ANT_TWEAK_BAR_FOUND "YES")
ENDIF (ANT_TWEAK_BAR_INCLUDE_PATH AND ANT_TWEAK_BAR_LIBRARY)

INCLUDE(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AntTweakBar DEFAULT_MSG ANT_TWEAK_BAR_INCLUDE_PATH ANT_TWEAK_BAR_LIBRARY)
