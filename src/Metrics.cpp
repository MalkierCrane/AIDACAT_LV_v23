/* Šeit tiek iekļauta Metrics klases deklarācija, svaru profili (lai varētu iegūt noklusējuma profilu evaluate() funkcijā), 
matemātikas funkcijas un algoritmu funkcijas. */
#include "Metrics.h"
#include "Coefficients.h"
#include <cmath>
#include <algorithm>

using namespace std;

// Šī funkcija droši dala divus skaitļus
double Metrics::safeDivide(double a, double b) {
    if (b == 0) {
        return 0;
    }

    return a / b;
}

// Šī funkcija saskaita vārdus
map<string, int> Metrics::countWords(vector<string> words) {

    map<string, int> result;

    for (int i = 0; i < (int)words.size(); i++) {
        result[words[i]]++;
    }

    return result;
}

// Šī funkcija saskaita n-gramus
map<string, int> Metrics::countNgrams(vector<string> words, int n) {

    map<string, int> result;

    if ((int)words.size() < n) {
        return result;
    }

    // Šis cikls veido katru n-gramu, saliekot n pēc kārtas ejošus vārdus kopā ar "_" atdalītāju
    for (int i = 0; i <= (int)words.size() - n; i++) {
        string key = "";

        for (int j = 0; j < n; j++) {
            if (j > 0) {
                key += "_";
            }
            key += words[i + j];
        }

        result[key]++;
    }

    return result;
}

// Šī funkcija saskaita sakritības starp divām vārdnīcām
int Metrics::countOverlap(map<string, int> a, map<string, int> b) {

    int count = 0;

    // Šis cikls iet cauri pirmajai vārdnīcai un pieskaita mazāko no abu vārdnīcu skaitiem katrai kopīgai atslēgai
    for (auto it = a.begin(); it != a.end(); it++) {
        string key = it->first;

        if (b.find(key) != b.end()) {
            count += min(it->second, b[key]);
        }
    }

    return count;
}

// Šī funkcija aprēķina garāko kopīgo vārdu secību (LCS) ar standarta dinamiskās programmēšanas algoritmu
int Metrics::lcsLength(vector<string> a, vector<string> b) {

    vector<vector<int>> dp(a.size() + 1, vector<int>(b.size() + 1, 0));

    for (int i = 1; i <= (int)a.size(); i++) {
        for (int j = 1; j <= (int)b.size(); j++) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            }
            else {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }

    return dp[a.size()][b.size()];
}

// Šī funkcija aprēķina precision
double Metrics::calcPrecision(vector<string> ref, vector<string> cand) {

    map<string, int> refMap = countWords(ref);
    map<string, int> candMap = countWords(cand);
    int overlap = countOverlap(candMap, refMap);

    return safeDivide(overlap, (double)cand.size());
}

// Šī funkcija aprēķina recall
double Metrics::calcRecall(vector<string> ref, vector<string> cand) {

    map<string, int> refMap = countWords(ref);
    map<string, int> candMap = countWords(cand);
    int overlap = countOverlap(refMap, candMap);

    return safeDivide(overlap, (double)ref.size());
}

// Šī funkcija aprēķina vienkāršotu BLEU-4
double Metrics::calcBleu(vector<string> ref, vector<string> cand) {
    if (cand.size() == 0) {
        return 0;
    }

    double logSum = 0;

    // Šis cikls aprēķina 1 līdz 4 gramu precision un summē to logaritmus ģeometriskajam vidējam
    for (int n = 1; n <= 4; n++) {
        map<string, int> refN = countNgrams(ref, n);
        map<string, int> candN = countNgrams(cand, n);

        int total = 0;
        for (auto it = candN.begin(); it != candN.end(); it++) {
            total += it->second;
        }

        int overlap = countOverlap(candN, refN);

        // Te es veicu Smoothing (+1/+1), lai īsiem tekstiem nebūtu tūlītēja nulle
        double p = safeDivide((double)overlap + 1.0, (double)total + 1.0);
        logSum += log(p);
    }

    double geoMean = exp(logSum / 4.0);

    // Brevity penalty - sods, ja kandidāts ir īsāks par references tekstu
    double bp = 1.0;
    if (cand.size() < ref.size()) {
        bp = exp(1.0 - safeDivide((double)ref.size(), (double)cand.size()));
    }

    return bp * geoMean;
}

