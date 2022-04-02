cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(spreadsheet)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
  set(SYSTEM_LIBS -lstdc++)
else()
  set(SYSTEM_LIBS)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}/JMC")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-implicit-fallthrough")
endif()

set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/ANTLR4/antlr-4.9.3-complete.jar)
include(${CMAKE_CURRENT_SOURCE_DIR}/FindANTLR.cmake)

add_definitions(-DANTLR4CPP_STATIC -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

set(WITH_STATIC_CRT OFF CACHE BOOL "Visual C++ static CRT for ANTLR" FORCE)

add_subdirectory(ANTLR4/antlr4_runtime)

antlr_target(FormulaParser Formula.g4 LEXER PARSER LISTENER)

include_directories(${ANTLR4_INCLUDE_DIRS} 
                    ${ANTLR_FormulaParser_OUTPUT_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/ANTLR4/antlr4_runtime/runtime/src)

set(INCLUDE_DIR inc)
set(SOURCE_DIR src)

set(SPREADSHEET_FILES "${INCLUDE_DIR}/cell.h"
                      "${INCLUDE_DIR}/sheet.h"
                      "${INCLUDE_DIR}/common.h"
                      "${INCLUDE_DIR}/formula.h"
                      "${INCLUDE_DIR}/FormulaAST.h"
                      "${SOURCE_DIR}/cell.cpp"
                      "${SOURCE_DIR}/sheet.cpp"
                      "${SOURCE_DIR}/structures.cpp"
                      "${SOURCE_DIR}/formula.cpp"
                      "${SOURCE_DIR}/FormulaAST.cpp")
set(TESTS_FILES "${SOURCE_DIR}/tests.cpp"
                "${INCLUDE_DIR}/tests.h"
                "${INCLUDE_DIR}/test_runner_p.h")
set(MAIN_FILES "${SOURCE_DIR}/main.cpp")

source_group("Main" FILES ${MAIN_FILES})
source_group("Tests" FILES ${TESTS_FILES})
source_group("Spreadsheet" FILES ${SPREADSHEET_FILES})

add_executable(spreadsheet ${ANTLR_FormulaParser_CXX_OUTPUTS} ${MAIN_FILES} ${TESTS_FILES} ${SPREADSHEET_FILES})

target_link_libraries(spreadsheet antlr4_static ${SYSTEM_LIBS})

install(
  TARGETS spreadsheet
  DESTINATION bin
  EXPORT spreadsheet
)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT spreadsheet)