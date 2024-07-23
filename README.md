# Weather forecast app
A console application that displays the weather forecast for a list of cities. 
- Weather forecast displays for several days ahead (the default value is defined by the config file)
- Forecast updates with some frequency (defined by the config file)

## Data sources
- [Open-Meteo](https://open-meteo.com/en/docs#latitude=59.94&longitude=30.31&hourly=temperature_2m&forecast_days=16 ) for the forecast
- [Api-Ninjas](https://api-ninjas.com/api/city ) for the cities' coordinates
## Libraries used
- [ncurses](https://en.wikipedia.org/wiki/Ncurses) for visualization
- [C++ Requests](https://github.com/libcpr/cpr) for HTTP requests

## User interface details
- Switch between cities using the `n`, `p` keys
- Stop the program using `Esc`
- Switch between forecast pages for the current city with `+` and `-` keys

---
- The list of cities, the update frequency, and the number of forecast days must be defined in the config file in 
  `json` format. Use [config.json](config.json) as a template
- set _api-ninjas_ API key as an environment variable: `API_KEY=<your_api_key>`
- run app with only argument - config file (.json) path