/* Šī funkcija saskaita saskaņoto vārdu ķēžu (chunks) skaitu METEOR fragmentācijas sodam. candMatchRefIndex[i] saka, ar kuru reference vārda indeksu 
i-tais kandidāta vārds ir saskaņots, vai -1, ja tas nav saskaņots vispār. Ķēde ir virkne pēc kārtas saskaņotu vārdu, kur reference
indeksi arī iet secīgi pa vienam uz priekšu - tas nozīmē, ka kandidāts un reference šajā vietā "saka to pašu tajā pašā secībā". 
Vienkāršības labad es uzskatu, ka nesaskaņots kandidāta vārds vienmēr pārtrauc iepriekšējo ķēdi. */
int Metrics::countMeteorChunks(vector<int> candMatchRefIndex) {

    int chunkCount = 0;
    int previousRefIndex = -2;  // -2 nekad nesakrīt ar īstu indeksu

    // Šis cikls iet cauri visiem kandidāta vārdiem secībā un skaita, kur sākas jauna ķēde
    for (int i = 0; i < (int)candMatchRefIndex.size(); i++) {
        int refIndex = candMatchRefIndex[i];

        // Ja vārds nav saskaņots, tas pārtrauc esošo ķēdi un nesāk jaunu
        if (refIndex == -1) {
            previousRefIndex = -2;
            continue;
        }

        // Ja šis saskaņojums neturpina iepriekšējo secīgi, sākas jauna ķēde
        if (refIndex != previousRefIndex + 1) {
            chunkCount++;
        }

        previousRefIndex = refIndex;
    }

    return chunkCount;
}

/* Šī funkcija aprēķina METEOR ar reālu vārdu saskaņošanu divos posmos un fragmentācijas sodu. Iepriekšējā programmas versijā šeit bija tikai 
formula 10pr/(r+9p) uz parasta bag-of-words precision/recall, bez īstas saskaņošanas - tagad vispirms saskaņoju vārdus (exact, tad stem/sinonīms), un 
tikai tad rēķinu formulu no reālā saskaņoto vārdu skaita. */
double Metrics::calcMeteor(vector<string> ref, vector<string> cand) {
    if (ref.size() == 0 || cand.size() == 0) {
        return 0;
    }

    vector<int> candMatchRefIndex((int)cand.size(), -1);  // Te katram kandidāta vārdam - saskaņotā reference vārda indekss
    vector<bool> refUsed((int)ref.size(), false);         // Šis atzīmē, vai reference vārds jau ir izmantots saskaņošanā

    // 1. posms: precīza vārdu saskaņošana (exact match), vārdi jau ir normalizēti tokenize solī
    for (int i = 0; i < (int)cand.size(); i++) {
        for (int j = 0; j < (int)ref.size(); j++) {     // Šeit es meklēju pirmo vēl neizmantoto reference vārdu, kas precīzi sakrīt
            if (refUsed[j]) {
                continue;
            }

            if (cand[i] == ref[j]) {
                candMatchRefIndex[i] = j;
                refUsed[j] = true;
                break;
            }
        }
    }

    /* 2. posms: stem/sinonīmu saskaņošana neizmantotajiem vārdiem, izmantojot to pašu TextProc::normalizeWord vārdnīcu, ko jau lieto tokenize. 
Tas aizstāj īstu WordNet sinonīmu meklēšanu ar manu mazo, jau esošo sinonīmu tabulu. */
    for (int i = 0; i < (int)cand.size(); i++) {
        if (candMatchRefIndex[i] != -1) {
            continue;
        }

        string candStem = textProc.normalizeWord(cand[i]);

        // Šeit meklēju vēl neizmantotu reference vārdu ar tādu pašu normalizēto formu
        for (int j = 0; j < (int)ref.size(); j++) {
            if (refUsed[j]) {
                continue;
            }

            string refStem = textProc.normalizeWord(ref[j]);

            if (candStem == refStem) {
                candMatchRefIndex[i] = j;
                refUsed[j] = true;
                break;
            }
        }
    }

    // Te es saskaitu, cik kandidāta vārdu kopumā ir saskaņoti
    int matchCount = 0;
    for (int i = 0; i < (int)candMatchRefIndex.size(); i++) {
        if (candMatchRefIndex[i] != -1) {
            matchCount++;
        }
    }

    if (matchCount == 0) {
        return 0;
    }

    // Precision un recall balstās uz reālo saskaņoto vārdu skaitu, nevis bag-of-words pārklājumu
    double p = safeDivide((double)matchCount, (double)cand.size());
    double r = safeDivide((double)matchCount, (double)ref.size());

    /* F-mean formula dod lielāku svaru recall pusei - šī formula pati par sevi bija pareiza jau iepriekšējā versijā, 
problēma bija tikai tajā, ka p un r nenāca no īstas saskaņošanas.*/
    double fMean = safeDivide(10.0 * p * r, r + 9.0 * p);

    int chunkCount = countMeteorChunks(candMatchRefIndex);

    /* Fragmentācijas attiecība - jo vairāk atsevišķu īsu ķēžu salīdzinājumā ar kopējo saskaņoto skaitu, 
jo vairāk kandidāts "pārkārto" vārdus salīdzinājumā ar references secību. */
    double fragmentation = safeDivide((double)chunkCount, (double)matchCount);
    double penalty = 0.5 * fragmentation * fragmentation * fragmentation;  // Tas ir standarta METEOR fragmentācijas sods

    double score = fMean * (1.0 - penalty);
    if (score < 0.0) {
        score = 0.0;
    }

    return score;
}

