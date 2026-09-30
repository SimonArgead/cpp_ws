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
    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    // Read the weather_data.csv file and store the data in a vector of WeatherData structs
    std::ifstream file("NewDataset_small_extra_v2.csv");
    std::vector<WeatherData> weatherData;
    std::string line;

    if (!file){
        throw std::runtime_error("Could not open file");
    }

    // Spring header-linjen over
    std::getline(file, line);

    int lineNumber = 1;

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

        weatherData.emplace_back(dt, rain, rain_minutes, average_temperature, maximum_temperature,
            minimum_temperature, average_windspeed, maximum_windspeed, pressure, cloud, humidity, sun, wind_dir);
    }

    // Divide into seperate vectors.
    for (const auto& data : weatherData){
        char keyBuffer[8];
        std::tm dtCopy = data.datetime;
        std::strftime(keyBuffer, sizeof(keyBuffer), "%Y-%m", &dtCopy);
        std::string key(keyBuffer);
        WeatherDataMap[key].push_back(data);
    }

    for (const auto& [yearMonth, data] : WeatherDataMap){
        WeatherStats stats;
        // Rain
        stats.avgRain = Average(data, [](const WeatherData& wd){ return wd.rain; });
        stats.minRain = MinMax(data, [](const WeatherData& wd){ return wd.rain; }).first;
        stats.maxRain = MinMax(data, [](const WeatherData& wd){ return wd.rain; }).second;

        // Rain Minutes
        stats.avgrainMinutes = Average(data, [](const WeatherData& wd){ return wd.rainMinutes; });
        stats.minrainMinutes = MinMax(data, [](const WeatherData& wd){ return wd.rainMinutes; }).first;
        stats.maxrainMinutes = MinMax(data, [](const WeatherData& wd){ return wd.rainMinutes; }).second;

        // Avg Temperature
        stats.avgavgTemp = Average(data, [](const WeatherData& wd){ return wd.avgTemp; });
        stats.minavgTemp = MinMax(data, [](const WeatherData& wd){ return wd.avgTemp; }).first;
        stats.maxavgTemp = MinMax(data, [](const WeatherData& wd){ return wd.avgTemp; }).second;

        // max Temperature
        stats.avgmaxTemp = Average(data, [](const WeatherData& wd){ return wd.maxTemp; });
        stats.minmaxTemp = MinMax(data, [](const WeatherData& wd){ return wd.maxTemp; }).first;
        stats.maxmaxTemp = MinMax(data, [](const WeatherData& wd){ return wd.maxTemp; }).second;

        // min Temperature
        stats.avgminTemp = Average(data, [](const WeatherData& wd){ return wd.minTemp; });
        stats.minminTemp = MinMax(data, [](const WeatherData& wd){ return wd.minTemp; }).first;
        stats.maxminTemp = MinMax(data, [](const WeatherData& wd){ return wd.minTemp; }).second;

        // Pressure
        stats.avgPressure = Average(data, [](const WeatherData& wd){ return wd.pressure; });
        stats.minPressure = MinMax(data, [](const WeatherData& wd){ return wd.pressure; }).first;
        stats.maxPressure = MinMax(data, [](const WeatherData& wd){ return wd.pressure; }).second;

        // Humidity
        stats.avgHumidity = Average(data, [](const WeatherData& wd){ return wd.humidity; });
        stats.minHumidity = MinMax(data, [](const WeatherData& wd){ return wd.humidity; }).first;
        stats.maxHumidity = MinMax(data, [](const WeatherData& wd){ return wd.humidity; }).second;

        // Use templates instead.
        std::cout << "Date Time: " << yearMonth << std::endl;
        std::cout << "  Avg Temp - Min: " << stats.minavgTemp << ", Max: " << stats.maxavgTemp << ", Avg: " << stats.avgavgTemp << std::endl;
        std::cout << "  Max Temp - Min: " << stats.minmaxTemp << ", Max: " << stats.maxmaxTemp << ", Avg: " << stats.avgmaxTemp << std::endl;
        std::cout << "  Min Temp - Min: " << stats.minminTemp << ", Max: " << stats.maxminTemp << ", Avg: " << stats.avgminTemp << std::endl;
        std::cout << "  Pressure - Min: " << stats.minPressure << ", Max: " << stats.maxPressure << ", Avg: " << stats.avgPressure << std::endl;
        std::cout << "  Humidity - Min: " << stats.minHumidity << ", Max: " << stats.maxHumidity << ", Avg: " << stats.avgHumidity << std::endl;
        std::cout << "  Rain     - Min: " << stats.minRain << ", Max: " << stats.maxRain << ", Avg: " << stats.avgRain << std::endl;

    }

    auto [globalMinPressure, globalMaxPressure] = MinMax(weatherData, [](const WeatherData& wd){ return wd.pressure; });
    auto [globalMinMinTemp, globalMaxMinTemp] = MinMax(weatherData, [](const WeatherData& wd){ return wd.minTemp; });
    auto [globalMinMaxTemp, globalMaxMaxTemp] = MinMax(weatherData, [](const WeatherData& wd){ return wd.maxTemp; });
    auto [globalMinAvgTemp, globalMaxAvgTemp] = MinMax(weatherData, [](const WeatherData& wd){ return wd.avgTemp;});
    auto [globalMinRain, globalMaxRain] = MinMax(weatherData, [](const WeatherData& wd){ return wd.rain; });

    std::cout << "--- Global values ---" << std::endl;
    std::cout << "Avg Temp - Min: " << globalMinAvgTemp << ", Max: " << globalMaxAvgTemp << std::endl;
    std::cout << "Min Temp - Min: " << globalMinMinTemp << ", Max: " << globalMaxMinTemp << std::endl;
    std::cout << "Max Temp - Min: " << globalMinMaxTemp << ", Max: " << globalMaxMaxTemp << std::endl;
    std::cout << "Pressure - Min: " << globalMinPressure << ", Max: " << globalMaxPressure << std::endl;
    std::cout << "Rain     - Min: " << globalMinRain << ", Max: " << globalMaxRain << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "\nTotal program time: " << elapsed.count() << " ms" << std::endl;

    return 0;
}