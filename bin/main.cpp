#include <iostream>
#include <lib/interface.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "invlid input. run app with only argument: config .json file path\n";
    return 1;
  }
  std::string config_path = argv[1];
  UI forecast(config_path);
  forecast.RunApp();
}
