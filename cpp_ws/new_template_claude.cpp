#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>

// Struct der repræsenterer én vejrmåling (én time)
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

    WeatherData(std::tm datetime, float rain, float rainMinutes, float avgTemp, float minTemp, float maxTemp,
                float avgWindspeed, float maxWindspeed, float pressure, int cloud, int humidity, int sun, int windDir)
        : datetime(datetime), rain(rain), rainMinutes(rainMinutes), avgTemp(avgTemp), minTemp(minTemp), maxTemp(maxTemp),
          avgWindspeed(avgWindspeed), maxWindspeed(maxWindspeed), pressure(pressure), cloud(cloud),
          humidity(humidity), sun(sun), windDir(windDir) {}
};

// Struct der samler statistik (min/max/avg) for én periode (fx én måned)
struct WeatherStats{
    float minavgTemp, maxavgTemp, avgavgTemp;
    float minminTemp, maxminTemp, avgminTemp;
    float minmaxTemp, maxmaxTemp, avgmaxTemp;
    float minPressure, maxPressure, avgPressure;
    float minHumidity, maxHumidity, avgHumidity;
    float minRain, maxRain, avgRain;
    float minrainMinutes, maxrainMinutes, avgrainMinutes;
    float minCloud, maxCloud, avgCloud;
    float minSun, maxSun, avgSun;
    float minwindDir, maxwindDir, avgwindDir;
};

// Map: år-måned (fx "2020-01") -> alle målinger i den måned
std::unordered_map<std::string, std::vector<WeatherData>> WeatherDataMap;

// Generisk gennemsnitsberegning
template<typename T, typename Func>
double Average(const std::vector<T>& data, Func getValue){
    double sum = 0.0;
    for (const auto& item : data){
        sum += getValue(item);
    }
    return sum / data.size();
}

// Generisk min/max-beregning
template<typename T, typename Func>
std::pair<double, double> MinMax(const std::vector<T>& data, Func getValue){
    auto [minIt, maxIt] = std::minmax_element(data.begin(), data.end(),
        [getValue](const T& a, const T& b){
            return getValue(a) < getValue(b);
        });
    return {getValue(*minIt), getValue(*maxIt)};
}

int main(){
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

        if (ss.fail() || dtStream.fail()){
            std::cerr << "Advarsel: ugyldig data på linje " << lineNumber << " - springes over\n";
            continue;
        }

        weatherData.emplace_back(dt, rain, rain_minutes, average_temperature, minimum_temperature, maximum_temperature,
                                  average_windspeed, maximum_windspeed, pressure, cloud, humidity, sun, wind_dir);
    }

    std::cout << "Indlæst " << weatherData.size() << " gyldige målinger.\n";

    // Gruppér efter år-måned
    for (const auto& data : weatherData){
        char keyBuffer[8];
        std::tm dtCopy = data.datetime;
        std::strftime(keyBuffer, sizeof(keyBuffer), "%Y-%m", &dtCopy);
        std::string key(keyBuffer);
        WeatherDataMap[key].push_back(data);
    }

    // Beregn og print statistik pr. måned
    for (const auto& [yearMonth, data] : WeatherDataMap){
        WeatherStats stats;

        stats.avgavgTemp = Average(data, [](const WeatherData& wd){ return wd.avgTemp; });
        stats.minavgTemp = MinMax(data, [](const WeatherData& wd){ return wd.avgTemp; }).first;
        stats.maxavgTemp = MinMax(data, [](const WeatherData& wd){ return wd.avgTemp; }).second;

        stats.avgminTemp = Average(data, [](const WeatherData& wd){ return wd.minTemp; });
        stats.minminTemp = MinMax(data, [](const WeatherData& wd){ return wd.minTemp; }).first;
        stats.maxminTemp = MinMax(data, [](const WeatherData& wd){ return wd.minTemp; }).second;

        stats.avgmaxTemp = Average(data, [](const WeatherData& wd){ return wd.maxTemp; });
        stats.minmaxTemp = MinMax(data, [](const WeatherData& wd){ return wd.maxTemp; }).first;
        stats.maxmaxTemp = MinMax(data, [](const WeatherData& wd){ return wd.maxTemp; }).second;

        stats.avgPressure = Average(data, [](const WeatherData& wd){ return wd.pressure; });
        stats.minPressure = MinMax(data, [](const WeatherData& wd){ return wd.pressure; }).first;
        stats.maxPressure = MinMax(data, [](const WeatherData& wd){ return wd.pressure; }).second;

        stats.avgHumidity = Average(data, [](const WeatherData& wd){ return wd.humidity; });
        stats.minHumidity = MinMax(data, [](const WeatherData& wd){ return wd.humidity; }).first;
        stats.maxHumidity = MinMax(data, [](const WeatherData& wd){ return wd.humidity; }).second;

        stats.avgRain = Average(data, [](const WeatherData& wd){ return wd.rain; });
        stats.minRain = MinMax(data, [](const WeatherData& wd){ return wd.rain; }).first;
        stats.maxRain = MinMax(data, [](const WeatherData& wd){ return wd.rain; }).second;

        std::cout << "\nPeriode: " << yearMonth << " (" << data.size() << " målinger)" << std::endl;
        std::cout << "  Avg Temp - Min: " << stats.minavgTemp << ", Max: " << stats.maxavgTemp << ", Avg: " << stats.avgavgTemp << std::endl;
        std::cout << "  Max Temp - Min: " << stats.minmaxTemp << ", Max: " << stats.maxmaxTemp << ", Avg: " << stats.avgmaxTemp << std::endl;
        std::cout << "  Min Temp - Min: " << stats.minminTemp << ", Max: " << stats.maxminTemp << ", Avg: " << stats.avgminTemp << std::endl;
        std::cout << "  Pressure - Min: " << stats.minPressure << ", Max: " << stats.maxPressure << ", Avg: " << stats.avgPressure << std::endl;
        std::cout << "  Humidity - Min: " << stats.minHumidity << ", Max: " << stats.maxHumidity << ", Avg: " << stats.avgHumidity << std::endl;
        std::cout << "  Rain     - Min: " << stats.minRain << ", Max: " << stats.maxRain << ", Avg: " << stats.avgRain << std::endl;
        std::cout << "Global " ;
    }

    return 0;
}