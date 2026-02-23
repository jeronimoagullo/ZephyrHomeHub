#include <lvgl.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(lvgl_tabs, LOG_LEVEL_INF);

#include "lvgl_tabs.h"

bool temperature_tab_active = false;

// NOTE: there is a bug in which the tabview changes when you press the button, so the returning value 
// of lv_tabview_get_tab_act is a weird number, working only by "sliding" instead of "clicking"
// it is solved using the tabview as user data parameter
static void event_tabview_cb(lv_event_t *e){

        lv_obj_t * tabview = lv_event_get_user_data(e);
        uint16_t tabIdx = lv_tabview_get_tab_act(tabview);

        LOG_INF("Current Active Tab : %d", tabIdx);

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
        LV_IMG_DECLARE(Zephyr_RTOS_logo_2015_100pp_a8);
        lv_obj_t * img1 = lv_img_create(tab);
        lv_img_set_src(img1, &Zephyr_RTOS_logo_2015_100pp_a8);
        lv_obj_align(img1, LV_ALIGN_TOP_RIGHT, -20, 0);
}
/**
 * @brief Create a tab2 object
 *              This tab shows the sliders and buttons (with mqtt)
 * 
 * @param tab 
 */

void create_tab_temperature(lv_obj_t* tab){

        // create top text
        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "This tab depicts the temperature and humidity from sensors");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        // TODO
        init_node_container(tab);
        
}

void create_tab_forecast(lv_obj_t* tab){

        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "This tab shows the weather forecast from Internet");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        // TODO
}

void create_tab_todo(lv_obj_t* tab){

        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "This tab is booked for something special");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        // TODO
}

/**
 * @brief Create a tab5 object
 *              Config tag to
 * 
 * @param tab 
 */
void create_tab_settings(lv_obj_t* tab){
        lv_obj_t * label = lv_label_create(tab);
        lv_label_set_text(label, "You can check and modify the settings of device");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        // TODO
}


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

        lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
        lv_obj_add_style(tab_btns, &style_tab, 0);

        /* Add callbacks for each button of the tabs and for swipe handling */
        lv_obj_add_event_cb(tab_btns, event_tabview_cb, LV_EVENT_CLICKED, tabview);
        lv_obj_add_event_cb(tabview, event_tabview_cb, LV_EVENT_VALUE_CHANGED, tabview);

        create_tab_welcome(tab1);
        create_tab_temperature(tab2);
        create_tab_forecast(tab3);
        create_tab_todo(tab4);
        create_tab_settings(tab5);

}

