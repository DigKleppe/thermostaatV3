set(COMPONENT_SRCDIRS ".")
set(WAVESHARE_BSP_DIR ../managed_components//waveshare__esp32_s3_touch_lcd_4/include)
set(COMPONENT_ADD_INCLUDEDIRS "include" ${WAVESHARE_BSP_DIR} )

#set(COMPONENT_PRIV_REQUIRES  "esp_pm")
#set(COMPONENT_EMBED_TXTFILES "${project_dir}/server_certs/ca_cert.pem")
#set(COMPONENT_PRIV_REQUIRES  "driver esp_lcd esp_timer esp_mm touch ")
#set(COMPONENT_PRIV_REQUIRES  "lvgl esp_driver_ledc esp_driver_ppa esp_mm esp_lcd esp_timer touch lcd")

set(COMPONENT_EMBED_TXTFILES "${project_dir}/main/server_certs/ca_cert.pem")
register_component()

spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)

#set(LV_DEMO_DIR ../managed_components/lvgl__lvgl/demos)
#file(GLOB_RECURSE LV_DEMOS_SOURCES ${LV_DEMO_DIR}/*.c)
#set(EXTRA_REQUIRES)
#list(APPEND EXTRA_REQUIRES nvs_flash)

#idf_component_register(
#    SRCS main.c ${LV_DEMOS_SOURCES}
#    INCLUDE_DIRS . ${LV_DEMO_DIR}
#    REQUIRES ${EXTRA_REQUIRES})
idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_options(
    ${LVGL_LIB} 
    PRIVATE
        -Wno-error=attributes
        -DLV_LVGL_H_INCLUDE_SIMPLE
    
)
