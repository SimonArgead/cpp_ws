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
#include <thread>
#include <mutex>
#include <future>
#include <iomanip>
#include <ctime>
#include <chrono>

// Function to read weather_data.csv. Remember to split each row between commas and place each row in a small struct in a vector.
struct WeatherData{
    std::tm datetime;
    float rain;
    float rainMinutes;
    float avgTemp;
    float minTemp;
    float maxTemp;
    float avgWindspeed;
    float maxWindspeed;
    float pressure;
    int cloud;
    int humidity;
    int sun;
    int windDir;

    WeatherData(std::tm datetime, float rain, float rainMinutes, float avgTemp, float minTemp, float maxTemp, float avgWindspeed, float maxWindspeed, float pressure, int cloud, int humidity, int sun, int windDir)
        : datetime(datetime), rain(rain),rainMinutes(rainMinutes), avgTemp(avgTemp), minTemp(minTemp), maxTemp(maxTemp), avgWindspeed(avgWindspeed), maxWindspeed(maxWindspeed), pressure(pressure), cloud(cloud), humidity(humidity), sun(sun), windDir(windDir) {
        };

};

struct WeatherStats{
    float minTemp, maxTemp, avgTemp;
    float minminTemp, maxminTemp, avgminTemp;
    float minmaxTemp, maxmaxTemp, avgmaxTemp;
    float minavgTemp, maxavgTemp, avgavgTemp;
    float minPressure, maxPressure, avgPressure;
    float minHumidity, maxHumidity, avgHumidity;
    float minRain, maxRain, avgRain;
    float minrainMinutes, maxrainMinutes, avgrainMinutes;
    float minCloud, maxCloud, avgCloud;
    float minSun, maxSun, avgSun;
    float minwindDir, maxwindDir, avgwindDir;
};

