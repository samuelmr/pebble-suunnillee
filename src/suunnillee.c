#include "pebble.h"

#define BUFFER_SIZE 20

// aehijklmnoprstuvyäö

Window *window;
char top_string[BUFFER_SIZE];
char middle_string[BUFFER_SIZE];
char bottom_string[BUFFER_SIZE];  

GFont regular36;
GFont light36;
GFont bold36;

static struct TopStruct {
  TextLayer *label;
  char buffer[BUFFER_SIZE];
} top_line;

static struct MiddleStuct {
  TextLayer *label;
  char buffer[BUFFER_SIZE];
} middle_line;

static struct BottomStruct {
  TextLayer *label;
  char buffer[BUFFER_SIZE];
} bottom_line;

static const char* const LITTLE[] = {
  "vähän",
  "pikkusen"
};

static const char* const ALMOST[] = {
  "melkeen",
  "kohta"
};

static const char* STR_QUARTER = "varttii";
static const char* STR_TEN = "kymppii";
static const char* STR_TWENTY = "kakskyt";

static const char* STR_TO = "vaille";
static const char* STR_PAST = "yli";

static const char* STR_HALF = "puol";

static const char* const HOURS[] = {
  "",
  "yks",
  "kaks",
  "kolme",
  "neljä",
  "viis",
  "kuus",
  "seittemä",
  "kaheksa",
  "yheksä",
  "kymppi",
  "ykstoist",
};

static const char* STR_NOON = "kakstoist";
static const char* STR_MIDNIGHT = "keskiyö";

static size_t append_number(char* words, int num) {
  strcat(words, HOURS[num]);
  return strlen(HOURS[num]);
}

static size_t append_string(char* buffer, const size_t length, const char* str) {
  strncat(buffer, str, length);
  size_t written = strlen(str);
  return (length > written) ? written : length;
}


