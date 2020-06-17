find_package(PythonInterp 3.4 REQUIRED)
include(FindPythonInterp)
add_custom_target(
    thcrap_repo 
    ALL 
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/utils/repo_update.py -f ${CMAKE_CURRENT_SOURCE_DIR} -t ${CMAKE_CURRENT_BINARY_DIR}
)