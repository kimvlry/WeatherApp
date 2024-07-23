#include "interface.h"

UI::UI(const std::string& config) {
  config_path_ = config;
}

std::pair<bool, std::string> UI::GetForecastParameters() {
  std::ifstream file(config_path_);
  if (!file.is_open()) {
    return {false, msg::config_not_open};
  }
  try {
    json config = json::parse(file);
    freq_ = config["update_frequency"];
    count_days_ = config["forecast_days"];
    count_cities_ = config["cities_list"].size();
    return {true, ""};
  }
  catch (const json::parse_error& e) {
    return {false, (msg::config_parse_error + std::string(e.what()))};
  }
}

void UI::CenteredPrint(const char* message, int line) {
  start_color();
  init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
  attron(COLOR_PAIR(1));
  int x = (COLS - strlen(message)) / 2;
  mvprintw(line, x, message);
  attroff(COLOR_PAIR(1));
  refresh();
}

void UI::DrawBlock(int day_part_code, int day_number, int y) {
  int ix = (day_number - 1) * 24;
  int x;
  const char* name;
  switch (day_part_code) {
    case morning:
      name = "morning";
      ix += kMorningHour;
      x = kPaddingW + kBoxW;
      break;
    case noon:
      name = "noon";
      ix += kNoonHour;
      x = kPaddingW + kBoxW * 2;
      break;
    case evening:
      name = "evening";
      ix += kEveningHour;
      x = kPaddingW + kBoxW * 3;
      break;
    case night:
      name = "night";
      x = kPaddingW + kBoxW * 4;
      break;
    default:
      break;
  }
  const char* t = weather_model_.temperatures_[ix].c_str();
  const char* apparent = weather_model_.apparent_temperatures_[ix].c_str();
  const char* description = weather_model_.descriptions_[ix].c_str();
  const char* timestamp = weather_model_.timestamps_[ix].c_str();

  std::unique_ptr<WINDOW, decltype(&delwin)> date_win(newwin(kDateBoxH, kDateBoxW, y, kPaddingW + 7), &delwin);
  box(date_win.get(), 0, 0);
  mvwprintw(date_win.get(), 1, 2, timestamp);
  wrefresh(date_win.get());

  std::unique_ptr<WINDOW, decltype(&delwin)> win(newwin(kBoxH, kBoxW, y, x), &delwin);
  mvprintw(y - 1, x + 1, name);
  box(win.get(), 0, 0);

  mvwprintw(win.get(), 1, 2, description);
  mvwprintw(win.get(), 3, 2, t);
  mvwprintw(win.get(), 3, 7, "°C");
  mvwprintw(win.get(), 4, 2, "feels like ");
  mvwprintw(win.get(), 4, 13, apparent);
  mvwprintw(win.get(), 4, 18, "°C");
  wrefresh(win.get());
  refresh();
}

void UI::DrawDayTable(int day_number) {
  int y;
  switch (day_number % kDaysPerScreen) {
    case 1:
      y = kPaddingH;
      break;
    case 2:
      y = kPaddingH + kBoxH + kPaddingBetween;
      break;
    case 0:
      y = kPaddingH + (kBoxH + kPaddingBetween) * 2;
      break;
    default:
      break;
  }
  DrawBlock(morning, day_number, y);
  DrawBlock(noon, day_number, y);
  DrawBlock(evening, day_number, y);
  DrawBlock(night, day_number, y);
}

void UI::IntroMsg() {
  clear();
  CenteredPrint("hi darling!", LINES / 2 - 7);
  CenteredPrint("this is how u use da app:", LINES / 2 - 5);
  CenteredPrint("press `n` to switch to the next city", LINES / 2 - 4);
  CenteredPrint("press `p` to go back to the previous city", LINES / 2 - 3);
  CenteredPrint("press `esc` to exit", LINES / 2 - 2);
  CenteredPrint("now press ENTER to start", LINES / 2);
}

void UI::CantShowNextMsg() {
  clear();
  CenteredPrint("oopsie!", LINES / 2 - 6);
  CenteredPrint("there's no more cities", LINES / 2 - 4);
  CenteredPrint("press `p` to go back to the previous city", LINES / 2 - 3);
  int ch = getch();
  while (ch != 'p') {
    ch = getch();
  }
}

void UI::CantShowPrevMsg() {
  clear();
  CenteredPrint("oopsie!", LINES / 2 - 6);
  CenteredPrint("it was the first city of the list", LINES / 2 - 4);
  CenteredPrint("press `n` to switch to the next city", LINES / 2 - 3);
  int ch = getch();
  while (ch != 'n') {
    ch = getch();
  }
}

void UI::MaxDaysMsg() {
  clear();
  CenteredPrint("oopsie... i only know weather for the next 16 days..", LINES / 2 - 6);
  CenteredPrint("go check your config file and come back!", LINES / 2 - 4);
  CenteredPrint("press any character to exit ", LINES / 2 - 3);
  getch();
}

void UI::DrawFirstPage() {
  weather_model_.ClearModel();
  UpdWeatherInfo();
  clear();
  mvprintw(kPaddingH, kPaddingW / 2.5, city_model_.name_.c_str());
  while (count_shown_days_ < count_days_ && count_displaying_days_ <
    kDaysPerScreen) {
    DrawDayTable(++count_shown_days_);
    ++count_displaying_days_;
  }
  count_displaying_days_ = 0;

  if (count_shown_days_ < count_days_) {
    mvprintw(kCornerMsgY, kCornerMsgX, msg::see_more);
    refresh();
  }
}