static void update_time(struct tm* t) {
  int fuzzy_hours = t->tm_hour;
  int fuzzy_minutes = ((t->tm_min + 2) / 5) * 5;

  if (fuzzy_minutes > 55) {
    fuzzy_minutes = 0;
    fuzzy_hours += 1;
    if (fuzzy_hours > 23) {
      fuzzy_hours = 0;
    }
  }
  char *top = top_string;
  char *middle = middle_string;
  char *bottom = bottom_string;

  size_t top_remaining = BUFFER_SIZE;
  size_t middle_remaining = BUFFER_SIZE;
  size_t bottom_remaining = BUFFER_SIZE;

  memset(top, 0, BUFFER_SIZE);
  memset(middle, 0, BUFFER_SIZE);
  memset(bottom, 0, BUFFER_SIZE);
  
  GFont top_font = regular36;
  GFont middle_font = light36;
  GFont bottom_font = bold36;

  int fuzzy_index = 0;
  if (fuzzy_hours % 2) {
    fuzzy_index = 1;
  }

  if (fuzzy_minutes != 0 && (fuzzy_minutes >= 10 || fuzzy_minutes == 5 || fuzzy_hours == 0 || fuzzy_hours == 12)) {
    if (fuzzy_minutes == 55) {
      middle_font = regular36;
      middle_remaining -= append_string(middle, middle_remaining, ALMOST[fuzzy_index]);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes == 50) {
      top_remaining -= append_string(top, top_remaining, STR_TEN);
      middle_remaining -= append_string(middle, middle_remaining, STR_TO);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes == 45) {
      top_remaining -= append_string(top, top_remaining, STR_QUARTER);
      middle_remaining -= append_string(middle, middle_remaining, STR_TO);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes == 40) {
      top_remaining -= append_string(top, top_remaining, STR_TWENTY);
      middle_remaining -= append_string(middle, middle_remaining, STR_TO);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes == 35) {
      top_font = light36;
      middle_font = regular36;
      top_remaining -= append_string(top, top_remaining, LITTLE[fuzzy_index]);
      middle_remaining -= append_string(middle, middle_remaining, STR_PAST);
      middle_remaining -= append_string(middle, middle_remaining, " ");
      middle_remaining -= append_string(middle, middle_remaining, STR_HALF);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes == 30) {
      middle_remaining -= append_string(middle, middle_remaining, STR_HALF);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes == 25) {
      top_font = light36;
      middle_font = regular36;
      top_remaining -= append_string(top, top_remaining, ALMOST[fuzzy_index]);
      middle_remaining -= append_string(middle, middle_remaining, STR_HALF);
      fuzzy_hours = (fuzzy_hours + 1) % 24;
    } else if (fuzzy_minutes == 20) {
      top_remaining -= append_string(top, top_remaining, STR_TWENTY);
      middle_remaining -= append_string(middle, middle_remaining, STR_PAST);
    } else if (fuzzy_minutes == 15) {
      top_remaining -= append_string(top, top_remaining, STR_QUARTER);
      middle_remaining -= append_string(middle, middle_remaining, STR_PAST);
    } else if (fuzzy_minutes == 10) {
      top_remaining -= append_string(top, top_remaining, STR_TEN);
      middle_remaining -= append_string(middle, middle_remaining, STR_PAST);
    } else if (fuzzy_minutes == 5) {
      top_remaining -= append_string(top, top_remaining, LITTLE[fuzzy_index]);
      middle_remaining -= append_string(middle, middle_remaining, STR_PAST);
    }
  }
  if (fuzzy_hours == 0) {
    bottom_remaining -= append_string(bottom, bottom_remaining, STR_MIDNIGHT);
  } else if (fuzzy_hours == 12) {
    bottom_remaining -= append_string(bottom, bottom_remaining, STR_NOON);
    bottom_font = regular36;
  } else {
    if ((fuzzy_hours % 12 == 7) || (fuzzy_hours % 12 == 8)) {
      // APP_LOG(APP_LOG_LEVEL_DEBUG, "%d, %d", fuzzy_hours, fuzzy_hours % 12);  
      bottom_font = regular36;
    }
    bottom_remaining -= append_number(bottom, fuzzy_hours % 12);
  }
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "%s, %s, %s", top, middle, bottom);  
  text_layer_set_font(top_line.label, top_font);
  text_layer_set_text(top_line.label, top);
  text_layer_set_font(middle_line.label, middle_font);
  text_layer_set_text(middle_line.label, middle);
  text_layer_set_font(bottom_line.label, bottom_font);
  text_layer_set_text(bottom_line.label, bottom);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void do_init(void) {
  window = window_create();
  const bool animated = true;
  window_stack_push(window, animated);

  window_set_background_color(window, GColorBlack);

  regular36 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BEATLES_REGULAR_36));
  light36 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BEATLES_LIGHT_36));
  bold36 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BEATLES_BOLD_36));
  
  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

  top_line.label = text_layer_create(GRect(0, 20, frame.size.w, 40));
  text_layer_set_background_color(top_line.label, GColorBlack);
  text_layer_set_text_color(top_line.label, GColorWhite);
  layer_add_child(root_layer, text_layer_get_layer(top_line.label));

  middle_line.label = text_layer_create(GRect(0, 60, frame.size.w, 40));
  text_layer_set_background_color(middle_line.label, GColorBlack);
  text_layer_set_text_color(middle_line.label, GColorWhite);
  layer_add_child(root_layer, text_layer_get_layer(middle_line.label));

  bottom_line.label = text_layer_create(GRect(0, 100, frame.size.w, 44));
  text_layer_set_background_color(bottom_line.label, GColorBlack);
  text_layer_set_text_color(bottom_line.label, GColorWhite);
  layer_add_child(root_layer, text_layer_get_layer(bottom_line.label));

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void do_deinit(void) {
  window_destroy(window);
  text_layer_destroy(top_line.label);
  text_layer_destroy(middle_line.label);
  text_layer_destroy(bottom_line.label);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
