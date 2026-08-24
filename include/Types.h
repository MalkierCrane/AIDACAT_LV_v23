#ifndef TYPES_H
#define TYPES_H

// Šeit tiek iekļauts teksta tips, saraksta tips, vārdnīcas tipu korpusa statistikai
#include <string>
#include <vector>
#include <map>

using namespace std;

// Šī struktūra glabā vienu attēlu, kas atrasts tīmekļa lapā
struct ImageItem {
    string pageUrl;        // Šī ir lapas adrese, kur attēls tika atrasts
    string src;            // Attēla src atribūts no HTML
    string imageUrl;       // Pilnā attēla adrese
    string localPath;      // Lokālais ceļš, kur attēls ir saglabāts
    string altText;        // Alternatīvais teksts no alt atribūta
    string note;           // Piezīme, piemēram, ja alt teksts nav atrasts
    bool wrappedInAnchor;  // True, ja img tags atrodas iekš <a> saites
    string purpose;        // Attēla funkcija: decorative / informative / functional / complex
};

// Šī struktūra glabā vienu Flickr8k attēlu un tā aprakstus
struct FlickrItem {
    string imageName;          // Attēla faila nosaukums
    vector<string> captions;   // Cilvēku veidotie apraksti šim attēlam, sakārtoti pēc #0..#4
};

/* Šī struktūra glabā vienu svaru profilu, kas nosaka, cik liela nozīme ir katrai metrikai.
Atkarībā no attēla funkcijas (decorative/informative/functional/complex) es izmantoju citu profilu. */
struct WeightProfile {
    string name;          // Profila nosaukums, piemēram "informative"
    double wPrecision;    // Precision svars
    double wRecall;       // Recall svars
    double wF1;           // F1 svars
    double wBleu;         // BLEU svars
    double wMeteor;       // METEOR svars
    double wRougeL;       // ROUGE-L svars
    double wCider;        // CIDER svars
    double wSemantic;     // Semantiskās līdzības svars
};

// Šī struktūra glabā korpusa statistiku, ko izmanto CIDER TF-IDF aprēķinam. documentFrequency saka, cik references aprakstos parādās katrs n-grams.
struct CorpusStats {
    map<string, int> documentFrequency;   // n-grama atslēga nozīmē, cik dokumentos tas parādās.
    int documentCount = 0;                // Kopējais references aprakstu skaits korpusā. Sākas ar 0, kamēr korpuss nav ielādēts.
};

// Šī struktūra glabā metrikas rezultātu
struct MetricResult {
    double precision;      // Vārdu precizitāte
    double recall;         // Vārdu pārklājums no references apraksta
    double f1;             // Precision un recall kopējais rādītājs
    double bleu;           // Vienkāršota BLEU-4 metrika
    double meteor;         // METEOR metrika ar reālu saskaņošanu un fragmentācijas sodu
    double rougeL;         // ROUGE-L metrika
    double cider;          // CIDER metrika, ja pieejama korpusa statistika - ar TF-IDF svariem
    double semantic;       // Semantiskā līdzība no sentence-transformers modeļa, 0 līdz 1
    double finalScore;     // Gala vērtējums ir no 0 līdz 100
    string level;          // Tekstiska kategorija angliski: insufficient, weak, average, good, excellent
    string purpose;        // Attēla funkcija, ja tā tika noteikta, citādi "n/a"
};

#endif
