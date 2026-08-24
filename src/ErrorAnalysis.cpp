// Šeit tiek iekļauta ErrorAnalysis klases deklarācija, ekrāna izvade, failu rakstīšana, teksta plūsma atskaites veidošanai un formatēšanas funkcijas
#include "ErrorAnalysis.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

// Funkcija saskaita, cik pāru iekrīt katrā no 5 level kategorijām, fiksētā secībā: insufficient, weak, average, good, excellent
vector<int> ErrorAnalysis::countLevelBuckets(vector<ScoredPair> pairs) {

    vector<int> counts(5, 0);

    // Šis cikls salīdzina katra pāra kategoriju ar visām zināmajām un palielina atbilstošo skaitītāju
    for (int i = 0; i < (int)pairs.size(); i++) {
        string level = pairs[i].metrics.level;

        if (level == "insufficient") {
            counts[0]++;
        }
        else if (level == "weak") {
            counts[1]++;
        }
        else if (level == "average") {
            counts[2]++;
        }
        else if (level == "good") {
            counts[3]++;
        }
        else if (level == "excellent") {
            counts[4]++;
        }
    }

    return counts;
}

// Šī funkcija formatē visu detalizēto atskaiti par tekstu
string ErrorAnalysis::formatReport(vector<ScoredPair> pairs, vector<ScoredPair> worstPairs, vector<ScoredPair> bestPairs, int topN) {

    ostringstream out;

    // Te es savācu katras metrikas vērtības atsevišķos sarakstos, lai varētu rēķināt statistiku
    vector<double> precisionValues;
    vector<double> recallValues;
    vector<double> f1Values;
    vector<double> bleuValues;
    vector<double> meteorValues;
    vector<double> rougeLValues;
    vector<double> ciderValues;
    vector<double> semanticValues;
    vector<double> finalScoreValues;

    for (int i = 0; i < (int)pairs.size(); i++) {
        precisionValues.push_back(pairs[i].metrics.precision);
        recallValues.push_back(pairs[i].metrics.recall);
        f1Values.push_back(pairs[i].metrics.f1);
        bleuValues.push_back(pairs[i].metrics.bleu);
        meteorValues.push_back(pairs[i].metrics.meteor);
        rougeLValues.push_back(pairs[i].metrics.rougeL);
        ciderValues.push_back(pairs[i].metrics.cider);
        semanticValues.push_back(pairs[i].metrics.semantic);
        finalScoreValues.push_back(pairs[i].metrics.finalScore);
    }

    // Te ir statistika un korelācija ar final_score katrai metrikai atsevišķi
    MetricStats precisionStats = statsUtil.calcStats(precisionValues);
    MetricStats recallStats = statsUtil.calcStats(recallValues);
    MetricStats f1Stats = statsUtil.calcStats(f1Values);
    MetricStats bleuStats = statsUtil.calcStats(bleuValues);
    MetricStats meteorStats = statsUtil.calcStats(meteorValues);
    MetricStats rougeLStats = statsUtil.calcStats(rougeLValues);
    MetricStats ciderStats = statsUtil.calcStats(ciderValues);
    MetricStats semanticStats = statsUtil.calcStats(semanticValues);
    MetricStats finalScoreStats = statsUtil.calcStats(finalScoreValues);

    double corrPrecision = statsUtil.pearsonCorrelation(finalScoreValues, precisionValues);
    double corrRecall = statsUtil.pearsonCorrelation(finalScoreValues, recallValues);
    double corrF1 = statsUtil.pearsonCorrelation(finalScoreValues, f1Values);
    double corrBleu = statsUtil.pearsonCorrelation(finalScoreValues, bleuValues);
    double corrMeteor = statsUtil.pearsonCorrelation(finalScoreValues, meteorValues);
    double corrRougeL = statsUtil.pearsonCorrelation(finalScoreValues, rougeLValues);
    double corrCider = statsUtil.pearsonCorrelation(finalScoreValues, ciderValues);
    double corrSemantic = statsUtil.pearsonCorrelation(finalScoreValues, semanticValues);

    vector<int> levelCounts = countLevelBuckets(pairs);

    out << "=== Flickr8k kludu analize (error analysis) ===" << endl;
    out << endl;
    out << "Apstradato paru skaits: " << pairs.size() << endl;
    out << endl;

    // Šī ir statistikas tabula pa metrikām
    out << "--- Statistika pa metrikam (videjais / mediana / std.novirze / min / max) ---" << endl;
    out << fixed << setprecision(4);
    out << "precision  : " << precisionStats.mean << " / " << precisionStats.median << " / " << precisionStats.stdDev << " / " << precisionStats.minVal << " / " << precisionStats.maxVal << endl;
    out << "recall     : " << recallStats.mean << " / " << recallStats.median << " / " << recallStats.stdDev << " / " << recallStats.minVal << " / " << recallStats.maxVal << endl;
    out << "f1         : " << f1Stats.mean << " / " << f1Stats.median << " / " << f1Stats.stdDev << " / " << f1Stats.minVal << " / " << f1Stats.maxVal << endl;
    out << "bleu       : " << bleuStats.mean << " / " << bleuStats.median << " / " << bleuStats.stdDev << " / " << bleuStats.minVal << " / " << bleuStats.maxVal << endl;
    out << "meteor     : " << meteorStats.mean << " / " << meteorStats.median << " / " << meteorStats.stdDev << " / " << meteorStats.minVal << " / " << meteorStats.maxVal << endl;
    out << "rouge_l    : " << rougeLStats.mean << " / " << rougeLStats.median << " / " << rougeLStats.stdDev << " / " << rougeLStats.minVal << " / " << rougeLStats.maxVal << endl;
    out << "cider      : " << ciderStats.mean << " / " << ciderStats.median << " / " << ciderStats.stdDev << " / " << ciderStats.minVal << " / " << ciderStats.maxVal << endl;
    out << "semantic   : " << semanticStats.mean << " / " << semanticStats.median << " / " << semanticStats.stdDev << " / " << semanticStats.minVal << " / " << semanticStats.maxVal << endl;
    out << setprecision(2);
    out << "final_score: " << finalScoreStats.mean << " / " << finalScoreStats.median << " / " << finalScoreStats.stdDev << " / " << finalScoreStats.minVal << " / " << finalScoreStats.maxVal << endl;
    out << endl;

    // Līmeņu sadalījums
    out << "--- Level sadalijums ---" << endl;
    out << "insufficient: " << levelCounts[0] << endl;
    out << "weak        : " << levelCounts[1] << endl;
    out << "average     : " << levelCounts[2] << endl;
    out << "good        : " << levelCounts[3] << endl;
    out << "excellent   : " << levelCounts[4] << endl;
    out << endl;

    // Korelācija ar final_score
    out << "--- Korelacija starp final_score un katru metriku (Pirsona koeficients, no -1 lidz 1) ---" << endl;
    out << setprecision(3);
    out << "precision vs final_score: " << corrPrecision << endl;
    out << "recall    vs final_score: " << corrRecall << endl;
    out << "f1        vs final_score: " << corrF1 << endl;
    out << "bleu      vs final_score: " << corrBleu << endl;
    out << "meteor    vs final_score: " << corrMeteor << endl;
    out << "rouge_l   vs final_score: " << corrRougeL << endl;
    out << "cider     vs final_score: " << corrCider << endl;
    out << "semantic  vs final_score: " << corrSemantic << endl;
    out << endl;

    // Sliktākie pāri - noderīgi, lai saprastu, kur metrikas vai Florence "krīt"
    out << "--- " << topN << " sliktakie pari (zemakais final_score) ---" << endl;

    // worstPairs jau ir sakārtots augošā secībā, tāpēc pirmais ir vissliktākais
    for (int i = 0; i < (int)worstPairs.size(); i++) {
        out << (i + 1) << ". attels: " << worstPairs[i].imageName << ", final_score: " << fixed << setprecision(2) << worstPairs[i].metrics.finalScore << endl;
        out << "   reference: " << worstPairs[i].referenceText << endl;
        out << "   kandidats: " << worstPairs[i].candidateText << endl;
    }
    out << endl;

    // Labākie pāri - noderīgi kā "veiksmes stāsti" salīdzinājumam
    out << "--- " << topN << " labakie pari (augstakais final_score) ---" << endl;

    // bestPairs arī ir sakārtots augošā secībā, tāpēc labākais ir pēdējais - es eju no beigām uz sākumu
    int rank = 1;
    for (int i = (int)bestPairs.size() - 1; i >= 0; i--) {
        out << rank << ". attels: " << bestPairs[i].imageName << ", final_score: " << fixed << setprecision(2) << bestPairs[i].metrics.finalScore << endl;
        out << "   reference: " << bestPairs[i].referenceText << endl;
        out << "   kandidats: " << bestPairs[i].candidateText << endl;
        rank++;
    }

    return out.str();
}

