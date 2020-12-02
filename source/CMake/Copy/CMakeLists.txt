##设置库名称
set(LIBRARY_TARGET_NAME Copy)

##设置生成目标
add_library(${LIBRARY_TARGET_NAME} )

##打开qt特性配置
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTOMOC ON)
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTOUIC ON)
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTORCC ON)

set(QtPath "${QT_PREFIX_PATH}")
set(GTJPath "${GTJ_BULID_DIR}/bin/${PROJECT_PLATFORM}/$(Configuration)")
set(DestPath "${PROJECT_SOURCE_DIR}/../build/bin/${PROJECT_PLATFORM}/$<CONFIG>")

set(ResPath "${PROJECT_SOURCE_DIR}/../resource")

add_custom_command(TARGET ${LIBRARY_TARGET_NAME} POST_BUILD
	COMMAND cmd /c ${PROJECT_SOURCE_DIR}/Copy/CopyDll.bat "${QtPath}" "${GTJPath}" "${DestPath}"
	COMMAND cmd /c ${PROJECT_SOURCE_DIR}/Copy/CopyRes.bat "${ResPath}" "${DestPath}"
)
