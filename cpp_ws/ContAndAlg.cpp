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

// Function to find the min/max, and average value of temperature, pressure, and humidity data classes for each city.
CityStats calculateCityStats(const std::vector<WeatherData>& data){
    CityStats stats;
    auto [minTemp, maxTemp] = std::minmax_element(data.begin(), data.end(),
    [](const WeatherData& a, const WeatherData& b){
        return a.temperature < b.temperature;
    });
    stats.minTemp = minTemp->temperature;
    stats.maxTemp = maxTemp->temperature;
    stats.avgTemp = std::accumulate(data.begin(), data.end(), 0.0f, [](float sum, const WeatherData & wd){
        return sum + wd.temperature;
    }) / data.size();

    auto [minPressure, maxPressure] = std::minmax_element(data.begin(), data.end(),
    [](const WeatherData& a, const WeatherData& b){
        return a.pressure < b.pressure;
    });
    stats.minPressure = minPressure->pressure;
    stats.maxPressure = maxPressure->pressure;
    stats.avgPressure = std::accumulate(data.begin(), data.end(), 0.0f, [](float sum, const WeatherData & wd){
        return sum + wd.pressure;
    }) / data.size();

    auto [minHumidity, maxHumidity] = std::minmax_element(data.begin(), data.end(),
    [](const WeatherData& a, const WeatherData& b){
        return a.humidity < b.humidity;
    });
    stats.minHumidity = minHumidity->humidity;
    stats.maxHumidity = maxHumidity->humidity;
    stats.avgHumidity = std::accumulate(data.begin(), data.end(), 0.0f, [](float sum, const WeatherData & wd){
        return sum + wd.humidity;
    }) / data.size();

    return stats;
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

    // Using a loop to iterate through the map and calculate the average and min/max values for each city.
    for (const auto& [city, data] : cityWeatherMap){
        CityStats stats = calculateCityStats(data);
        std::cout << "City: " << city << std::endl;
        std::cout << "Temperature - Min: " << stats.minTemp << ", Max: " << stats.maxTemp << ", Avg: " << stats.avgTemp << std::endl;
        std::cout << "Pressure - Min: " << stats.minPressure << ", Max: " << stats.maxPressure << ", Avg: " << stats.avgPressure << std::endl;
        std::cout << "Humidity - Min: " << stats.minHumidity << ", Max: " << stats.maxHumidity << ", Avg: " << stats.avgHumidity << std::endl;
    }

    return 0;
}