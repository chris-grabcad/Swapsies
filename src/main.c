#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static BitmapLayer *s_background_layer;

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  bool use24h = clock_is_24h_style();
  static char buffer[] = "00->00";
  static char xfer[] = "00";

  strftime(xfer, sizeof("00"), use24h?"%H":"%I", tick_time);
  int hours = atoi(xfer);
  strftime(xfer, sizeof("00"), "%M", tick_time);
  int minutes = atoi(xfer);

  if (minutes > 30) {
    hours += 1;
    minutes = 60 - minutes;

    if (use24h) {
      if (hours > 23) {
        hours = 0;
      }
    } else {
      if (hours > 12) {
        hours = 1;
      }
    }

    snprintf(buffer, sizeof("00->00"), "%02d:%02d", minutes, hours);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    bitmap_layer_set_background_color(s_background_layer, GColorBlack);
  } else {
    snprintf(buffer, sizeof("00->00"), "%02d:%02d", hours, minutes);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    bitmap_layer_set_background_color(s_background_layer, GColorWhite);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);  
}

static void main_window_load(Window *window) {
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  update_time();

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
   update_time();
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) 
                             { 
                               .load = main_window_load, 
                               .unload = main_window_unload
                             });
  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}