// Šī funkcija aprēķina ROUGE-L
double Metrics::calcRougeL(vector<string> ref, vector<string> cand) {
    if (ref.size() == 0 || cand.size() == 0) {
        return 0;
    }

    int lcs = lcsLength(ref, cand);
    double p = safeDivide((double)lcs, (double)cand.size());
    double r = safeDivide((double)lcs, (double)ref.size());

    if (p == 0 && r == 0) {
        return 0;
    }

    return safeDivide(2.0 * p * r, p + r);
}

/* Šī funkcija aprēķina kosinusa līdzību starp diviem svērtiem vektoriem. To kopā izmanto gan vecā, neapstrādāto skaitu CIDER versija, gan jaunā, 
ar korpusa TF-IDF svariem, lai matemātika būtu uzrakstīta tikai vienā vietā. */
double Metrics::cosineSimilarity(map<string, double> vecA, map<string, double> vecB) {

    double dot = 0.0;
    double lenA = 0.0;
    double lenB = 0.0;

    for (auto it = vecA.begin(); it != vecA.end(); it++) {
        lenA += it->second * it->second;
    }

    // Šis cikls uzkrāj otrā vektora garumu un, kur atslēga sakrīt ar pirmo vektoru, arī skalāro reizinājumu
    for (auto it = vecB.begin(); it != vecB.end(); it++) {
        lenB += it->second * it->second;

        if (vecA.find(it->first) != vecA.end()) {
            dot += it->second * vecA[it->first];
        }
    }

    return safeDivide(dot, sqrt(lenA) * sqrt(lenB));
}

/* Šī funkcija aprēķina CIDER bez korpusa statistikas - neapstrādātu n-gramu skaitu versija. Šo lieto tikai tad, ja korpusa faila (Flickr8k.token.txt) nevar 
atrast un Evaluation klase nevar uzbūvēt CorpusStats - skat. calcCiderCorpus par īsto, TF-IDF svērto versiju. */
double Metrics::calcCider(vector<string> ref, vector<string> cand) {

    double sum = 0;
    int used = 0;

    // Šis cikls iet cauri 1 līdz 4 gramiem un summē kosinusa līdzību katram n-gramu garumam
    for (int n = 1; n <= 4; n++) {
        map<string, int> refN = countNgrams(ref, n);
        map<string, int> candN = countNgrams(cand, n);

        if (refN.size() == 0 || candN.size() == 0) {
            continue;
        }

        // Šeit es pārvēršu skaitīšanas vārdnīcas par double vektoriem, lai varētu padot cosineSimilarity
        map<string, double> refVec;
        for (auto it = refN.begin(); it != refN.end(); it++) {
            refVec[it->first] = (double)it->second;
        }

        map<string, double> candVec;
        for (auto it = candN.begin(); it != candN.end(); it++) {
            candVec[it->first] = (double)it->second;
        }

        double cosine = cosineSimilarity(refVec, candVec);
        sum += cosine;
        used++;
    }

    return safeDivide(sum, (double)used);
}

