#pragma once
#include <thread>
#include <mutex>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <ncurses.h>
#include "city.h"
#include "weather.h"

using nlohmann::json;

namespace msg {
inline const char* see_more = "press `+` to see more...";
inline const char* see_less = "press `-` to go back...";
inline const char* last_page = "it's the last forecast page!";
inline const char* first_page = "it's the first forecast page!";

inline const char* refresh_or_exit = "press any character to refresh or press `esc` to exit";
inline const char* exit = "press any character to exit";

inline const char* invalid_command = "invalid command!";
inline const char* config_parse_error = "couldn't parse config file. error: ";
} // namespace msg


const uint8_t kEscCharCode = 27;
const uint8_t kMaxDaysAmount = 16;

const uint8_t kBoxH = 10;
const uint8_t kBoxW = 30;
const uint8_t kDateBoxH = 3;
const uint8_t kDateBoxW = 10;
const uint8_t kPaddingW = 20;
const uint8_t kPaddingH = 3;
const uint8_t kPaddingBetween = 2;

const uint8_t kMorningHour = 6;
const uint8_t kNoonHour = 12;
const uint8_t kEveningHour = 18;

const uint8_t kDaysPerScreen = 3;
const uint8_t kCornerMsgY = kPaddingH * 2 + kBoxH * 3 + kPaddingBetween * 2;
const uint8_t kCornerMsgX = kBoxW * 4 + kPaddingW + 2;

enum daypart {
  morning, noon, evening, night
};

class UI {
  std::string config_path_;
  size_t freq_ = 0;
  size_t count_days_ = 0;
  size_t count_cities_ = 0;
  int cur_city_code_ = 0;
  int count_shown_days_ = 0;
  int count_displaying_days_ = 0;
  city::model city_model_;
  weather::day_model weather_model_;

  std::pair<bool, std::string> GetForecastParameters();
  void UpdCityInfo();
  void UpdWeatherInfo();
  static void CenteredPrint(const char* message, int n);
  static void IntroMsg();
  static void CantShowNextMsg();
  static void CantShowPrevMsg();
  static void MaxDaysMsg();
  void DrawDayTable(int day_number);
  void DrawFirstPage();
  void DrawBlock(int day_part_code, int day_number, int y);
  void UpdForecast();

public:
  explicit UI(const std::string& config);
  void RunApp();
};
