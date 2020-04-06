PROJECT(openvibe-plugins-contrib-pybox)

SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

FILE(GLOB_RECURSE source_files src/*.cpp src/*.hpp  src/*.h src/*.inl)
ADD_LIBRARY(${PROJECT_NAME} SHARED ${source_files})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	COMPILE_FLAGS "-DOVP_Exports -DOVP_Shared")

INCLUDE_DIRECTORIES("src")

# ---------------------------------
# OpenVibe Modules (uncomment usefull package)
# ---------------------------------
# OpenViBE Base
INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEToolkit")

# OpenViBE Module
INCLUDE("FindOpenViBEModuleEBML")
INCLUDE("FindOpenViBEModuleSystem")

# OpenViBE Third Party
INCLUDE("FindThirdPartyBoost")
INCLUDE("FindThirdPartyPython")

# ---------------------------------
# Target macros
# Defines target operating system
# Defines target architecture
# Defines target compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# -----------------------------
# Install files
# -----------------------------
INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DIST_BINDIR}
	LIBRARY DESTINATION ${DIST_LIBDIR}
	ARCHIVE DESTINATION ${DIST_LIBDIR})

INSTALL(DIRECTORY box-tutorials DESTINATION ${DIST_DATADIR}/openvibe/scenarios/)
INSTALL(DIRECTORY share/ DESTINATION ${DIST_DATADIR}/openvibe/plugins/python)
#INSTALL(DIRECTORY share/ DESTINATION ${DIST_DATADIR}/openvibe/plugins/python3) # For 3.0.0 Version

# ---------------------------------
# Test applications (uncomment to enable your test directory)
# ---------------------------------
#IF(OV_COMPILE_TESTS)
#	ADD_SUBDIRECTORY(test)
#ENDIF(OV_COMPILE_TESTS)

