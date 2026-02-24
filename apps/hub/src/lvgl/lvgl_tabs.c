/**
 * @file lvgl_tabs.c
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief LVGL tabbed interface implementation - main GUI with Welcome, Temperature, Forecast, TODO, and Settings tabs.
 * @version 1.0
 * @date 2025-02-24
 * @copyright Copyright (c) 2025
 * @see https://github.com/jeroagullo
 */

#include <lvgl.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(lvgl_tabs, LOG_LEVEL_INF);

#include "lvgl_tabs.h"

bool temperature_tab_active = false;

/**
 * @brief Callback for tab change events.
 * 
 * Handles button presses using LV_EVENT_RELEASED (code 26) for tab buttons
 * and LV_EVENT_VALUE_CHANGED for swipe gestures.
 * 
 * @param e Pointer to the LVGL event.
 */
static void event_tabview_cb(lv_event_t *e) {
        lv_obj_t *tabview = lv_event_get_user_data(e);
        uint16_t tabIdx;
        
        tabIdx = lv_tabview_get_tab_active(tabview);
        LOG_INF("Current Active Tab : %d", tabIdx);

        /* Reset all tab active flags first */
        temperature_tab_active = false;

        switch(tabIdx){
                case TAB_WELCOME:
                        LOG_INF("Welcome tab");
                break;
                case TAB_TEMPERATURE:
                        LOG_INF("Temperature tab");
                        temperature_tab_active = true;
                break;
                case TAB_FORECAST:
                        LOG_INF("Forecast tab");
                break;
                case TAB_TODO:
                        LOG_INF("TODO tab");
                break;
                case TAB_SETTINGS:
                        LOG_INF("Settings tab");
                break;
        }
}

/**
 * @brief Create the Welcome tab content with logo and introduction text.
 * @param tab Pointer to the tab object.
 */
void create_tab_welcome(lv_obj_t* tab){

        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "Welcome!!!\n\n"
                        "It is the ZephyrHomeHub project\n"
                        "by jeroagullo\n"
                        "to demostrate the capabilities\n"
                        "of Zephyr RTOS.\n\n"
                        "Please, move around scrolling to the right\n"
                        "or touching the top tabs.\n\n"
                        "Enjoy your day :)");

        // Add image
        LV_IMAGE_DECLARE(Zephyr_RTOS_logo_2015_100pp_a8);
        lv_obj_t * img1 = lv_image_create(tab);
        lv_image_set_src(img1, &Zephyr_RTOS_logo_2015_100pp_a8);
        lv_obj_align(img1, LV_ALIGN_TOP_RIGHT, -20, 0);
}

/**
 * @brief Create the Temperature tab content with sensor node grid.
 * @param tab Pointer to the tab object.
 */
void create_tab_temperature(lv_obj_t* tab){

        // create top text
        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "This tab depicts the temperature and humidity from sensors");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        // TODO
        init_node_container(tab);
        
}

/**
 * @brief Create the Forecast tab content (weather forecast placeholder).
 * @param tab Pointer to the tab object.
 */
void create_tab_forecast(lv_obj_t* tab){

        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "This tab shows the weather forecast from Internet");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        // TODO
}

/**
 * @brief Create the TODO tab content (reserved for future features).
 * @param tab Pointer to the tab object.
 */
void create_tab_todo(lv_obj_t* tab){

        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "This tab is booked for something special");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        // TODO
}

/**
 * @brief Create the Settings tab content with network configuration panel.
 * @param tab Pointer to the tab object.
 */
void create_tab_settings(lv_obj_t* tab){
        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "You can check and modify the settings of device");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        create_settings_network_panel(tab);
}

/**
 * @brief Initialize the main GUI with tabview and all application tabs.
 * 
 * Creates the main tabview widget with 5 tabs: Welcome, Temperature,
 * Forecast, TODO, and Settings. Applies custom styles and registers
 * event callbacks for tab navigation.
 */
void init_gui(void)
{

        style_init();

        /*Create a Tab view object*/
        lv_obj_t * tabview;
        tabview = lv_tabview_create(lv_scr_act());
        lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
        lv_tabview_set_tab_bar_size(tabview, 30);

        lv_obj_add_style(tabview,  &style_tabview, 0);

        /*Add tabs (the tabs are page (lv_page) and can be scrolled*/
        lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Welcome");
        lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Temperature");
        lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Forecast");
        lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "TODO");
        lv_obj_t * tab5 = lv_tabview_add_tab(tabview, "Settings");

        /* Modern v9 API: get_tab_bar instead of get_tab_btns */
        lv_obj_t * tab_bar = lv_tabview_get_tab_bar(tabview);
        lv_obj_add_style(tab_bar, &style_tab, 0);

        /* 1. Register for VALUE_CHANGED on the tabview for swipes */
        lv_obj_add_event_cb(tabview, event_tabview_cb, LV_EVENT_VALUE_CHANGED, tabview);

        /* 2. Iterate through all individual tab buttons and attach the CLICKED event */
        uint32_t tab_count = lv_obj_get_child_count(tab_bar);
        for(uint32_t i = 0; i < tab_count; i++) {
                lv_obj_t * tab_btn = lv_obj_get_child(tab_bar, i);
                /* We use LV_EVENT_CLICKED for buttons to ensure it fires reliably */
                lv_obj_add_event_cb(tab_btn, event_tabview_cb, LV_EVENT_CLICKED, tabview);
        }

        create_tab_welcome(tab1);
        create_tab_temperature(tab2);
        create_tab_forecast(tab3);
        create_tab_todo(tab4);
        create_tab_settings(tab5);

}

