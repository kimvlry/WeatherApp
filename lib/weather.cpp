#include "weather.h"

std::pair<bool, std::string> weather::day_model::GetWeatherInfo(
  city::model& city_model,
  size_t days_amount) {
  cpr::Response response = cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast"}
                                  , cpr::Parameters{
                                    {"latitude", city_model.latitude_},
                                    {"longitude", city_model.longitude_},
                                    {"timezone", "auto"},
                                    {"hourly", "temperature_2m"},
                                    {"hourly", "apparent_temperature"},
                                    {"hourly", "weather_code"},
                                    {"forecast_days", std::to_string(days_amount)}
                                  });
  if (response.status_code != 200) {
    return {false, msg::openmeteo_connection_error};
  }

  try {
    json data = json::parse(response.text);
    json hourly_array = data["hourly"];
    for (size_t i = 0; i < hourly_array["time"].size(); ++i) {
      std::string temperature_str = to_string(hourly_array["temperature_2m"][i]);
      temperatures_.push_back(temperature_str);

      std::string apparent_str = to_string(hourly_array["apparent_temperature"][i]);
      apparent_temperatures_.push_back(apparent_str);

      descriptions_.push_back(all_description_by_codes[hourly_array["weather_code"][i]]);
      timestamps_.push_back(ParseTimestamp(hourly_array["time"][i]));
    }
    return {true, ""};
  }
  catch (const json::parse_error& e) {
    return {false, (msg::openmeteo_parse_error + std::string(e.what()))};
  }
}

std::string weather::day_model::ParseTimestamp(const std::string& stamp) {
  std::tm timeStruct = {};
  std::istringstream iss(stamp);
  iss >> std::get_time(&timeStruct, "%Y-%m-%dT%H:%M");
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%d %b", &timeStruct);
  return buffer;
}

void weather::day_model::ClearModel() {
  timestamps_.clear();
  temperatures_.clear();
  apparent_temperatures_.clear();
  descriptions_.clear();
}
