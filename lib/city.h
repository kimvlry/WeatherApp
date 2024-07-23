#pragma once
#include <string>
#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using nlohmann::json;

namespace msg {
inline const char* city_connection_error = "couldn't connect to api-ninjas.com";
inline const char* city_parse_error = "couldn't parse api-ninjas.com response. error: ";
inline const char* config_not_open = "couldn't open configuration file";
inline const char* apikey_error = "couldn't find apikey";
} // namespace msg

namespace city {
struct model {
  std::string name_;
  std::string latitude_;
  std::string longitude_;
  std::string config_path_;
  std::pair<bool, std::string> GetCityInfo(int city_number);
};

void EncodeURLSpaces(std::string& input);
std::optional<std::string> GetApiKey();
} // namespace city
