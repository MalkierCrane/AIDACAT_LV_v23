#ifndef STATSUTIL_H
#define STATSUTIL_H

// Iekļauju saraksta tipu
#include <vector>

using namespace std;

// Šī struktūra glabā vienkāršu statistiku par vienu skaitļu sarakstu
struct MetricStats {
    double mean;        // Vidējā vērtība
    double median;      // Mediāna
    double stdDev;      // Standartnovirze
    double minVal;      // Mazākā vērtība
    double maxVal;      // Lielākā vērtība
};

// Šī klase satur vienkāršas statistikas funkcijas, ko kopā izmanto ErrorAnalysis un Calibration
class StatsUtil {
public:
    vector<double> sortAscending(vector<double> values);           // Šis sakārto sarakstu augošā secībā (insertion sort)
    double calcMean(vector<double> values);                        // Šis aprēķina vidējo vērtību
    double calcStdDev(vector<double> values, double meanValue);    // Šis aprēķina standartnovirzi 
    MetricStats calcStats(vector<double> values);                  // Šis aprēķina visu statistiku uzreiz
    double pearsonCorrelation(vector<double> x, vector<double> y); // Šis aprēķina Pīrsona korelāciju starp diviem sarakstiem
};

#endif
