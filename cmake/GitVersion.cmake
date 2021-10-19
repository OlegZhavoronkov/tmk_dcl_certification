#
# Extract GIT version to be included into any application that wants to know it
#
function(add_version TARGET DEFINE_NAME)
  execute_process(COMMAND git describe --tags
                  OUTPUT_VARIABLE GIT_INFO_COMMIT
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  )
  if(NOT GIT_INFO_COMMIT)
    execute_process(COMMAND git rev-list HEAD --count
                    OUTPUT_VARIABLE GIT_COMMIT_NUMBER
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    )
    set(GIT_INFO_COMMIT 0.0-${GIT_COMMIT_NUMBER})
  endif()

  execute_process(COMMAND git rev-parse --abbrev-ref HEAD
                  OUTPUT_VARIABLE GIT_INFO_BRANCH
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  )

  string(REPLACE "\n" "" GIT_INFO_COMMIT ${GIT_INFO_COMMIT})
  string(REPLACE "\n" "" GIT_INFO_BRANCH ${GIT_INFO_BRANCH})

  string(TIMESTAMP BUILD_DATA "%y.%m.%d %H:%M")

  set(VERSION "${GIT_INFO_COMMIT} ${GIT_INFO_BRANCH} ${BUILD_DATA}")
  string(REGEX REPLACE "-[a-z0-9-]*" "" TAG_ONLY ${GIT_INFO_COMMIT})

  get_target_property(TARGET_TYPE ${TARGET} TYPE)
  if(NOT ${TARGET_TYPE} STREQUAL "UTILITY")
    target_compile_definitions(${TARGET}
                               PRIVATE
                               ${DEFINE_NAME}="${VERSION}"
                               ${DEFINE_NAME}_TAG="${TAG_ONLY}"
                               )
  else()
    set(${DEFINE_NAME} ${VERSION} PARENT_SCOPE)
    set(${DEFINE_NAME}_TAG ${TAG_ONLY} PARENT_SCOPE)
  endif()

endfunction(add_version)
