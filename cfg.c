
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

extern char dwmfont[];
extern char dmenufont[];
extern char stfont[];

static int is_comment(const char *line) {
  for (; *line; ++line) {
    if (*line == '#')
      return 1;
  }
  return 0;
}

static void skip_wp(char **line) {
  char *ptr = *line;
  while (*ptr == 0x20) ++ptr;
  *line = ptr;
}

static int parse_option(char **line, char *option) {
  skip_wp(line);
  char *ptr = *line;
  while (*ptr != '\0' && *ptr != 0x20 && *ptr != '=') ++ptr;
  
  const int size = ptr - *line;
  if (!size) return 0;
  
  memcpy(option, *line, size);
  option[size] = '\0';
  *line = ptr;

  return 1;
}

static int parse_eq(char **line) {
  char *ptr = *line;
  while (*ptr != '\0' && *ptr == 0x20) ++ptr;
  
  if (*ptr == '=') {
    *line = ptr + 1;
    return 1;
  } else {
    *line = ptr;
  }

  return 0;
}

static int parse_value(char **line, char *value) {
  skip_wp(line);
  char *ptr = *line;
  char delim = *ptr == '\"' ? '\"' : 0x20;
  if (delim == '\"') {
    ++ptr;
    *line = ptr;
  }
  while (*ptr != '\0' && *ptr != delim) ++ptr;
  if (delim == '\"' && *ptr != '\"') return 0;

  const int size = ptr - *line;
  if (!size) return 0;
  
  memcpy(value, *line, size);
  value[size] = '\0';
  *line = ptr;

  return 1;
}

static void 
parse_config(FILE *file) {
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;

  int linenum = 1;
  while ((nread = getline(&line, &len, file)) != -1) {
    if (is_comment(line)) continue;
    char option[256], value[256], *base = line;

    if (!(parse_option(&line, option) && parse_eq(&line) && parse_value(&line, value)))
      die("Parse error line %d: %s", linenum, base);

    if (strcmp("dwm_font", option) == 0) {
      strcpy(dwmfont, value);
    } else if (strcmp("dmenu_font", option) == 0) {
      strcpy(dmenufont, value);
    } else if (strcmp("st_font", option) == 0) {
      strcpy(stfont, value);
    } else
      die("Unknown option: \"%s\"\n", option);
    }
}

void 
load_config(void) {
  int is_hyperv = 0;
  const char *env_is_hyperv = getenv("IS_HYPERV");
  if (env_is_hyperv != NULL) {
    is_hyperv = strcmp(getenv("IS_HYPERV"), "1") == 0;
  }
  fprintf(stdout, "Using %s config\n", (is_hyperv ? "Hyper-V" : "Default"));

  const char* config_file = is_hyperv ? ".dwm/.wmhypervrc" : ".dwm/.wmrc";
  char *home_dir = getenv("HOME");
  if (home_dir != NULL) {
    char file_path[256];
    sprintf(file_path, "%s/%s", home_dir, config_file);
    FILE *file = fopen(file_path, "r");
    if (file != NULL) {
      parse_config(file);
      fclose(file);
    } else {
      fprintf(stderr, "Failed to open file: %s\n", file_path);
    }
  } else {
    fprintf(stderr, "Home directory not found.\n");
  }
}

