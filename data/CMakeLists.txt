function (generateTranslations sources)
    set(TS_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/AssetsOpt_de.ts
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/AssetsOpt_en.ts
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/AssetsOpt_fr.ts
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/AssetsOpt_ja.ts
        CACHE FILEPATH "List of translations files")

    message("Updating translations files")
    qt5_create_translation(QM_FILES ${TS_FILES} ${sources})

endfunction()

function (getResources list)
    SET(${list}
        ${CMAKE_CURRENT_SOURCE_DIR}/styles/Cathedral_Assets_Optimizer.rc
        ${CMAKE_CURRENT_SOURCE_DIR}/styles/qdarkstyle/style.qrc
        PARENT_SCOPE)
endfunction()

function (copyDataToBinaryDir target dir)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/profiles ${dir}/profiles)
endfunction()
