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
template<typename T, typename Func>
double Average(const std::vector<T>& data, Func getValue){
    double sum = 0.0;
    for (const auto & item : data){
        sum += getValue(item);
    }
    return sum / data.size();
}

// Template function that finds min/max values of a given data type.
template<typename T, typename Func> 
std::pair<double, double> MinMax(const std::vector<T>& data, Func getValue){
    auto [minIt, maxIt] = std::minmax_element(data.begin(), data.end(),
        [getValue](const T& a, const T& b){
            return getValue(a) < getValue(b);
        });
        return {getValue(*minIt), getValue(*maxIt)};
}

int main(){
    // Read the weather_data.csv file and store the data in a vector of WeatherData structs
    std::ifstream file("weather_data.csv");
    std::vector<WeatherData> weatherData;
    std::string line;
    while (std::getline(file, line)){
        std::istringstream ss(line);
        std::string city;
        float temperature, pressure, humidity;
        std::getline(ss, city, ',');
        ss >> temperature;
        ss.ignore();
        ss >> pressure;
        ss.ignore();
        ss >> humidity;
        weatherData.emplace_back(city, temperature, pressure, humidity);
    }

    // Divide the weather data into separate vectors for each unique city
    for (const auto& data : weatherData){
        cityWeatherMap[data.city].push_back(data);
    }

    for (const auto& [city, data] : cityWeatherMap){
        CityStats stats;
        // Temperature
        stats.avgTemp = Average(data, [](const WeatherData& wd){ return wd.temperature; });
        stats.minTemp = MinMax(data, [](const WeatherData& wd){ return wd.temperature; }).first;
        stats.maxTemp = MinMax(data, [](const WeatherData& wd){ return wd.temperature; }).second;

        // Pressure
        stats.avgPressure = Average(data, [](const WeatherData& wd){ return wd.pressure; });
        stats.minPressure = MinMax(data, [](const WeatherData& wd){ return wd.pressure; }).first;
        stats.maxPressure = MinMax(data, [](const WeatherData& wd){ return wd.pressure; }).second;

        // Humidity
        stats.avgHumidity = Average(data, [](const WeatherData& wd){ return wd.humidity; });
        stats.minHumidity = MinMax(data, [](const WeatherData& wd){ return wd.humidity; }).first;
        stats.maxHumidity = MinMax(data, [](const WeatherData& wd){ return wd.humidity; }).second;

         // Use templates instead.
        std::cout << "City: " << city << std::endl;
        std::cout << "Temperature - Min: " << stats.minTemp << ", Max: " << stats.maxTemp << ", Avg: " << stats.avgTemp << std::endl;
        std::cout << "Pressure - Min: " << stats.minPressure << ", Max: " << stats.maxPressure << ", Avg: " << stats.avgPressure << std::endl;
        std::cout << "Humidity - Min: " << stats.minHumidity << ", Max: " << stats.maxHumidity << ", Avg: " << stats.avgHumidity << std::endl;
    }

    return 0;
}