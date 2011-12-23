# macro that defines the use of project folders
MACRO (GROUP_PROJECT_TO_FOLDER TargetProject FolderName)
    SET_TARGET_PROPERTIES(${TargetProject}
                          PROPERTIES
                          FOLDER "${FolderName}")
ENDMACRO (GROUP_PROJECT_TO_FOLDER)

# macro that sets up the demo projects
MACRO (ADD_DEMO_PROJECT PRJ_NAME SRC_FILES)
   # define all required policies
   CMAKE_POLICY(SET CMP0003 NEW)
   # add the project and files to the solution
   ADD_EXECUTABLE(${PRJ_NAME} ${SRC_FILES})
   # group these projects to the demo folder
   GROUP_PROJECT_TO_FOLDER(${PRJ_NAME} DEMO)
   # add the libraries to link to
   TARGET_LINK_LIBRARIES(${PRJ_NAME} ${FREETYPE_GL_LIB})
   TARGET_LINK_LIBRARIES(${PRJ_NAME} ${GLUT_glut_LIBRARY})
   TARGET_LINK_LIBRARIES(${PRJ_NAME} ${OPENGL_glu_LIBRARY})
   # atb requires the ant tweak bar library
   IF (${PRJ_NAME} STREQUAL "ATB")
      INCLUDE_DIRECTORIES(${ANT_TWEAK_BAR_INCLUDE_PATH})
      TARGET_LINK_LIBRARIES(${PRJ_NAME} ${ANT_TWEAK_BAR_LIBRARY})
   ENDIF (${PRJ_NAME} STREQUAL "ATB")
   # add compiler flags
   ADD_COMPILER_FLAGS(${PRJ_NAME} ${CXX_FLAGS})
   # add compiler definitions
   ADD_COMPILER_DEFINITIONS(${PRJ_NAME} FREEGLUT_LIB_PRAGMAS=0)
   # add linker flags
   ADD_LINKER_FLAGS(${PRJ_NAME} LINK_FLAGS_DEBUG /nodefaultlib:msvcrt)
ENDMACRO (ADD_DEMO_PROJECT)

# macro to make sure all required libs are found
MACRO (VALIDATE_LIBRARY LIB)
   IF (NOT ${LIB})
      MESSAGE(FATAL_ERROR "-- Could NOT find ${LIB}")
   ENDIF (NOT ${LIB})
ENDMACRO (VALIDATE_LIBRARY)

# macro to add compiler definitions
MACRO (ADD_COMPILER_DEFINITIONS TargetProject Definitions)
  SET_PROPERTY(TARGET ${TargetProject}
               APPEND PROPERTY
               COMPILE_DEFINITIONS ${Definitions})
ENDMACRO (ADD_COMPILER_DEFINITIONS)

# macro that allows compiler flags to be defined
MACRO (ADD_COMPILER_FLAGS TargetProject Flags)
  SET_PROPERTY(TARGET ${TargetProject}
               APPEND PROPERTY
               COMPILE_FLAGS " ${Flags}")
ENDMACRO (ADD_COMPILER_FLAGS)

# macro that allows linker flags to be defined
MACRO (ADD_LINKER_FLAGS TargetProject Flags)
   SET_PROPERTY(TARGET ${TargetProject}
                APPEND PROPERTY
                LINK_FLAGS " ${Flags}")
ENDMACRO (ADD_LINKER_FLAGS)

# macro that allows linker flags to be defined per configuration
MACRO (ADD_LINKER_FLAGS TargetProject TargetConfig Flags)
   SET_PROPERTY(TARGET ${TargetProject}
                APPEND PROPERTY
                ${TargetConfig} " ${Flags}")
ENDMACRO (ADD_LINKER_FLAGS)

# a macro that allows multiple lines to be concatenated
MACRO (MULTILINE_SET OutVar)
  FOREACH (STR ${ARGN})
    SET(${OutVar} "${${OutVar}}${STR}")
  ENDFOREACH(STR)
ENDMACRO (MULTILINE_SET)

##############################################################
##############################################################
##############################################################

# define the minimum cmake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.5 )

# just have debug and release configurations
SET(CMAKE_CONFIGURATION_TYPES
    Debug Release
    CACHE STRING
    "Overrides the available build modes for freetype-gl and freetype-gl based applications"
    FORCE)