void UI::UpdCityInfo() {
  while (true) {
    auto [success, error_description] = city_model_.GetCityInfo(cur_city_code_);
    if (success) {
      break;
    }
    clear();
    CenteredPrint("oopsie", LINES / 2 - 6);
    CenteredPrint(error_description.c_str(), LINES / 2 - 4);
    int ch;
    if (error_description == msg::city_connection_error) {
      CenteredPrint(msg::refresh_or_exit, LINES / 2 - 3);
      if ((ch = getch()) == kEscCharCode) {
        return;
      }
    }
    else {
      CenteredPrint(msg::exit, LINES / 2 - 3);
      getch();
      break;
    }
  }
}

void UI::UpdWeatherInfo() {
  while (true) {
    auto [success, error_description] = weather_model_.GetWeatherInfo(city_model_, count_days_);
    if (success) {
      break;
    }
    clear();
    CenteredPrint("oopsie", LINES / 2 - 6);
    CenteredPrint(error_description.c_str(), LINES / 2 - 4);
    int ch;
    if (error_description == msg::openmeteo_connection_error) {
      CenteredPrint(msg::refresh_or_exit, LINES / 2 - 3);
      if ((ch = getch()) == kEscCharCode) {
        return;
      }
    }
    else {
      CenteredPrint(msg::exit, LINES / 2 - 3);
      getch();
      break;
    }
  }
}

std::mutex data_mutex;
std::atomic<bool> exit_flag(false);

void UI::UpdForecast() {
  while (!exit_flag) {
    {
      std::lock_guard<std::mutex> lock(data_mutex);
      count_shown_days_ = 0;
      count_displaying_days_ = 0;
      UpdCityInfo();
      DrawFirstPage();
    }
    std::this_thread::sleep_for(std::chrono::seconds(freq_));
  }
}

void UI::RunApp() {
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  auto [success, error_description] = GetForecastParameters();
  city_model_.config_path_ = config_path_;
  if (!success) {
    clear();
    CenteredPrint("oopsie", LINES / 2 - 6);
    CenteredPrint(error_description.c_str(), LINES / 2 - 4);
    CenteredPrint(msg::exit, LINES / 2 - 3);
    getch();
    return;
  }

  if (count_days_ > kMaxDaysAmount) {
    MaxDaysMsg();
    return;
  }

  UpdCityInfo();
  IntroMsg();
  int ch;
  while (true) {
    ch = getch();
    if (ch == kEscCharCode) {
      exit_flag = true;
      return;
    }
    if (ch == '\n') {
      DrawFirstPage();
      break;
    }
    else {
      mvprintw(kCornerMsgY, kCornerMsgX, msg::invalid_command);
      refresh();
    }
  }

  std::thread updater_thread(&UI::UpdForecast, this);

  while ((ch = getch()) != kEscCharCode) {
    std::lock_guard<std::mutex> lock(data_mutex);
    switch (ch) {
      case 'n':
        count_shown_days_ = 0;
        if (cur_city_code_ + 1 >= count_cities_) {
          CantShowNextMsg();
          break;
        }
        ++cur_city_code_;
        UpdCityInfo();
        DrawFirstPage();
        break;

      case 'p':
        count_shown_days_ = 0;
        if (cur_city_code_ - 1 < 0) {
          CantShowPrevMsg();
          break;
        }
        --cur_city_code_;
        UpdCityInfo();
        DrawFirstPage();
        break;

      case '+':
        if (count_shown_days_ < count_days_) {
          clear();
          mvprintw(kPaddingH, kPaddingW / 2.5, city_model_.name_.c_str());
          while (count_shown_days_ < count_days_ && count_displaying_days_ < kDaysPerScreen) {
            DrawDayTable(++count_shown_days_);
            ++count_displaying_days_;
          }
          count_displaying_days_ = 0;
          if (count_shown_days_ < count_days_) {
            mvprintw(kCornerMsgY, kCornerMsgX, msg::see_more);
          }
          mvprintw(kCornerMsgY + 1, kCornerMsgX, msg::see_less);
        }
        else {
          start_color();
          init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
          attron(COLOR_PAIR(1));
          mvprintw(kCornerMsgY, kCornerMsgX, msg::last_page);
          attroff(COLOR_PAIR(1));
        }
        refresh();
        break;

      case '-':
        if (count_shown_days_ > kDaysPerScreen) {
          clear();
          int step_back = kDaysPerScreen * 2 + (count_shown_days_ % kDaysPerScreen);
          count_shown_days_ -= step_back;
          mvprintw(kPaddingH, kPaddingW / 2.5, city_model_.name_.c_str());
          while (count_shown_days_ < count_days_ && count_displaying_days_ < kDaysPerScreen) {
            DrawDayTable(++count_shown_days_);
            ++count_displaying_days_;
          }
          count_displaying_days_ = 0;
          if (count_shown_days_ > kDaysPerScreen) {
            mvprintw(kCornerMsgY, kCornerMsgX, msg::see_less);
          }
          if (count_shown_days_ < count_days_) {
            mvprintw(kCornerMsgY + 1, kCornerMsgX, msg::see_more);
          }
        }
        else {
          start_color();
          init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
          attron(COLOR_PAIR(1));
          mvprintw(kCornerMsgY, kCornerMsgX, msg::first_page);
          attroff(COLOR_PAIR(1));
        }
        refresh();
        break;

      default:
        mvprintw(kCornerMsgY, kCornerMsgX, msg::invalid_command);
        refresh();
        break;
    }
  }
  exit_flag = true;
  updater_thread.join();
  endwin();
}
