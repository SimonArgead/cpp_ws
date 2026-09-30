#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <type_traits>
#include <optional>

// Function to read weather_data.csv. Remember to split each row between commas and place each row in a small struct in a vector.
struct WeatherData{
    std::string city;
    float temperature;
    float pressure;
    float humidity;

    WeatherData(std::string city, float temperature, float pressure, float humidity)
        : city(city), temperature(temperature), pressure(pressure), humidity(humidity) {

        };

};

struct CityStats{
    float minTemp, maxTemp, avgTemp;
    float minPressure, maxPressure, avgPressure;
    float minHumidity, maxHumidity, avgHumidity;
};

// Divide weather_data.csv into separate vectores for each unique value in the "city" column. Use a map or unordered_map to store the data, with the city name as the key and a vector of weather data as the value.
std::unordered_map<std::string, std::vector<WeatherData>> cityWeatherMap;

// Template function that finds the average value of a given data type.
// using std::optional to handle empty data cases.
template<typename T, typename Func>
std::optional<float> Average(const std::vector<T>& data, Func getValue){
    if (data.empty()){
        return std::nullopt;
    }
    double sum = 0.0;
    for (const auto & item : data){
        sum += getValue(item);
    }
    return sum / data.size();
}

// Template function that finds min/max values of a given data type.
template<typename T, typename Func> 
std::optional<std::pair<float, float>> MinMax(const std::vector<T>& data, Func getValue){
    if (data.empty()){
        return std::nullopt;
    }
    auto [minIt, maxIt] = std::minmax_element(data.begin(), data.end(),
        [getValue](const T& a, const T& b){
            return getValue(a) < getValue(b);
        });
        return {std::make_pair(getValue(*minIt), getValue(*maxIt))};
}

int main(){
    // Read the weather_data.csv file and store the data in a vector of WeatherData structs
    std::ifstream file("weather_data_errors.csv");
    std::vector<WeatherData> weatherData;
    std::string line;

    // In case of file reading error
    if (!file){
        throw std::runtime_error("Could not open file");
    }
    int lineNumber = 1;
    std::getline(file, line);

    while (std::getline(file, line)){
        lineNumber++;
        std::istringstream ss(line);
        std::string city;
        float temperature, pressure, humidity;
        std::getline(ss, city, ',');
        ss >> temperature;
        ss.ignore();
        ss >> pressure;
        ss.ignore();
        ss >> humidity;

        if (ss.fail()) {
        std::cerr << "Advarsel: ugyldig data på linje " << lineNumber << ": \"" << line << "\" - springes over\n";
        continue; // spring denne linje over, tilføj IKKE til weatherData
        }

        weatherData.emplace_back(city, temperature, pressure, humidity);
    }

    // Divide the weather data into separate vectors for each unique city
    for (const auto& data : weatherData){
        cityWeatherMap[data.city].push_back(data);
    }

    for (const auto& [city, data] : cityWeatherMap){
        CityStats stats;
        // Temperature
        if (auto avgTemp = Average(data, [](const WeatherData& wd){ return wd.temperature; })){
            stats.avgTemp = *avgTemp;
        }
        else {
            stats.avgTemp = 0.0f; // Handle empty data case
        }
        if (auto minmaxTemp = MinMax(data, [](const WeatherData& wd){ return wd.temperature; })){
            stats.minTemp = minmaxTemp->first;
            stats.maxTemp =minmaxTemp->second;
        }
        else {
            stats.minTemp = 0.0f;
            stats.maxTemp = 0.0f;
        }

        // Pressure
        if (auto avgPressure = Average(data, [](const WeatherData& wd){ return wd.pressure; })){
            stats.avgPressure = *avgPressure;
        }
        else {
            stats.avgPressure = 0.0f; // Handle empty data case
        }
        if (auto minmaxPressure = MinMax(data, [](const WeatherData& wd){ return wd.pressure; })){
            stats.minPressure = minmaxPressure->first;
            stats.maxPressure =minmaxPressure->second;
        }
        else {
            stats.minPressure = 0.0f;
            stats.maxPressure = 0.0f;
        }

        // Humidity
        if (auto avgHumidity = Average(data, [](const WeatherData& wd){ return wd.humidity; })){
            stats.avgHumidity = *avgHumidity;
        }
        else {
            stats.avgHumidity = 0.0f; // Handle empty data case
        }
        if (auto minmaxHumidity = MinMax(data, [](const WeatherData& wd){ return wd.humidity; })){
            stats.minHumidity = minmaxHumidity->first;
            stats.maxHumidity =minmaxHumidity->second;
        }
        else {
            stats.minHumidity = 0.0f;
            stats.maxHumidity = 0.0f;
        }

         // Use templates instead.
        std::cout << "City: " << city << std::endl;
        std::cout << "Temperature - Min: " << stats.minTemp << ", Max: " << stats.maxTemp << ", Avg: " << stats.avgTemp << std::endl;
        std::cout << "Pressure - Min: " << stats.minPressure << ", Max: " << stats.maxPressure << ", Avg: " << stats.avgPressure << std::endl;
        std::cout << "Humidity - Min: " << stats.minHumidity << ", Max: " << stats.maxHumidity << ", Avg: " << stats.avgHumidity << std::endl;
    }

    return 0;
}