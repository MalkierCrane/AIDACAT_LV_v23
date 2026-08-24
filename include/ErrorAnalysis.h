#ifndef ERRORANALYSIS_H
#define ERRORANALYSIS_H

// Šeit tiek iekļautas kopīgās struktūras, statistikas palīgfunkcijas, teksta tips un saraksta tips
#include "Types.h"
#include "StatsUtil.h"
#include <string>
#include <vector>

using namespace std;

/* Šī struktūra glabā vienu novērtēto pāri - attēlu, references un kandidāta tekstu, un metrikas
To izmanto experiment režīms, lai savāktu visus rezultātus pirms kļūdu analīzes veikšanas */
struct ScoredPair {
    string imageName;        // Attēla nosaukums
    string referenceText;    // Reference teksts
    string candidateText;    // Kandidāta (testējamais) teksts
    MetricResult metrics;    // Aprēķinātās metrikas šim pārim
};

/* Šī klase veic detalizētu kļūdu analīzi pēc tam, kad experiment režīms ir izgājis cauri visam (vai lielai daļai) Flickr8k datu kopai. Iepriekšējā programmas versijā experiment
režīms tikai izdrukāja vidējo score - tagad papildus rēķinu sadalījumu, sliktākos un labākos gadījumus, un korelāciju starp metrikām. */
class ErrorAnalysis {
private:
    StatsUtil statsUtil; // Statistikas palīgobjekts

    // Saskaitu, cik pāru iekrīt katrā no 5 līmeņu kategorijām, fiksētā secībā
    vector<int> countLevelBuckets(vector<ScoredPair> pairs);

    // Formatēju visu atskaiti par tekstu, ko pēc tam ierakstu failā
    string formatReport(vector<ScoredPair> pairs, vector<ScoredPair> worstPairs, vector<ScoredPair> bestPairs, int topN);

public:
    // Šeit veicu pilnu kļūdu analīzi un ierakstu rezultātu norādītajā failā
    void runAnalysis(vector<ScoredPair> pairs, string outputPath, int topN);
};

#endif
