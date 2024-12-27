#include <ctype.h>
#include <locale.h>
#include <ncurses/ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");

  initscr();
  noecho();
  keypad(stdscr, 1);
  curs_set(0);
  start_color();

  init_pair(1, COLOR_BLACK, COLOR_BLACK);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  init_pair(4, COLOR_RED, COLOR_BLACK);
  init_pair(5, COLOR_BLACK, COLOR_WHITE);
  init_pair(6, COLOR_YELLOW, COLOR_BLACK);

  int done = 0;
  int scroll = 0;
  char header[64] = {};

  FILE *file = fopen(argv[1], "r");

  if (file == NULL) {
    fprintf(stderr, "File provided \"%s\" is not found\n", argv[1]);
    return 1;
  }

  while (!done) {
    clear();

    char ch;
    char line[512] = {};
    int line_num = 0;

    rewind(file);

    while ((ch = fgetc(file)) != EOF) {
      if (ch == '\n') {
        int text_offset[] = {line_num + scroll, 0};

        if (strcmp(header, "") != 0) {
          text_offset[0] += 1;
        }

        if (strncmp(line, "# ", 2) == 0) {
          memmove(line, line + 1, strlen(line) + 1);
          memcpy(line + strlen(line), " ", 1);
          memcpy(line, " ", 1);
          text_offset[1] += 1;
          attron(COLOR_PAIR(1) | A_BOLD);

          for (int i = 0; i < COLS; i++) {
            mvprintw(text_offset[0], i, "—");
          }

          attroff(COLOR_PAIR(1) | A_BOLD);
        }

        if (strncmp(line, "## ", 3) == 0) {
          text_offset[1] = COLS / 2 - strlen(line) / 2;
          memmove(line, line + 2, strlen(line) + 1);
          memcpy(line, " ", 1);
          memcpy(line + strlen(line), " ", 1);
          attron(COLOR_PAIR(1) | A_BOLD);

          for (int i = 0; i < COLS; i++) {
            mvprintw(text_offset[0], i, "—");
          }

          attroff(COLOR_PAIR(1) | A_BOLD);
        }

        if (strncmp(line, "- ", 2) == 0) {
          memmove(line, line + 2, strlen(line) + 1);
          text_offset[1] += 2;
          attron(COLOR_PAIR(2));
          mvprintw(text_offset[0], 0, "•");
          attroff(COLOR_PAIR(2));
        }

        if (strncmp(line, "// ", 3) == 0) {
          attron(COLOR_PAIR(1) | A_BOLD);
          memmove(line, line + 3, 61);
        }

        if (strncmp(line, "[ ] ", 4) == 0) {
          memmove(line, line + 4, strlen(line) + 1);
          text_offset[1] += 2;
          attron(COLOR_PAIR(4));
          mvprintw(text_offset[0], 0, "");
          attroff(COLOR_PAIR(4));
        }

        if (strncmp(line, "[x] ", 4) == 0) {
          memmove(line, line + 4, strlen(line) + 1);
          text_offset[1] += 2;
          attron(COLOR_PAIR(3));
          mvprintw(text_offset[0], 0, "");
          attroff(COLOR_PAIR(3));
        }

        if (strncmp(line, "? ", 2) == 0) {
          memmove(line, line + 2, strlen(line) + 1);
          text_offset[1] += 2;
          attron(COLOR_PAIR(2));
          mvprintw(text_offset[0], 0, "");
        }

        if (strncmp(line, "! ", 2) == 0) {
          memmove(line, line + 2, strlen(line) + 1);
          text_offset[1] += 2;
          attron(COLOR_PAIR(6));
          mvprintw(text_offset[0], 0, "");
        }

        if (strncmp(line, "!! ", 3) == 0) {
          memmove(line, line + 3, strlen(line) + 1);
          text_offset[1] += 2;
          attron(COLOR_PAIR(4));
          mvprintw(text_offset[0], 0, "");
        }

        char ordered_element[16] = {};
        int ordered_element_valid = 1;

        for (int i = 0; i < strlen(line); i++) {
          if (isdigit(line[i])) {
            ordered_element[i] = line[i];
          } else if (line[i] == '.') {
            ordered_element[i] = line[i];
            break;
          } else {
            ordered_element_valid = 0;
            break;
          }
        }

        if (ordered_element_valid) {
          memmove(line, line + strlen(ordered_element), strlen(line) + 1);
          text_offset[1] += strlen(ordered_element);
          attron(COLOR_PAIR(2));
          mvprintw(text_offset[0], 0, "%s", ordered_element);
          attroff(COLOR_PAIR(2));
        }

        if (strncmp(line, "---", 3) == 0) {
          attron(COLOR_PAIR(1) | A_BOLD);

          for (int i = 0; i < COLS; i++) {
            mvprintw(text_offset[0], text_offset[1] + i, "—");
          }

          attroff(COLOR_PAIR(1) | A_BOLD);
          memset(line, 0, 64);
        }

        if (strncmp(line, "_HEADER ", 8) == 0) {
          memcpy(header, line + 8, strlen(line) - 7);
          memset(line, 0, 64);
          line_num -= 1;
        }

        if (strncmp(line, "_PROGRESS_BAR ", 14) == 0) {
          char value_str[8] = {};
          memcpy(value_str, line + 14, 8);

          for (int i = 0; i < COLS / 4; i++) {
            if ((i * 100) / (COLS / 4) >= atoi(value_str)) {
              attron(COLOR_PAIR(1));
            } else {
              attron(COLOR_PAIR(2));
            }

            mvprintw(text_offset[0], text_offset[1] + i, "—");

            attroff(COLOR_PAIR(1) | COLOR_PAIR(2));
          }

          mvprintw(text_offset[0], text_offset[1] + COLS / 4 + 1, "%s%%",
                   value_str);
          memset(line, 0, 64);
        }

        mvprintw(text_offset[0], text_offset[1], "%s", line);

        attroff(COLOR_PAIR(1) | A_BOLD);
        attroff(COLOR_PAIR(2));

        line_num += 1;
        memset(line, 0, 64);
      } else {
        line[strlen(line)] = ch;
        line[strlen(line) + 1] = '\0';
      }
    }

    attron(COLOR_PAIR(5) | A_STANDOUT);

    for (int i = 0; i < COLS; i++) {
      if (strcmp(header, "") != 0) {
        mvprintw(0, i, " ");
      }

      mvprintw(LINES - 1, i, " ");
    }

    mvprintw(0, 0, "%s", header);
    mvprintw(LINES - 1, 0, "Viewing %s; Sroll: %d", argv[1], -scroll);
    attroff(A_STANDOUT);

    int input = getch();

    if (input == 'q') {
      done = 1;
    } else if (input == KEY_UP) {
      scroll += 1;
    } else if (input == KEY_DOWN) {
      scroll -= 1;
    }

    if (scroll > 0) {
      scroll = 0;
    }
  }

  fclose(file);

  endwin();

  return 0;
}
