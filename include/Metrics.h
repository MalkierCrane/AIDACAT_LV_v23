#ifndef METRICS_H
#define METRICS_H

// Šeit tiek iekļautas kopīgās datu struktūras, teksta apstrādes klase, teksta tips, saraksta tips un vārdnīcas tips n-gramiem
#include "Types.h"
#include "TextProc.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

// Šī klase aprēķina aprakstu līdzības metrikas
class Metrics {
private:
    TextProc textProc;                                              // Teksta sagatavošanas objekts
    double safeDivide(double a, double b);                          // Te ir droša dalīšana ar nulles pārbaudi
    map<string, int> countWords(vector<string> words);              // Tas saskaita vārdus
    map<string, int> countNgrams(vector<string> words, int n);      // Tas saskaita n-gramus
    int countOverlap(map<string, int> a, map<string, int> b);       // Tas saskaita sakritības
    int lcsLength(vector<string> a, vector<string> b);              // Šī ir garākā kopīgā secība
    double calcPrecision(vector<string> ref, vector<string> cand);  // Precision metrika
    double calcRecall(vector<string> ref, vector<string> cand);     // Recall metrika
    double calcBleu(vector<string> ref, vector<string> cand);       // Vienkāršots BLEU-4

    // METEOR ar reālu vārdu saskaņošanu (exact + stem/sinonīms) un fragmentācijas sodu
    double calcMeteor(vector<string> ref, vector<string> cand);
    int countMeteorChunks(vector<int> candMatchRefIndex);        // Šis saskaita saskaņoto vārdu ķēdes (chunks)

    double calcRougeL(vector<string> ref, vector<string> cand);  // ROUGE-L.
    double calcCider(vector<string> ref, vector<string> cand);   // CIDER bez korpusa (neapstrādātu skaitu versija)

    /* CIDER ar korpusa TF-IDF svariem - vajag CorpusStats, ko uzbūvē buildCorpusStats.
PIEZĪME PAR & ŠEIT: pārējā programmā es visur nododu parametrus pēc vērtības (kopējot), bet CorpusStats var saturēt simtiem tūkstošu ierakstu (visa Flickr8k korpusa n-grami).
Pirmajā versijā šeit es arī nodevu pēc vērtības, un pilna eksperimenta palaišana kļuva absurdi lēna (~170ms uz katru pāri, tikai tāpēc, ka katru reizi kopējās visa milzīgā
vārdnīca). Ar & (atsauci) šeit nekas netiek kopēts, tikai padota adrese - tas atrisināja problēmu. Tas ir vienīgais izņēmums visā programmā, kur apzināti lietoju atsauci. */
    double calcCiderCorpus(vector<string> ref, vector<string> cand, const CorpusStats& corpusStats);
    double cosineSimilarity(map<string, double> vecA, map<string, double> vecB);    // Šī ir kopīgā kosinusa līdzības funkcija

    string makeLevel(double score);     // Šis pārvērš score kategorijā (angliski)

public:
    MetricResult evaluate(string referenceText, string candidateText);                                  // Šitais salīdzina bez korpusa statistikas
    MetricResult evaluate(string referenceText, string candidateText, const CorpusStats& corpusStats);  // Šitais salīdzina ar korpusa CIDER (& tāpat kā calcCiderCorpus - skat. piezīmi tur)
    MetricResult evaluateMulti(vector<string> references, string candidate);                            // Šitais salīdzina kandidātu ar vairākām references vērtībām

    /* Te es pievienoju semantisko līdzību rezultātam un pārrēķinu gala score pēc dotā svaru profila
    Šī ir vienīgā vieta visā programmā, kur tiek saskaitīts gala svērtais score */
    MetricResult attachSemantic(MetricResult result, double semanticScore, WeightProfile profile);

    // Šitais uzbūvē korpusa statistiku (n-gramu dokumentu biežumu) no liela references aprakstu saraksta
    CorpusStats buildCorpusStats(vector<string> referenceTexts);
};

#endif
