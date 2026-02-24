/**
 * @file lvgl_tabs.h
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief LVGL tabbed interface header - tab definitions and GUI function declarations.
 * @version 1.0
 * @date 2025-02-24
 * @copyright Copyright (c) 2025
 * @see https://github.com/jeroagullo
 */

#ifndef LVGL_TABS_H
#define LVGL_TABS_H

#include <lvgl.h>
#include "../utils.h"

#define TAB_WELCOME         0
#define TAB_TEMPERATURE     1
#define TAB_FORECAST        2
#define TAB_TODO            3
#define TAB_SETTINGS        4

extern bool temperature_tab_active;

// Style definitions
extern lv_style_t style_btn;
extern lv_style_t style_tabview;
extern lv_style_t style_tab;
extern lv_style_t style_error;

/**
 * @brief Initialize all LVGL styles used in the application.
 */
void style_init(void);

/**
 * @brief Initialize the main GUI with tabview and all tabs.
 */
void init_gui(void);

/**
 * @brief Create the Temperature tab content with sensor node grid.
 * @param tab Pointer to the tab object.
 */
void create_tab_temperature(lv_obj_t *tab);

/**
 * @brief Create LVGL widgets for a sensor node card.
 * @param node Pointer to the node info structure.
 */
void create_node_widgets(node_info_t *node);

/**
 * @brief Update the display of a sensor node with current data.
 * @param node Pointer to the node info structure.
 */
void update_node_display(node_info_t *node);

/**
 * @brief Create the network status panel in the settings tab.
 * @param tab Pointer to the settings tab object.
 */
void create_settings_network_panel(lv_obj_t *tab);

#endif /* LVGL_TABS_H */