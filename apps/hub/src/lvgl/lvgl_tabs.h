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

/* Style definitions */
extern lv_style_t style_btn;
extern lv_style_t style_tabview;
extern lv_style_t style_tab;
extern lv_style_t style_error;

void style_init(void);

void init_gui(void);

void init_node_container(lv_obj_t *tab);

void create_node_widgets(node_info_t *node);

void update_node_display(node_info_t *node);

#endif /* LVGL_TABS_H */