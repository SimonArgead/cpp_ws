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

// Struct for weather stats
struct WeatherStats{
    /* float minTemp, maxTemp, avgTemp;
    float minminTemp, maxminTemp, avgminTemp;
    float minmaxTemp, maxmaxTemp, avgmaxTemp;
    float minavgTemp, maxavgTemp, avgavgTemp;
    float minPressure, maxPressure, avgPressure;
    float minHumidity, maxHumidity, avgHumidity;
    float minRain, maxRain, avgRain;
    float minrainMinutes, maxrainMinutes, avgrainMinutes;
    float minCloud, maxCloud, avgCloud;
    float minSun, maxSun, avgSun;
    float minwindDir, maxwindDir, avgwindDir;*/
    float avgWindspeed, maxWindspeed;
    float avgPowerOutput;
};

// Function to estimate windpower output
double EstimatePowerOutput(double windSpeed) {
    const double cutIn = 3.0, rated = 12.0, cutOut = 25.0;
    if (windSpeed < cutIn || windSpeed > cutOut) return 0.0;
    if (windSpeed >= rated) return 1.0; // 100% rated effekt
    // Kubisk interpolation mellem cut-in og rated
    double fraction = (windSpeed - cutIn) / (rated - cutIn);
    return std::pow(fraction, 3);
}

// class to convert windpower 
class PowerConverter {
public:
    PowerConverter(double ratedPower, double baseEfficiency)
        : ratedPower_(ratedPower), baseEfficiency_(baseEfficiency) {}

    double CalculateEfficiency(double loadFraction, double ambientTemp) const {
        double tempDerating = (ambientTemp > 40.0) ? (1.0 - (ambientTemp - 40.0) * 0.01) : 1.0;
        double loadEfficiency = baseEfficiency_ * (1.0 - std::pow(1.0 - loadFraction, 2) * 0.1);
        return loadEfficiency * tempDerating;
    }

private:
    double ratedPower_;
    double baseEfficiency_;
};

// Divide weather_data.csv into separate vectores for each unique value in the "city" column. Use a map or unordered_map to store the data, with the city name as the key and a vector of weather data as the value.
std::map<std::string, std::vector<WeatherData>> WeatherDataMap;

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
    std::vector<double> powerOutputs; // Vector for estimated poweroutputs

    for (const auto& [yearMonth, data] : WeatherDataMap){
    futures.push_back(std::async(std::launch::async, [yearMonth, data]() {
        WeatherStats stats;

        double totalWindspeed = 0.0;
        double totalPowerFraction = 0.0;
        for (const auto& wd : data) {
            totalWindspeed += wd.avgWindspeed;
            totalPowerFraction += EstimatePowerOutput(wd.avgWindspeed);
        }

        stats.avgWindspeed = data.empty() ? 0.0f : (totalWindspeed / data.size());
        stats.avgPowerOutput = data.empty() ? 0.0f : (totalPowerFraction / data.size());

        return std::make_pair(yearMonth, stats);
    }));
    }

    for (auto& future : futures){
        auto [yearMonth, stats] = future.get();
        // Use templates instead.
        std::cout << "Date Time: " << yearMonth << std::endl;
        std::cout << "Avg windspeed: " << stats.avgWindspeed << std::endl;
        std::cout << "Avg power output: " << stats.avgPowerOutput << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "\nTotal program time: " << elapsed.count() << " ms" << std::endl;

    return 0;
}
