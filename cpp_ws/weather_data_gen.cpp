#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> cities = {
        "Aalborg", "Odense", "Aarhus", "Koebenhavn", "Esbjerg",
        "Randers", "Kolding", "Horsens", "Vejle", "Roskilde"
    };

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> cityIndex(0, cities.size() - 1);
    std::uniform_real_distribution<double> tempDist(-10.0, 30.0);
    std::uniform_real_distribution<double> pressureDist(970.0, 1040.0);
    std::uniform_real_distribution<double> humidityDist(20.0, 100.0);

    std::ofstream file("weather_data.csv");
    if (!file) {
        std::cerr << "Kunne ikke oprette filen!" << std::endl;
        return 1;
    }

    file << "city,temperature,pressure,humidity\n";

    const int numRows = 200;
    for (int i = 0; i < numRows; ++i) {
        std::string city = cities[cityIndex(gen)];
        double temperature = tempDist(gen);
        double pressure = pressureDist(gen);
        double humidity = humidityDist(gen);

        file << city << ","
             << temperature << ","
             << pressure << ","
             << humidity << "\n";
    }

    file.close();
    std::cout << "Genererede " << numRows << " raekker til weather_data.csv" << std::endl;

    return 0;
}