# turn folder grouping on for the ide
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# adjust the cmake module path
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# define the project name...
SET(FREETYPE_GL_LIB FreeType-GL)
PROJECT(${FREETYPE_GL_LIB})

# bring in a few environment vars
# environment vars that should be defined
# for command-line CMake builds...
# GLEW_ROOT_DIR
# GLUT_ROOT_PATH
# FREETYPE_DIR
# ANT_TWEAK_BAR_ROOT
SET(GLEW_ROOT_DIR $ENV{GLEW_ROOT_DIR})
SET(GLUT_ROOT_PATH $ENV{GLUT_ROOT_PATH})

# find required packages
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)
FIND_PACKAGE(AntTweakBar REQUIRED)

# some of the packages do not force an error if
# not found... force an error so it can be fixed...
VALIDATE_LIBRARY(GLEW_LIBRARY)
VALIDATE_LIBRARY(FREETYPE_LIBRARY)
VALIDATE_LIBRARY(GLUT_glut_LIBRARY)
VALIDATE_LIBRARY(OPENGL_gl_LIBRARY)
VALIDATE_LIBRARY(OPENGL_glu_LIBRARY)
VALIDATE_LIBRARY(ANT_TWEAK_BAR_LIBRARY)

# define all the source files
SET(FREETYPE_GL_SRC
edtaa3func.c
font-manager.c
freetype-gl.c
markup.c
texture-atlas.c
texture-font.c
texture-glyph.c
vector.c
vertex-buffer.c

edtaa3func.h
font-manager.h
freetype-gl.h
markup.h
texture-atlas.h
texture-font.h
texture-glyph.h
vector.h
vertex-buffer.h)

# define the project as a static library
ADD_LIBRARY(${FREETYPE_GL_LIB} STATIC ${FREETYPE_GL_SRC})

# add the project's includes
INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GLEW_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_ft2build})
INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIR_freetype2})

# add compiler flags to the project
MULTILINE_SET(CXX_FLAGS
              # force the code to be compiled as C++ (/TP)
              # microsoft, get a C99 compliant compiler please...
              "/TP "
              # force the glew header to be preprocessed first
              "/FI${GLEW_INCLUDE_PATH}/GL/Glew.h ")
ADD_COMPILER_FLAGS(${FREETYPE_GL_LIB} ${CXX_FLAGS})

# add the project compiler defintions
ADD_COMPILER_DEFINITIONS(${FREETYPE_GL_LIB} FREEGLUT_LIB_PRAGMAS=0)
ADD_COMPILER_DEFINITIONS(${FREETYPE_GL_LIB} _CRT_SECURE_NO_WARNINGS)
ADD_COMPILER_DEFINITIONS(${FREETYPE_GL_LIB} _CRT_NONSTDC_NO_WARNINGS)

# add the project's external dependencies
TARGET_LINK_LIBRARIES(${FREETYPE_GL_LIB} ${GLEW_LIBRARY})
TARGET_LINK_LIBRARIES(${FREETYPE_GL_LIB} ${FREETYPE_LIBRARY})
TARGET_LINK_LIBRARIES(${FREETYPE_GL_LIB} ${OPENGL_gl_LIBRARY})

# add all the demo projects
ADD_DEMO_PROJECT(ATB             "demo-atb-agg.c;subpixel.vert;subpixel-gamma.frag")
ADD_DEMO_PROJECT(Benchmark       "demo-benchmark.c")
ADD_DEMO_PROJECT(Console         "demo-console.c")
ADD_DEMO_PROJECT(Cube            "demo-cube.c")
ADD_DEMO_PROJECT(DistanceField   "demo-distance-field.c;distance-field.frag;distance-field.vert")
ADD_DEMO_PROJECT(Font            "demo-font.c")
ADD_DEMO_PROJECT(Glyph           "demo-glyph.c")
ADD_DEMO_PROJECT(LCD             "demo-lcd.c")
ADD_DEMO_PROJECT(Line-AA         "demo-line-aa.c")
ADD_DEMO_PROJECT(MakeFont        "demo-makefont.c;arial-16.h")
ADD_DEMO_PROJECT(MakeFont2       "makefont.c")
ADD_DEMO_PROJECT(Markup          "demo-markup.c")
ADD_DEMO_PROJECT(Subpixel        "demo-subpixel.c;subpixel.frag;subpixel.vert")
ADD_DEMO_PROJECT(Texture         "demo-texture.c")
