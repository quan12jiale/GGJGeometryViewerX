##设置库名称
set(LIBRARY_TARGET_NAME GGJGeometryViewer)

##查找所有头文件
file(GLOB_RECURSE  ${LIBRARY_TARGET_NAME}_HEADER_FILES
    LIST_DIRECTORIES False 
    "${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}/include/*.h"
)
##设置VS筛选器，头文件分文件夹
source_group(
    TREE "${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}/include"
    PREFIX "Header Files"
    FILES ${${LIBRARY_TARGET_NAME}_HEADER_FILES}
)

##查找所有源文件
file(GLOB_RECURSE  ${LIBRARY_TARGET_NAME}_SRC_FILES
    LIST_DIRECTORIES False 
    "${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}/src/*.cpp"
)
##设置VS筛选器，源码分文件夹
source_group(
    TREE "${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}/src"
    PREFIX "Source Files"
    FILES ${${LIBRARY_TARGET_NAME}_SRC_FILES}
)


##设置生成目标
add_executable(${LIBRARY_TARGET_NAME}
    ${${LIBRARY_TARGET_NAME}_HEADER_FILES}
    ${${LIBRARY_TARGET_NAME}_SRC_FILES}
)


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES WIN32_EXECUTABLE ON)
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES LINK_FLAGS "/SAFESEH:NO /LARGEADDRESSAWARE")


##打开qt特性配置
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTOMOC ON)
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTOUIC ON)
set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES AUTORCC ON)

##设置预处理器定义
target_compile_definitions(${LIBRARY_TARGET_NAME} PRIVATE UNICODE WIN32 QT_DLL QT_NO_DEBUG NDEBUG QT_CORE_LIB QT_GUI_LIB QT_WIDGETS_LIB GDP_QT)

##配置构建/使用时的头文件路径
target_include_directories(
    ${LIBRARY_TARGET_NAME}
    PUBLIC   
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/${LIBRARY_TARGET_NAME}/include/>"

)

##配置库依赖
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(GBIM COMPONENTS Common GMath Geometry ViewCore ViewManager CGBase REQUIRED)


target_link_libraries(${LIBRARY_TARGET_NAME}
    PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets
	PRIVATE GBIM::Common GBIM::GMath GBIM::Geometry GBIM::ViewCore GBIM::ViewManager GBIM::CGBase
)