// Šī funkcija veic pilnu kļūdu analīzi un ieraksta rezultātu norādītajā failā
void ErrorAnalysis::runAnalysis(vector<ScoredPair> pairs, string outputPath, int topN) {

    // Ja nav neviena pāra, nav ko analizēt
    if (pairs.size() == 0) {
        cout << "Kludu analizei nav paru, so soli izlaizam." << endl;
        return;
    }

    /* Sliktāko un labāko pāru saraksti tiek uzturēti sakārtoti augošā secībā pēc final_score, ar ne vairāk kā topN elementiem, 
    izmantojot ierobežotu ievietošanu, nevis visa 32000 pāru saraksta pilnu sakārtošanu (kas pie liela datu apjoma būtu lēni). */
    vector<ScoredPair> worstPairs;
    vector<ScoredPair> bestPairs;

    vector<double> finalScoreValues;  // Tas ir ātram vidējā final_score aprēķinam konsoles izvadei

    for (int i = 0; i < (int)pairs.size(); i++) {
        double score = pairs[i].metrics.finalScore;
        finalScoreValues.push_back(score);

        // Sliktākā saraksta uzturēšana (topN zemākie score, sakārtoti augoši)

        // Ja sliktāko saraksts vēl nav pilns, vienkārši pievienoju pāri un pabīdu to pareizajā vietā
        if ((int)worstPairs.size() < topN) {
            worstPairs.push_back(pairs[i]);

            int j = (int)worstPairs.size() - 1;
            while (j > 0 && worstPairs[j - 1].metrics.finalScore > worstPairs[j].metrics.finalScore) {
                ScoredPair temp = worstPairs[j - 1];
                worstPairs[j - 1] = worstPairs[j];
                worstPairs[j] = temp;
                j--;
            }
        }

        /* Ja saraksts jau ir pilns, aizvietoju tikai tad, ja šis pāris ir sliktāks par pašlaik
        saglabāto "vissliktāko no sliktākajiem" (pēdējo sarakstā, jo tas sakārtots augoši) */
        else if (worstPairs.size() > 0 && score < worstPairs[worstPairs.size() - 1].metrics.finalScore) {
            worstPairs[worstPairs.size() - 1] = pairs[i];

            int j = (int)worstPairs.size() - 1;
            while (j > 0 && worstPairs[j - 1].metrics.finalScore > worstPairs[j].metrics.finalScore) {
                ScoredPair temp = worstPairs[j - 1];
                worstPairs[j - 1] = worstPairs[j];
                worstPairs[j] = temp;
                j--;
            }
        }

        // Labākā saraksta uzturēšana (topN augstākie score, sakārtoti augoši)

        if ((int)bestPairs.size() < topN) {
            bestPairs.push_back(pairs[i]);

            int j = (int)bestPairs.size() - 1;
            while (j > 0 && bestPairs[j - 1].metrics.finalScore > bestPairs[j].metrics.finalScore) {
                ScoredPair temp = bestPairs[j - 1];
                bestPairs[j - 1] = bestPairs[j];
                bestPairs[j] = temp;
                j--;
            }
        }
        /* Ja saraksts jau ir pilns, aizvietoju tikai tad, ja šis pāris ir labāks par pašlaik
        saglabāto "vissliktāko no labākajiem" (pirmo sarakstā, jo tas sakārtots augoši) */
        else if (bestPairs.size() > 0 && score > bestPairs[0].metrics.finalScore) {
            bestPairs[0] = pairs[i];

            int j = 0;
            while (j < (int)bestPairs.size() - 1 && bestPairs[j].metrics.finalScore > bestPairs[j + 1].metrics.finalScore) {
                ScoredPair temp = bestPairs[j];
                bestPairs[j] = bestPairs[j + 1];
                bestPairs[j + 1] = temp;
                j++;
            }
        }
    }

    string report = formatReport(pairs, worstPairs, bestPairs, topN);

    ofstream outFile(outputPath);
    outFile << report;
    outFile.close();

    double averageScore = statsUtil.calcMean(finalScoreValues);

    cout << endl;
    cout << "Kludu analize pabeigta." << endl;
    cout << "Apstradato paru skaits: " << pairs.size() << endl;
    cout << "Videjais final_score: " << fixed << setprecision(2) << averageScore << " / 100" << endl;
    cout << "Detalizeta atskaite saglabata: " << outputPath << endl;
}
