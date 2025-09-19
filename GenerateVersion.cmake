find_package(Git)

if(GIT_EXECUTABLE)
  get_filename_component(WORKING_DIR ${SRC} DIRECTORY)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
    WORKING_DIRECTORY ${WORKING_DIR}
    OUTPUT_VARIABLE FOO_VERSION
    RESULT_VARIABLE ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} show -s --format=%cd --date=format:%b.%d.%Y..%H:%M:%S HEAD
    WORKING_DIRECTORY ${WORKING_DIR}
    OUTPUT_VARIABLE FOO_DATE
    RESULT_VARIABLE ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

if(FOO_VERSION STREQUAL "")
  set(FOO_VERSION 0.0.0-unknown)
  set(FOO_DATE 01.01.2001..00:00:00)
  message(WARNING "Failed to determine version from Git tags. Using default version \"${FOO_VERSION}\".")
endif()

configure_file(${SRC} ${DST} @ONLY)
