// Šeit tiek iekļauta StatsUtil klases deklarācija un matemātikas funkcijas
#include "StatsUtil.h"
#include <cmath>

using namespace std;

/* Šī funkcija sakārto skaitļu sarakstu augošā secībā ar vienkāršu insertion sort - saraksti šeit
nav pārāk lieli (mediānai un top-N vajag tikai mazus sarakstus, nevis visu 32000 pāru sarakstu uzreiz) */
vector<double> StatsUtil::sortAscending(vector<double> values) {

    vector<double> result = values;  // Te es strādāju ar kopiju, oriģinālais saraksts paliek neskarts

    // Ārējais cikls iet cauri visiem elementiem
    for (int i = 1; i < (int)result.size(); i++) {
        double current = result[i];
        int j = i - 1;

        // Iekšējais cikls pabīda lielākos elementus par vienu vietu uz priekšu
        while (j >= 0 && result[j] > current) {
            result[j + 1] = result[j];
            j--;
        }

        result[j + 1] = current;
    }

    return result;
}

// Šī funkcija aprēķina vidējo vērtību
double StatsUtil::calcMean(vector<double> values) {
    if (values.size() == 0) {
        return 0.0;
    }

    double sum = 0.0;

    for (int i = 0; i < (int)values.size(); i++) {
        sum += values[i];
    }

    return sum / (double)values.size();
}

// Šī funkcija aprēķina standartnovirzi ap doto vidējo vērtību
double StatsUtil::calcStdDev(vector<double> values, double meanValue) {
    // Ja ir mazāk par 2 vērtībām, standartnovirzi nevar jēgpilni rēķināt
    if (values.size() < 2) {
        return 0.0;
    }

    double sumSquares = 0.0;

    for (int i = 0; i < (int)values.size(); i++) {
        double diff = values[i] - meanValue;
        sumSquares += diff * diff;
    }

    return sqrt(sumSquares / (double)values.size());
}

// Šī funkcija aprēķina visu statistiku uzreiz vienam sarakstam
MetricStats StatsUtil::calcStats(vector<double> values) {

    MetricStats stats;

    if (values.size() == 0) {
        stats.mean = 0.0;
        stats.median = 0.0;
        stats.stdDev = 0.0;
        stats.minVal = 0.0;
        stats.maxVal = 0.0;
        return stats;
    }

    stats.mean = calcMean(values);
    stats.stdDev = calcStdDev(values, stats.mean);

    // Šeit es sakārtoju sarakstu, lai no tā varētu atrast mediānu un min/max
    vector<double> sorted = sortAscending(values);
    stats.minVal = sorted[0];
    stats.maxVal = sorted[sorted.size() - 1];

    // Pāra garumam mediāna ir divu vidus elementu vidējais, nepāra garumam - vidus elements pats
    int middle = (int)sorted.size() / 2;
    if (sorted.size() % 2 == 0) {
        stats.median = (sorted[middle - 1] + sorted[middle]) / 2.0;
    }
    else {
        stats.median = sorted[middle];
    }

    return stats;
}

/* Šī funkcija aprēķina Pīrsona korelāciju starp diviem vienāda garuma sarakstiem - tā rāda, cik
lielā mērā finalScore sakrīt ar citu vērtību (piemēram, eksperta score). */
double StatsUtil::pearsonCorrelation(vector<double> x, vector<double> y) {

    // Ja saraksti nav vienāda garuma vai ir pārāk mazi, korelāciju nevar rēķināt
    if (x.size() != y.size() || x.size() < 2) {
        return 0.0;
    }

    double meanX = calcMean(x);
    double meanY = calcMean(y);

    double numerator = 0.0;
    double sumSquareX = 0.0;
    double sumSquareY = 0.0;

    // Šis cikls saskaita skaitītāju un abus saucēja gabalus visiem pāriem
    for (int i = 0; i < (int)x.size(); i++) {
        double diffX = x[i] - meanX;
        double diffY = y[i] - meanY;

        numerator += diffX * diffY;
        sumSquareX += diffX * diffX;
        sumSquareY += diffY * diffY;
    }

    double denominator = sqrt(sumSquareX) * sqrt(sumSquareY);

    // Ja saucējs ir 0, kādam no sarakstiem nav izkliedes, korelāciju nevar rēķināt
    if (denominator == 0.0) {
        return 0.0;
    }

    return numerator / denominator;
}
