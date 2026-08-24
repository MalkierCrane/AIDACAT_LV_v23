#ifndef EVALUATION_H
#define EVALUATION_H

/* Šeit tiek iekļautas metrikas, Flickr ielādētājs, Florence izsaukšanas klase, lejupielādētājs, web parseris, semantiskās līdzības klase, svaru profilu klase,
attēla funkcijas klasificētājs, kļūdu analīzes klase, kalibrēšanas klase un teksta tips */
#include "Metrics.h"
#include "FlickrLoader.h"
#include "Florence.h"
#include "Downloader.h"
#include "WebParser.h"
#include "SemanticSimilarity.h"
#include "Coefficients.h"
#include "ImagePurpose.h"
#include "ErrorAnalysis.h"
#include "Calibration.h"
#include <string>

using namespace std;

// Šī ir galvenā klase, kas savieno programmas režīmus
class Evaluation {
private:
    Metrics metrics;                       // Metriku aprēķina objekts
    FlickrLoader flickrLoader;             // Flickr8k faila ielādētājs
    Florence florence;                     // Florence-2 izsaukšanas objekts
    Downloader downloader;                 // HTML un attēlu lejupielādētājs
    WebParser webParser;                   // Vienkāršs HTML parseris
    SemanticSimilarity semanticSimilarity; // Semantiskās līdzības objekts
    Coefficients coefficients;             // Svaru profilu objekts
    ImagePurpose imagePurpose;             // Attēla funkcijas klasificētājs
    ErrorAnalysis errorAnalysis;           // Kļūdu analīzes objekts
    Calibration calibration;               // Kalibrēšanas objekts

    CorpusStats corpusStats;             // Kešotā korpusa statistika CIDER metrikai
    bool corpusStatsLoaded = false;      // True, ja mēģināju ielādēt korpusu (neatkarīgi no rezultāta)
    bool corpusStatsAvailable = false;   // True, ja korpuss tika veiksmīgi ielādēts un statistika ir derīga

    void makeOutputDir();                           // Izveidoju output mapi
    string csvSafe(string value);                   // Sagatavoju tekstu CSV failam
    string joinPath(string folder, string file);    // Salieku mapes un faila ceļu
    string makeFileName(int index);                 // Izveidoju lokālu attēla faila nosaukumu
    void printMetric(MetricResult result);          // Izvadu rezultātu ekrānā

    /* Šeit es ielādēju korpusa statistiku no data/Flickr8k_text/Flickr8k.token.txt, ja tas vēl nav izdarīts.
    Ja fails nav atrodams, tiek izvadīts brīdinājums un ļauju programmai turpināt bez korpusa CIDER. */
    void ensureCorpusStats();

    /* Šeit aprēķinu metrikas vienam pārim - izvēlos korpusa vai bez-korpusa CIDER versiju atkarībā
    no tā, vai ensureCorpusStats izdevās, un pieskaitu semantisko līdzību ar doto profilu */
    MetricResult scoreTexts(string referenceText, string candidateText, WeightProfile profile);

public:
    void setPythonExe(string path);             // Tiek uzstādīts Python ceļš gan Florence, gan SemanticSimilarity objektiem
    void setModelName(string model);            // Tiek uzstādīts Florence modeļa nosaukums
    void setSemanticModelName(string model);    // Tiek uzstādīts sentence-transformers modeļa nosaukums

    void runExperiment(string tokenFile, int limit, int topN = 10);                    // Flickr8k validācijas režīms ar kļūdu analīzi
    void runSingle(string imagePath, string altText, bool classifyComplexity = false); // Viena attēla praktiska pārbaude
    void runWeb(string url, int limit, bool classifyComplexity = false);               // Reālas tīmekļa lapas režīms

    // šis ir koeficientu kalibrēšanas režīms - salīdzina manu score ar ExpertAnnotations.txt un, ja norādīts, arī ar CrowdFlowerAnnotations.txt
    void runCalibration(string expertFile, string tokenFile, int limit, bool useCrowdflower, string crowdflowerFile);
};

#endif
