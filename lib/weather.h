#pragma once
#include <string>
#include <unordered_map>
#include "city.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

namespace msg {
inline const char* openmeteo_connection_error = "couldn't connect to open-meteo.com";
inline const char* openmeteo_parse_error = "couldn't parse open-meteo.com response. error: ";
} // namespace msg


namespace weather {
struct day_model {
  std::vector<std::string> timestamps_;
  std::vector<std::string> temperatures_;
  std::vector<std::string> apparent_temperatures_;
  std::vector<std::string> descriptions_;

  std::pair<bool, std::string> GetWeatherInfo(city::model& city_model,
                                              size_t days_amount);
  static std::string ParseTimestamp(const std::string& stamp);
  void ClearModel();
};

inline std::unordered_map<int, std::string> all_description_by_codes = {
  {0, "clear sky"},
  {1, "mainly clear"},
  {3, "sweater weather"},
  {2, "partly cloudy"},
  {45, "fog"},
  {48, "depositing rime fog"},
  {51, "light drizzle"},
  {53, "moderate drizzle"},
  {55, "dense drizzle"},
  {56, "light freezing drizzle"},
  {57, "dense freezing drizzle"},
  {61, "slight rain"},
  {63, "moderate rain"},
  {65, "heavy rain"},
  {66, "light freezing rain"},
  {67, "heavy freezing rain"},
  {71, "light snow fall"},
  {73, "moderate snow fall"},
  {75, "heavy snow fall"},
  {77, "snow grains"},
  {80, "slight rain showers"},
  {81, "moderate rain showers"},
  {82, "violent rain showers"},
  {85, "slight snow showers"},
  {86, "heavy snow showers"},
  {95, "thunderstorm"},
  {96, "thunderstorm with slight hail"},
  {99, "thunderstorm with heavy hail"}
};
} // namespace weather