// Divide weather_data.csv into separate vectores for each unique value in the "city" column. Use a map or unordered_map to store the data, with the city name as the key and a vector of weather data as the value.
std::map<std::string, std::vector<WeatherData>> WeatherDataMap;

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
    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    // Read the weather_data.csv file and store the data in a vector of WeatherData structs
    std::ifstream file("NewDataset_small_extra_v2.csv");
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
        std::tm dt = {};
        std::string datetimeStr;

        // Establish classes
        float rain, rain_minutes, average_temperature, maximum_temperature, minimum_temperature;
        float average_windspeed, maximum_windspeed, pressure;
        int cloud, humidity, sun, wind_dir;

        std::getline(ss, datetimeStr, ',');
        std::istringstream dtStream(datetimeStr);

        dtStream >> std::get_time(&dt, "%Y-%m-%d %H:%M:%S");
        ss >> rain;
        ss.ignore();
        ss >> rain_minutes;
        ss.ignore();
        ss >> average_temperature;
        ss.ignore();
        ss >> maximum_temperature;
        ss.ignore();
        ss >> minimum_temperature;
        ss.ignore();
        ss >> average_windspeed;
        ss.ignore();
        ss >> maximum_windspeed;
        ss.ignore();
        ss >> pressure;
        ss.ignore();
        ss >> cloud;
        ss.ignore();
        ss >> humidity;
        ss.ignore();
        ss >> sun;
        ss.ignore();
        ss >> wind_dir;

        weatherData.emplace_back(dt, rain, rain_minutes, average_temperature, minimum_temperature,
            maximum_temperature, average_windspeed, maximum_windspeed, pressure, cloud, humidity, sun, wind_dir);
    }

    // Divide the weather data into separate vectors
    for (const auto& data : weatherData){
        char keyBuffer[8];
        std::tm dtCopy = data.datetime;
        std::strftime(keyBuffer, sizeof(keyBuffer), "%Y-%m", &dtCopy);
        std::string key(keyBuffer);
        WeatherDataMap[key].push_back(data);
    }

    std::vector<std::future<std::pair<std::string, WeatherStats>>> futures;

    std::map<std::string, WeatherStats> results; // store results

    for (const auto& [yearMonth, data] : WeatherDataMap){
        futures.push_back(std::async(std::launch::async, [yearMonth, data]() {
            WeatherStats stats;
            // Rain
            if (auto avgRain = Average(data, [](const WeatherData& wd){ return wd.rain; })){
                stats.avgRain = *avgRain;
            }
            else {
                stats.avgRain = 0.0f; // Handle empty data case
            }
            if (auto Rain = MinMax(data, [](const WeatherData& wd){ return wd.rain; })){
                stats.minRain = Rain->first;
                stats.maxRain = Rain->second;
            }

            else {
                stats.minRain = 0.0f;
                stats.maxRain = 0.0f;
            }

            // Rain Minutes
            if (auto avgrainMinutes = Average(data, [](const WeatherData& wd){ return wd.rainMinutes; })){
                stats.avgrainMinutes = *avgrainMinutes;
            }
            else {
                stats.avgrainMinutes = 0.0f; // Handle empty data case
            }
            if (auto rainMinutes = MinMax(data, [](const WeatherData& wd){ return wd.rainMinutes; })){
                stats.minrainMinutes = rainMinutes->first;
                stats.maxrainMinutes = rainMinutes->second;
            }

            else {
                stats.minRain = 0.0f;
                stats.maxRain = 0.0f;
            }
            
            // Avg Temperature
            if (auto avgavgTemp = Average(data, [](const WeatherData& wd){ return wd.avgTemp; })){
                stats.avgavgTemp = *avgavgTemp;
            }
            else {
                stats.avgavgTemp = 0.0f; // Handle empty data case
            }
            if (auto minmaxTemp = MinMax(data, [](const WeatherData& wd){ return wd.avgTemp; })){
                stats.minavgTemp = minmaxTemp->first;
                stats.maxavgTemp = minmaxTemp->second;
            }
            else {
                stats.minavgTemp = 0.0f;
                stats.maxavgTemp = 0.0f;
            }

            // max Temperature
            if (auto avgmaxTemp = Average(data, [](const WeatherData& wd){ return wd.maxTemp; })){
                stats.avgmaxTemp = *avgmaxTemp;
            }
            else {
                stats.avgmaxTemp = 0.0f; // Handle empty data case
            }
            if (auto minmaxTemp = MinMax(data, [](const WeatherData& wd){ return wd.maxTemp; })){
                stats.minmaxTemp = minmaxTemp->first;
                stats.maxmaxTemp = minmaxTemp->second;
            }
            else {
                stats.minmaxTemp = 0.0f;
                stats.maxmaxTemp = 0.0f;
            }

            // min Temperature
            if (auto avgminTemp = Average(data, [](const WeatherData& wd){ return wd.minTemp; })){
                stats.avgminTemp = *avgminTemp;
            }
            else {
                stats.avgminTemp = 0.0f; // Handle empty data case
            }
            if (auto minminTemp = MinMax(data, [](const WeatherData& wd){ return wd.minTemp; })){
                stats.minminTemp = minminTemp->first;
                stats.maxminTemp = minminTemp->second;
            }
            else {
                stats.minminTemp = 0.0f;
                stats.maxminTemp = 0.0f;
            }

            // Pressure
            if (auto avgPressure = Average(data, [](const WeatherData& wd){ return wd.pressure; })){
                stats.avgPressure = *avgPressure;
            }
            else {
                stats.avgPressure = 0.0f; // Handle empty data case
            }
            if (auto Pressure = MinMax(data, [](const WeatherData& wd){ return wd.pressure; })){
                stats.minPressure = Pressure->first;
                stats.maxPressure = Pressure->second;
            }

            else {
                stats.minPressure = 0.0f;
                stats.maxPressure = 0.0f;
            }

            return std::make_pair(yearMonth, stats);
    }));
    }
    
    for (auto& future : futures){
        auto [yearMonth, stats] = future.get();
        // Use templates instead.
        std::cout << "Date Time: " << yearMonth << std::endl;
        std::cout << "  Avg Temp - Min: " << stats.minavgTemp << ", Max: " << stats.maxavgTemp << ", Avg: " << stats.avgavgTemp << std::endl;
        std::cout << "  Max Temp - Min: " << stats.minmaxTemp << ", Max: " << stats.maxmaxTemp << ", Avg: " << stats.avgmaxTemp << std::endl;
        std::cout << "  Min Temp - Min: " << stats.minminTemp << ", Max: " << stats.maxminTemp << ", Avg: " << stats.avgminTemp << std::endl;
        std::cout << "  Pressure - Min: " << stats.minPressure << ", Max: " << stats.maxPressure << ", Avg: " << stats.avgPressure << std::endl;
        std::cout << "  Rain     - Min: " << stats.minRain << ", Max: " << stats.maxRain << ", Avg: " << stats.avgRain << std::endl;

    }

    std::cout << "Global min/max values" << std::endl;

    auto printGlobalStat = [&](const std::string& label, auto getValue){
        auto avg = Average(weatherData, getValue);
        auto minmax = MinMax(weatherData, getValue);
        if (avg && minmax){
            std::cout << "  " << label << " - Min: " << minmax->first
                       << ", Max: " << minmax->second
                       << ", Avg: " << *avg << std::endl;
        }
    };

    printGlobalStat("Avg Temp", [](const WeatherData& wd){ return wd.avgTemp; });
    printGlobalStat("Max Temp", [](const WeatherData& wd){ return wd.maxTemp; });
    printGlobalStat("Min Temp", [](const WeatherData& wd){ return wd.minTemp; });
    printGlobalStat("Pressure", [](const WeatherData& wd){ return wd.pressure; });
    printGlobalStat("Humidity", [](const WeatherData& wd){ return wd.humidity; });
    printGlobalStat("Rain",     [](const WeatherData& wd){ return wd.rain; });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "\nTotal program time: " << elapsed.count() << " ms" << std::endl;

    // Total program time: 183.994 ms

    return 0;
}