/* Šī funkcija aprēķina CIDER ar korpusa TF-IDF svariem. Katram n-gramam piešķiru svaru pēc tā retuma korpusā (IDF) - jo n-grams ir retāk sastopams
korpusā (piemēram, visos Flickr8k aprakstos), jo vairāk tas "saka" par konkrēto attēlu, un jo lielāku svaru tas dabū. 
Bieži sastopami n-grami (piemēram "a man") dabū mazu svaru. */
double Metrics::calcCiderCorpus(vector<string> ref, vector<string> cand, const CorpusStats& corpusStats) {

    double sum = 0.0;
    int used = 0;

    for (int n = 1; n <= 4; n++) {
        map<string, int> refN = countNgrams(ref, n);
        map<string, int> candN = countNgrams(cand, n);

        if (refN.size() == 0 || candN.size() == 0) {
            continue;
        }

        // Te es sagatavoju TF-IDF svērto references vektoru
        map<string, double> refVec;
        for (auto it = refN.begin(); it != refN.end(); it++) {
            int df = 0;

            /* Te es meklēju n-gramu korpusa statistikā VIENU reizi un paturu atrasto vietu (iterator).
PIEZĪME: sākumā šeit biju uzrakstījis vēl otru meklēšanu ar [] operatoru, bet tas nekompilējās, jo corpusStats šeit ir const& 
(skat. piezīmi pie funkcijas deklarācijas Metrics.h failā) - [] operators drīkstētu vārdnīcā arī IEVIETOT jaunu ierakstu, ja tāda atslēga vēl nav, 
tāpēc tas nav atļauts const vārdnīcai. Izmantojot atrasto iterator tieši, izvairos no otras, liekas meklēšanas. */
            auto foundEntry = corpusStats.documentFrequency.find(it->first);
            if (foundEntry != corpusStats.documentFrequency.end()) {
                df = foundEntry->second;
            }

            // IDF - jo retāk n-grams sastopams korpusā, jo lielāks IDF. TF šeit ir vienkārši n-grama skaits šajā tekstā
            double idf = log((double)corpusStats.documentCount / (1.0 + (double)df));
            refVec[it->first] = (double)it->second * idf;
        }

        // Šeit sagatavoju TF-IDF svērto kandidāta vektoru tāpat kā references vektoru augstāk
        map<string, double> candVec;
        for (auto it = candN.begin(); it != candN.end(); it++) {
            int df = 0;

            // Te meklēju n-gramu VIENU reizi (skat. garāko piezīmi pie references vektora augstāk)
            auto foundEntry = corpusStats.documentFrequency.find(it->first);
            if (foundEntry != corpusStats.documentFrequency.end()) {
                df = foundEntry->second;
            }

            double idf = log((double)corpusStats.documentCount / (1.0 + (double)df));
            candVec[it->first] = (double)it->second * idf;
        }

        double cosine = cosineSimilarity(refVec, candVec);
        sum += cosine;
        used++;
    }

    return safeDivide(sum, (double)used);
}

/* Šī funkcija uzbūvē korpusa statistiku no liela references aprakstu saraksta. Šo izsauc Evaluation klase vienreiz, ielādējot visu Flickr8k.token.txt failu 
par korpusu, un pēc tam atkārtoti izmanto to pašu CorpusStats visiem calcCiderCorpus izsaukumiem. */
CorpusStats Metrics::buildCorpusStats(vector<string> referenceTexts) {

    CorpusStats stats;
    stats.documentCount = 0;

    // Šis cikls tokenizē katru references tekstu (viens teksts = viens "dokuments" TF-IDF nozīmē) un palielina katra tajā sastopamā n-grama dokumentu biežumu
    for (int i = 0; i < (int)referenceTexts.size(); i++) {
        vector<string> tokens = textProc.tokenize(referenceTexts[i]);

        if (tokens.size() == 0) {
            continue;
        }

        stats.documentCount++;

        for (int n = 1; n <= 4; n++) {
            map<string, int> ngrams = countNgrams(tokens, n);  // Vajag tikai to, kuri n-grami parādās, nevis cik reizes

            for (auto it = ngrams.begin(); it != ngrams.end(); it++) {
                stats.documentFrequency[it->first]++;
            }
        }
    }

    return stats;
}

