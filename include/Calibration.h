#ifndef CALIBRATION_H
#define CALIBRATION_H

// Te tiek iekļautas kopīgās struktūras, metriku aprēķinu klase, semantiskās līdzības klase, statistikas palīgfunkcijas, teksta tips un saraksta tips
#include "Types.h"
#include "Metrics.h"
#include "SemanticSimilarity.h"
#include "StatsUtil.h"
#include <string>
#include <vector>

using namespace std;

/* Struktūra glabā vienu rindu no ExpertAnnotations.txt vai CrowdFlowerAnnotations.txt.
SVARĪGI: katra rinda satur DIVUS attēlu nosaukumus, nevis vienu - judgedImageName ir attēls, kuru vērtē, un candidateImageName#candidateIndex ir tā apraksta avots, kuru
eksperti novērtēja - VAIRUMĀ gadījumu tas ir cits attēls, nevis tas pats (skat. plāna piezīmi par 5664/5822 Expert rindām un 47109/47830 CrowdFlower rindām, kur šie
divi attēli atšķiras - tas ir sākotnējā raksta "ranking task" datu struktūra). */
struct ExpertRow {
    string judgedImageName;      // Attēls, kuru vērtē - tā paša cilvēku apraksti kalpo par mūsu references tekstu
    string candidateImageName;   // Attēls, no kura nāk vērtētais apraksts
    int candidateIndex;           // Apraksta indekss (#0..#4) candidateImageName attēlam
    double groundTruthScore;      // Eksperta vērtējums, jau pārrēķināts uz 0-100 skalu
};

// Struktūra glabā ielādēto rindu sarakstu kopā ar to, cik rindas nācās izlaist (bojāts formāts)
struct LoadedAnnotationRows {
    vector<ExpertRow> rows;  // Derīgās, ielādētās rindas
    int skippedCount;        // Cik rindas tika izlaistas parsēšanas dēļ
};

/* Šī klase salīdzina manis paša aprēķinātos score ar reāliem cilvēku vērtējumiem no ExpertAnnotations.txt / CrowdFlowerAnnotations.txt, un mēģina atrast svaru profilu,
kas labāk sakrīt ar cilvēku vērtējumu. Tas NAV automātisks pielāgotājs, kas pārraksta Coefficients.cpp - tas tikai uzraksta ieteikumu atskaites failā, kuru persona var
pēc izvēles pati ielikt Coefficients.cpp failā. */
class Calibration {
private:
    StatsUtil statsUtil; // Statistikas palīgobjekts

    LoadedAnnotationRows loadExpertAnnotations(string filePath, int limit);        // Ielādē ExpertAnnotations.txt
    LoadedAnnotationRows loadCrowdflowerAnnotations(string filePath, int limit);   // Ielādē CrowdFlowerAnnotations.txt

    // Palīgfunkcijas, kas pārvērš WeightProfile par vienkāršu 8 skaitļu sarakstu un atpakaļ - tas atvieglo svaru meklēšanas ciklu, kur ir jāiet cauri visiem 8 svariem pēc kārtas
    vector<double> profileToArray(WeightProfile profile);
    WeightProfile arrayToProfile(vector<double> values, string name);
    vector<double> normalizeWeightArray(vector<double> values); // Padara, lai 8 skaitļi summējas 1.0

    // Šis pārrēķina final_score visiem rezultātiem ar doto profilu un atgriež korelāciju ar cilvēku vērtējumu
    double evaluateProfileCorrelation(vector<MetricResult> ourResults, vector<double> groundTruthScores, WeightProfile profile);

    /* Meklē uzlabotu svaru profilu ar koordinātu kāpšanas (coordinate ascent / hill-climbing) metodi -
    pilna režģa meklēšana pa visām 8 dimensijām nebūtu praktiski izpildāma (par daudz kombināciju) */
    WeightProfile hillClimbSearch(vector<MetricResult> ourResults, vector<double> groundTruthScores, WeightProfile startProfile);

    // Uzrakstu gala atskaiti par kalibrēšanu
    void writeReport(string outputPath, string sourceLabel, int loadedCount, int skippedRowsCount, int skippedLookupCount,
                      vector<MetricResult> ourResults, vector<double> groundTruthScores,
                      WeightProfile startProfile, WeightProfile discoveredProfile);

public:
    /* Veicu pilnu kalibrēšanas ciklu vienam avota failam (Expert vai CrowdFlower). flickrItems jābūt ielādētam BEZ limita (limit=0), lai visi attēli, uz kuriem norāda
    annotāciju fails, būtu atrodami. metricsEngine un semanticSimilarity jau ir sagatavoti (Python ceļš un modeļa nosaukums uzstādīti) - Calibration tos tikai izmanto. */
    void runCalibration(string annotationFile, vector<FlickrItem> flickrItems, CorpusStats corpusStats,
                         string outputPath, int limit, bool useCrowdflower,
                         Metrics metricsEngine, SemanticSimilarity semanticSimilarity);
};

#endif
