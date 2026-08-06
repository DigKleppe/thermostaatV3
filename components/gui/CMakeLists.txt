file(GLOB_RECURSE SOURCES *.c *.cpp)

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list *.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()
HEADER_DIRECTORIES(header_dir_list)
#set(COMPONENT_PRIV_REQUIRES  "esp_timer")

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS .
               	       REQUIRES lvgl esp_timer main)
               	      
                                  