// Šī funkcija pārvērš score kategorijā. Kategorijas ir angliski, jo citādi latviski tās rada izvades problēmas izskata ziņā
string Metrics::makeLevel(double score) {
    if (score < 25) {
        return "insufficient";
    }
    if (score < 50) {
        return "weak";
    }
    if (score < 70) {
        return "average";
    }
    if (score < 85) {
        return "good";
    }

    return "excellent";
}

/* Šī funkcija pievieno semantisko līdzību rezultātam un pārrēķina gala score pēc dotā svaru profila. Šī ir vienīgā vieta visā programmā, kur tiek 
saskaitīts gala svērtais score - gan evaluate() (ar semantic=0.0 un noklusējuma profilu), gan Evaluation klase (ar īstu semantic score un
pareizo profilu pēc attēla funkcijas) izsauc tieši šo funkciju. */
MetricResult Metrics::attachSemantic(MetricResult result, double semanticScore, WeightProfile profile) {

    // Ja semantiskais score ir negatīvs (piemēram, -1.0 kļūdas signāls no SemanticSimilarity), tad aizstāju to ar 0
    if (semanticScore < 0.0) {
        semanticScore = 0.0;
    }

    result.semantic = semanticScore;

    result.finalScore = 100.0 * (
        profile.wPrecision * result.precision +
        profile.wRecall * result.recall +
        profile.wF1 * result.f1 +
        profile.wBleu * result.bleu +
        profile.wMeteor * result.meteor +
        profile.wRougeL * result.rougeL +
        profile.wCider * result.cider +
        profile.wSemantic * result.semantic
    );

    result.level = makeLevel(result.finalScore);

    return result;
}

// Šī funkcija salīdzina vienu reference ar vienu kandidātu, bez korpusa statistikas CIDER metrikai
MetricResult Metrics::evaluate(string referenceText, string candidateText) {

    MetricResult result;

    vector<string> ref = textProc.tokenize(referenceText);
    vector<string> cand = textProc.tokenize(candidateText);

    result.precision = calcPrecision(ref, cand);
    result.recall = calcRecall(ref, cand);
    result.f1 = safeDivide(2.0 * result.precision * result.recall, result.precision + result.recall);
    result.bleu = calcBleu(ref, cand);
    result.meteor = calcMeteor(ref, cand);
    result.rougeL = calcRougeL(ref, cand);
    result.cider = calcCider(ref, cand);
    result.purpose = "n/a";  // Šai funkcijai nav zināma attēla funkcija

    /* Gala score šeit aprēķinu bez semantiskās līdzības un ar noklusējuma svaru profilu - Evaluation klase parasti pati 
vēlreiz izsauc attachSemantic ar īstu semantic score un pareizo profilu. */
    Coefficients coefficients;
    result = attachSemantic(result, 0.0, coefficients.getDefaultProfile());

    return result;
}

// Šī funkcija salīdzina vienu reference ar vienu kandidātu, izmantojot korpusa TF-IDF statistiku CIDER metrikai
MetricResult Metrics::evaluate(string referenceText, string candidateText, const CorpusStats& corpusStats) {

    MetricResult result;

    vector<string> ref = textProc.tokenize(referenceText);
    vector<string> cand = textProc.tokenize(candidateText);

    result.precision = calcPrecision(ref, cand);
    result.recall = calcRecall(ref, cand);
    result.f1 = safeDivide(2.0 * result.precision * result.recall, result.precision + result.recall);
    result.bleu = calcBleu(ref, cand);
    result.meteor = calcMeteor(ref, cand);
    result.rougeL = calcRougeL(ref, cand);
    result.cider = calcCiderCorpus(ref, cand, corpusStats);
    result.purpose = "n/a";

    Coefficients coefficients;
    result = attachSemantic(result, 0.0, coefficients.getDefaultProfile());

    return result;
}

// Šī funkcija salīdzina kandidātu ar vairākām reference vērtībām
MetricResult Metrics::evaluateMulti(vector<string> references, string candidate) {

    MetricResult best;
    best.finalScore = -1;

    if (references.size() == 0) {
        return evaluate("", candidate);
    }

    // Šis cikls salīdzina kandidātu ar katru reference tekstu un patur labāko rezultātu
    for (int i = 0; i < (int)references.size(); i++) {
        MetricResult current = evaluate(references[i], candidate);

        if (current.finalScore > best.finalScore) {
            best = current;
        }
    }

    return best;
}
