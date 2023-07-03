function(generateTranslations sources qm_files)
  set(TS_FILES
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_de.ts
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_en.ts
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_es.ts
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_fr_FR.ts
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_ja.ts
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_ko_KR.ts
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_nl.ts
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_ru.ts
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_tr.ts
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/translations/AssetsOpt_zh_CN.ts
      CACHE FILEPATH "List of translations files")

  message("Updating translations files")
  qt5_create_translation(QM_FILES ${TS_FILES} ${sources})

  set(qm_files
      ${QM_FILES}
      PARENT_SCOPE)
endfunction()

function(getResources list)
  # AUTORCC does not seem to work here
  qt5_add_resources(
    cao_qrc ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/styles/qdarkstyle/style.qrc)

  set(${list}
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/styles/Cathedral_Assets_Optimizer.rc
      ${cao_qrc}
      PARENT_SCOPE)
endfunction()

function(getIcon list)
  set(${list}
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/styles/Cathedral_Assets_Optimizer.rc
      PARENT_SCOPE)
endfunction()

function(copyDataToBinaryDir target dir)
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/profiles ${dir}/profiles)
endfunction()
