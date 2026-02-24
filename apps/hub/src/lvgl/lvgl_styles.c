/**
 * @file lvgl_styles.c
 * @author Jeronimo Agullo (jeronimoagullo97@gmail.com)
 * @brief LVGL style definitions and color palette for the application UI.
 * @version 1.0
 * @date 2025-02-24
 * @copyright Copyright (c) 2025
 * @see https://github.com/jeroagullo
 */

#include <zephyr/kernel.h>
#include <lvgl.h>
#include "lvgl_tabs.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(jeroagullo_styles, LOG_LEVEL_INF);

/* Mutex for synchronization with lv_task_handler() calls */
K_MUTEX_DEFINE(lvgl_mutex);

/* Styles */
lv_style_t style_btn;
lv_style_t style_btn2;
lv_style_t style_btn3;
lv_style_t style_tabview;
lv_style_t style_tab;
lv_style_t style_error;

/* Custom color palette */
lv_color_t navy_color;
lv_color_t cyan_color;
lv_color_t honey_color;
lv_color_t kiwi_color;
lv_color_t sky_color;
lv_color_t gum_color;
lv_color_t tiger_color;

/* Grey scale colors */
lv_color_t d10_color;
lv_color_t l10_color;
lv_color_t l20_color;
lv_color_t l30_color;
lv_color_t l40_color;
lv_color_t l50_color;

/**
 * @brief Initialize custom color palette from hex values.
 */
void create_lvgl_colors(){
        navy_color = lv_color_hex(0x061B2B);
        cyan_color = lv_color_hex(0x00C1DE);
        honey_color = lv_color_hex(0xF7B334);
        kiwi_color = lv_color_hex(0x85BD41);
        sky_color = lv_color_hex(0x84D4F7);
        gum_color = lv_color_hex(0xDA1684);
        tiger_color = lv_color_hex(0xF5801F);

        /* Grey scale colors */
        d10_color = lv_color_hex(0x000712);
        l10_color = lv_color_hex(0x404B5C);
        l20_color = lv_color_hex(0x7F8691);
        l30_color = lv_color_hex(0xBFC3C8);
        l40_color = lv_color_hex(0xDEE1E2);
        l50_color = lv_color_hex(0xF7F8F8);
}

/**
 * @brief Create button style with navy background and cyan border.
 */
void create_style_btn(){
        lv_style_init(&style_btn);
        lv_style_set_radius(&style_btn, 10);
        lv_style_set_bg_opa(&style_btn, LV_OPA_COVER); 
        lv_style_set_bg_color(&style_btn, navy_color);

        lv_style_set_border_color(&style_btn, cyan_color);
        lv_style_set_border_opa(&style_btn, LV_OPA_40);
        lv_style_set_border_width(&style_btn, 2);

        lv_style_set_text_color(&style_btn, lv_color_white());
}

/**
 * @brief Create tab button style with navy background.
 */
void create_style_tab(){
        lv_style_init(&style_tab);
        lv_style_set_bg_opa(&style_tab, LV_OPA_80);
        lv_style_set_bg_color(&style_tab, navy_color);
        lv_style_set_text_color(&style_tab, lv_color_white());
        lv_style_set_border_side(&style_tab, LV_BORDER_SIDE_RIGHT);
}

/**
 * @brief Create tabview container style with gradient background.
 */
void create_style_tabview(){
        lv_style_init(&style_tabview);
        lv_style_set_bg_opa(&style_tabview, LV_OPA_30);
        lv_style_set_bg_color(&style_tabview, cyan_color);
        lv_style_set_bg_grad_color(&style_tabview, lv_palette_main(LV_PALETTE_GREY));
        lv_style_set_bg_grad_dir(&style_tabview, LV_GRAD_DIR_VER);

        lv_style_set_text_color(&style_tabview, lv_color_black());

        /* Create tab button style */
        create_style_tab();
}

/**
 * @brief Create error text style with red color.
 */
void create_style_error(){
        lv_style_init(&style_error);
        lv_style_set_text_color(&style_error, lv_palette_main(LV_PALETTE_RED));
}

/**
 * @brief Initialize all LVGL styles used in the application.
 * 
 * Must be called once at startup before creating any styled widgets.
 */
void style_init(void)
{
        /* Create custom color palette */
        create_lvgl_colors();

        /* Create button style */
        create_style_btn();

        /* Create tabview style */
        create_style_tabview();

        /* Create error style */
        create_style_error();
}