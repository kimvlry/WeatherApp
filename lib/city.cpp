#include "city.h"

void city::EncodeURLSpaces(std::string& input) {
  size_t pos = 0;
  std::string search = " ";
  std::string replace = "%20";
  while ((pos = input.find(search, pos)) != std::string::npos) {
    input.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

std::optional<std::string> city::GetApiKey() {
  auto apikey = std::getenv("API_KEY");
  if (apikey == nullptr) {
    return std::nullopt;
  }
  return apikey;
}

std::pair<bool, std::string> city::model::GetCityInfo(int city_number) {
  std::ifstream file(config_path_);
  if (!file.is_open()) {
    return {false, msg::config_not_open};
  }
  try {
    json config = json::parse(file);
    std::string base_url = "https://api.api-ninjas.com/v1/city?name=";
    std::string city_name = config["cities_list"][city_number];

    EncodeURLSpaces(city_name);
    base_url.append(city_name);
    if (!GetApiKey().has_value()) {
      return {false, msg::apikey_error};
    }
    std::string key = GetApiKey().value();
    cpr::Response ninjas_response = Get(cpr::Url{base_url},
                                        cpr::Header{{"X-Api-Key", key}});

    if (ninjas_response.status_code != 200) {
      return {false, msg::city_connection_error};
    }

    json ninjas_parsed = json::parse(ninjas_response.text);
    name_ = ninjas_parsed[0]["name"];
    latitude_ = to_string(ninjas_parsed[0]["latitude"]);
    longitude_ = to_string(ninjas_parsed[0]["longitude"]);
    return {true, ""};
  }
  catch (const json::parse_error& e) {
    return {false, (msg::city_parse_error + std::string(e.what()))};
  }
}
