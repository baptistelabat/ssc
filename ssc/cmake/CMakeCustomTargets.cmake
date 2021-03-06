# Add a target to generate API documentation with Doxygen
FIND_PACKAGE(Doxygen)
FIND_PACKAGE(Pandoc)

MESSAGE(STATUS "PANDOC : ${PANDOC}")
IF(PANDOC)
    MESSAGE(STATUS "Program PANDOC found -> Documentation will be generated")
ELSE()
    MESSAGE(STATUS "Program PANDOC NOT found -> Documentation will be uncomplete")
ENDIF()

IF(DOXYGEN_FOUND)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/DoxygenLayout.in ${CMAKE_CURRENT_BINARY_DIR}/DoxygenLayout.xml @ONLY)
    FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/doc")
    FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/doc/html")
    ADD_CUSTOM_TARGET(test_results
        COMMAND ./${TEST_EXE} --gtest_output=xml:test_output.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running all tests" VERBATIM
        DEPENDS ${TEST_EXE}
    )

    IF(PANDOC)
        ADD_CUSTOM_TARGET(doc_dev_guide
            ${PANDOC} overview.md -o html/overview.html
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
            COMMENT "Generating the developper guide" VERBATIM
        )
    ELSE()
        MESSAGE(STATUS "Program PANDOC not found -> Developer documentation will be uncomplete")
    ENDIF()

    ADD_CUSTOM_TARGET(doc_dev
                      ${DOXYGEN_EXECUTABLE} Doxyfile
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating API documentation with Doxygen" VERBATIM
                     )
    IF(PANDOC)
        ADD_DEPENDENCIES(doc_dev doc_dev_guide)
    ENDIF()

    FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/images_for_documentation/*.svg")
    FOREACH(f ${files})
        FILE(COPY ${f} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/doc/html)
    ENDFOREACH()

    FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/images_for_documentation/*.png")
    FOREACH(f ${files})
        FILE(COPY ${f} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/doc/latex)
    ENDFOREACH()

    FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.js")
    FOREACH(f ${files})
        FILE(COPY ${f} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/doc/html)
    ENDFOREACH()

    FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.css")
    FOREACH(f ${files})
        FILE(COPY ${f} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/doc/html)
    ENDFOREACH()
ELSE()
    MESSAGE("Doxygen not found.")
ENDIF()

ADD_CUSTOM_TARGET(
    sloccount
    sloccount `find . -maxdepth 1 -type d ! \\\( -name \".\" -o -name \"tools_ThirdParty\" -o -name \"doc\" -o -name \"data\" -o -name \"*bin*\" -o -name \".svn\" -o -name \".git\" -o -name \".settings\" \\\)`> "${PROJECT_BINARY_DIR}/sloccount.txt"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Evaluates the amount of work of the project"
)

ADD_CUSTOM_TARGET(
    md5sum
    COMMAND find . -type f \\\( -name \"*.c\" -o -name \"*.h\" \\\) -print0 | xargs -0 md5sum > "${PROJECT_BINARY_DIR}/md5sum.txt"
    COMMAND find . -type f \\\( -name \"*.cpp\" -o -name \"*.hpp\" \\\) -print0 | xargs -0 md5sum >> "${PROJECT_BINARY_DIR}/md5sum.txt"
    COMMAND find . -type f \\\( -name \"*.cxx\" -o -name \"*.hxx\" \\\) -print0 | xargs -0 md5sum >> "${PROJECT_BINARY_DIR}/md5sum.txt"
    COMMAND find . -type f -name \"*.sh\" -print0 | xargs -0 md5sum >> "${PROJECT_BINARY_DIR}/md5sum.txt"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Create a file named md5sum.txt that contains the md5 hash of all source files"
)

ADD_CUSTOM_TARGET(
    archive
    COMMAND export dateSTR=`date +'%Y_%m_%d__%H_%M'` &&
            export pkg0=`echo $$PWD` &&
            export pkg=`echo \$\${PWD\#\#*/}` &&
            export tarName=`echo \$\${pkg}__\$\${dateSTR}` &&
            export dirToSave=`find . -maxdepth 1 ! \\\( -name \".\" -o -name \$\${pkg}__*.tar.gz -o -name \$\${pkg}__*.tgz -o -name \"Results*\" -o -name \"data\" -o -name \"bin\" -o -regex \"bin*\" -o -name \"build\" \\\) | cut -f2 -d"/"` &&
            tar -czf \$\${tarName}.tgz \$\${dirToSave}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Create an archive of the project"
)
