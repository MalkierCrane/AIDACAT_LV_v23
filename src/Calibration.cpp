/* Šeit tiek iekļauta Calibration klases deklarācija, svaru profilu klase, failu lasīšana un rakstīšana, teksta plūsmas rindas sadalīšanai un atskaites veidošanai, 
cstdlib atof/atoi funkcijām, vārdnīcas attēlu indeksam, ekrāna izvade un formatēšanas funkcijas. */
#include "Calibration.h"
#include "Coefficients.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>
#include <iostream>
#include <iomanip>

using namespace std;

/* Šī funkcija ielādē ExpertAnnotations.txt failu. Katras rindas formāts ir:
judgedImage.jpg <TAB> candidateImage.jpg#N <TAB> vertetajs1 <TAB> vertetajs2 <TAB> vertetajs3, kur vertetaji ir skaitļi no 1 līdz 4.
Tad pārrēķinu vidējo uz 0-100 skalu, lai varētu salīdzināt ar finalScore. */
LoadedAnnotationRows Calibration::loadExpertAnnotations(string filePath, int limit) {
    
    // Rezultāta sagatavošana
    LoadedAnnotationRows result;
    result.skippedCount = 0;

    ifstream file(filePath);    // Faila atvēršana lasīšanai

    // Ja failu nevar atvērt, tad atgriežu tukšu rezultātu
    if (!file.is_open()) {
        return result;
    }

    string line;    // Te es sagatavoju vienas rindas tekstu

    // Šis cikls nolasa failu pa rindām
    while (getline(file, line)) {
        if (limit > 0 && (int)result.rows.size() >= limit) {    // Ja esu sasniedzis limitu derīgu rindu skaitā, beidzu.
            break;  // Izeju no cikla
        }

        // Ja rinda ir tukša, tad to izlaižu (nav uzskatāma par bojātu rindu)
        if (line.size() == 0) {
            continue;   // Tad turpinu ar nākamo rindu
        }

        // Te es sadalu rindu pa tabulatoriem
        vector<string> parts;
        stringstream ss(line);
        string part;

        // Šis cikls nolasa katru daļu starp tabulatoriem
        while (getline(ss, part, '\t')) {
            parts.push_back(part);  // Te es pievienoju daļu sarakstam
        }

        // Ja daļu ir mazāk par 5, rinda ir bojāta - izlaižu un saskaitu
        if (parts.size() < 5) {
            result.skippedCount++;      // Tad palielinu izlaisto rindu skaitītāju
            continue;                   // Un turpinu ar nākamo rindu
        }

        string judgedImage = parts[0];                  // Pirmā daļa ir vērtējamā attēla nosaukums
        string candidateField = parts[1];               // Otrā daļa ir kandidāta attēls un indekss, piemēram "image.jpg#2"
        size_t hashPos = candidateField.find('#');      // Šeit es meklēju # simbolu

        // Ja # nav atrasts, rinda ir bojāta
        if (hashPos == string::npos) {
            result.skippedCount++;      // Tad palielinu izlaisto rindu skaitītāju
            continue;                   // Un turpinu ar nākamo rindu
        }

        string candidateImage = candidateField.substr(0, hashPos);      // Šeit nogriežu kandidāta attēla nosaukumu
        string indexText = candidateField.substr(hashPos + 1);          // Tad nolasu indeksa tekstu aiz #
        int candidateIndex = atoi(indexText.c_str());                   // Tad pārvēršu indeksu par skaitli

        // Šeit es pārvēršu trīs vērtētāju skaitļus par double tipa skaitļiem
        double expert1 = atof(parts[2].c_str());
        double expert2 = atof(parts[3].c_str());
        double expert3 = atof(parts[4].c_str());

        double averageRaw = (expert1 + expert2 + expert3) / 3.0;    // Te es aprēķinu vidējo no trīs vērtētājiem, skala 1 līdz 4

        // Šeit es pārrēķinu skalu no 1-4 uz 0-100, lai to varētu salīdzināt ar finalScore
        double scaled = (averageRaw - 1.0) / 3.0 * 100.0;

        // Te tiek sagatavota rindas struktūra, lai to pievienotu rezultātam
        ExpertRow row;
        row.judgedImageName = judgedImage;
        row.candidateImageName = candidateImage;
        row.candidateIndex = candidateIndex;
        row.groundTruthScore = scaled;
        result.rows.push_back(row);
    }

    file.close();   // Faila aizvēršana

    return result;  // Ielādēto rindu atgriešana
}

/* Šī funkcija ielādē CrowdFlowerAnnotations.txt failu. Katras rindas formāts ir:
judgedImage.jpg <TAB> candidateImage.jpg#N <TAB> percentYes <TAB> numYes <TAB> numNo, kur percentYes jau ir daļskaitlis no 0 līdz 1 - pārrēķinu to uz 0-100 skalu. */
LoadedAnnotationRows Calibration::loadCrowdflowerAnnotations(string filePath, int limit) {

    // Rezultāta sagatavošana
    LoadedAnnotationRows result;
    result.skippedCount = 0;

    ifstream file(filePath);    // Faila atvēršana lasīšanai

    // Ja failu nevar atvērt, tad atgriežu tukšu rezultātu
    if (!file.is_open()) {
        return result;
    }

    string line;    // Te es sagatavoju vienas rindas tekstu

    // Šis cikls nolasa failu pa rindām
    while (getline(file, line)) {
        if (limit > 0 && (int)result.rows.size() >= limit) {    // Ja esmu sasniedzis limitu, beidzu.
            break;
        }

        // Ja rinda ir tukša, izlaižu to
        if (line.size() == 0) {
            continue;   // Un turpinu ar nākamo rindu
        }

        // Te es sadalām rindu pa tabulatoriem
        vector<string> parts;
        stringstream ss(line);
        string part;

        // Šis cikls nolasa katru daļu
        while (getline(ss, part, '\t')) {
            parts.push_back(part);  // Šeit es pievienoju daļu sarakstam
        }

        // Ja daļu ir mazāk par 3 (attēls, kandidāts, percentYes), rinda ir bojāta
        if (parts.size() < 3) {
            result.skippedCount++;      // Un vajag palielināt izlaisto rindu skaitītāju
            continue;                   // Un tad turpināt ar nākamo rindu
        }

        string judgedImage = parts[0];      // Pirmā daļa ir vērtējamā attēla nosaukums
        string candidateField = parts[1];   // Otrā daļa ir kandidāta attēls un indekss
        size_t hashPos = candidateField.find('#');  // Te es meklēju # simbolu

        // Ja # nav atrasts, rinda ir bojāta
        if (hashPos == string::npos) {
            result.skippedCount++;      // Un vajag palielināt izlaisto rindu skaitītāju
            continue;                   // Un tad turpināt ar nākamo rindu
        }

        string candidateImage = candidateField.substr(0, hashPos);  // Te nogriežu kandidāta attēla nosaukumu
        string indexText = candidateField.substr(hashPos + 1);      // Tad nolasu indeksa tekstu aiz #
        int candidateIndex = atoi(indexText.c_str());               // Tad es pārvēršu indeksu par skaitli
        double percentYes = atof(parts[2].c_str());                 // Tad pārvēršu percentYes daļskaitli
        double scaled = percentYes * 100.0;                         // Un pārrēķinu uz 0-100 skalu

        // Te tiek sagatavota rindas struktūra, lai to pievienotu rezultātam
        ExpertRow row;
        row.judgedImageName = judgedImage;
        row.candidateImageName = candidateImage;
        row.candidateIndex = candidateIndex;
        row.groundTruthScore = scaled;
        result.rows.push_back(row);
    }

    file.close();   // Faila aizvēršana

    return result;  // Ielādēto rindu atgriešana
}

// Šī funkcija pārvērš svaru profilu par vienkāršu 8 skaitļu sarakstu
vector<double> Calibration::profileToArray(WeightProfile profile) {

    vector<double> values;  // Saraksta sagatavošana

    // Tad es pievienoju katru svaru fiksētā secībā
    values.push_back(profile.wPrecision);
    values.push_back(profile.wRecall);
    values.push_back(profile.wF1);
    values.push_back(profile.wBleu);
    values.push_back(profile.wMeteor);
    values.push_back(profile.wRougeL);
    values.push_back(profile.wCider);
    values.push_back(profile.wSemantic);

    return values;  // Un atgriežu sarakstu
}

// Šī funkcija pārvērš 8 skaitļu sarakstu atpakaļ par svaru profilu
WeightProfile Calibration::arrayToProfile(vector<double> values, string name) {

    WeightProfile profile;  // Profila sagatavošana
    profile.name = name; // Nosaukuma saglabāšana

    // Tad saglabāju katru svaru tajā pašā secībā, kādā tie tika izvietoti profileToArray
    profile.wPrecision = values[0];
    profile.wRecall = values[1];
    profile.wF1 = values[2];
    profile.wBleu = values[3];
    profile.wMeteor = values[4];
    profile.wRougeL = values[5];
    profile.wCider = values[6];
    profile.wSemantic = values[7];

    return profile; // Un atgriežu profilu
}

// Šī funkcija padara, lai 8 svari summējas 1.0, dalot katru ar kopējo summu
vector<double> Calibration::normalizeWeightArray(vector<double> values) {

    double sum = 0.0;   // Summas sagatavošana

    for (int i = 0; i < (int)values.size(); i++) {      // Šis cikls saskaita visus svarus
        sum += values[i];                               // Un pieskaita svaru summai
    }

    // Ja summa ir 0 vai negatīva (nedrīkstētu notikt, bet drošības pēc), atgriežu bez izmaiņām
    if (sum <= 0.0) {
        return values;
    }

    vector<double> result;       // Te jau tiek sagatavots normalizēts saraksts

    for (int i = 0; i < (int)values.size(); i++) {      // Šis cikls dala katru svaru ar summu
        result.push_back(values[i] / sum);              // Un pievieno normalizēto vērtību
    }

    return result;      // Normalizētā saraksta atgriešana
}

/* Šī funkcija pārrēķina final_score visiem rezultātiem ar doto profilu un atgriež korelāciju ar cilvēku vērtējumu. 
Semantiskā vērtība katram rezultātam jau ir aprēķināta agrāk (nav jāsauc Python vēlreiz) - mainās tikai svari, ar kuriem tā tiek apvienota ar pārējām metrikām. */
double Calibration::evaluateProfileCorrelation(vector<MetricResult> ourResults, vector<double> groundTruthScores, WeightProfile profile) {

    Metrics metricsEngine;          // Te es izveidoju lokālu Metrics objektu tikai attachSemantic izsaukšanai
    vector<double> trialScores;     // Tad sagatavoju sarakstu ar pārrēķinātajiem final_score

    for (int i = 0; i < (int)ourResults.size(); i++) {          // Šis cikls pārrēķina katru rezultātu ar jauno profilu

        // Te es pārrēķinu gala score, izmantojot jau zināmo semantic vērtību un jauno profilu
        MetricResult recomputed = metricsEngine.attachSemantic(ourResults[i], ourResults[i].semantic, profile);

        // Un pievienoju pārrēķināto score sarakstam
        trialScores.push_back(recomputed.finalScore);
    }

        // Un tad atgriežu korelāciju starp pārrēķinātajiem score un cilvēku vērtējumu
    return statsUtil.pearsonCorrelation(trialScores, groundTruthScores);
}

/* Šī funkcija meklē uzlabotu svaru profilu ar koordinātu kāpšanas metodi. Pilna režģa meklēšana pa visām 8 dimensijām (piemēram, 5 vērtības katrai) būtu 5^8, 
kas nav praktiski izpildāms - tāpēc katrā solī maināma tikai viena svara vērtība uz reizi, un izmaiņa tiek paturēta tikai tad, ja tā uzlabo korelāciju ar cilvēku vērtējumu. */
WeightProfile Calibration::hillClimbSearch(vector<MetricResult> ourResults, vector<double> groundTruthScores, WeightProfile startProfile) {

    vector<double> currentWeights = profileToArray(startProfile);   // Te tiek pārvērsts sākuma profils par skaitļu sarakstu

    // Tad aprēķināta sākuma korelāciju
    double bestCorrelation = evaluateProfileCorrelation(ourResults, groundTruthScores, arrayToProfile(currentWeights, "informative"));

    // Tad es sagatavoju soļa lielumu un pāreju skaitu
    double step = 0.02;
    int passCount = 20;

    for (int pass = 0; pass < passCount; pass++) {                  // Šeit ārējais cikls iet cauri vairākām pārejām pa visiem svariem
        for (int fieldIndex = 0; fieldIndex < 8; fieldIndex++) {    // Un iekšējais cikls iet cauri katram no 8 svariem pēc kārtas

            // Te mēģinu palielināt šo svaru par vienu soli
            vector<double> trialUp = currentWeights;
            trialUp[fieldIndex] = trialUp[fieldIndex] + step;
            trialUp = normalizeWeightArray(trialUp);

            // Tad aprēķināt korelāciju ar palielināto svaru
            double correlationUp = evaluateProfileCorrelation(ourResults, groundTruthScores, arrayToProfile(trialUp, "informative"));

            // Ja tas uzlabo korelāciju, paturu šo izmaiņu, saglabāju jauno labāko korelāciju un jaunos svarus
            if (correlationUp > bestCorrelation) {
                bestCorrelation = correlationUp;
                currentWeights = trialUp;
                continue;       // Tad turpinu ar nākamo svaru, šo jau uzlaboju
            }

            // Te mēģinu samazināt šo svaru par vienu soli, ja tas nekļūst negatīvs
            if (currentWeights[fieldIndex] - step > 0.0) {

                // Šeit sagatavoju samazinātu variantu
                vector<double> trialDown = currentWeights;
                trialDown[fieldIndex] = trialDown[fieldIndex] - step;
                trialDown = normalizeWeightArray(trialDown);

                // Tad es aprēķinu korelāciju ar samazināto svaru
                double correlationDown = evaluateProfileCorrelation(ourResults, groundTruthScores, arrayToProfile(trialDown, "informative"));

                // Ja tas uzlabo korelāciju, paturu šo izmaiņu, saglabāju jauno labāko korelāciju un jaunos svarus
                if (correlationDown > bestCorrelation) {
                    bestCorrelation = correlationDown;
                    currentWeights = trialDown;
                }
            }
        }
    }

    // Tad atgriežu labāko atrasto profilu
    return arrayToProfile(currentWeights, "informative");
}

// Funkcija uzraksta gala atskaiti par kalibrēšanu.
void Calibration::writeReport(string outputPath, string sourceLabel, int loadedCount, int skippedRowsCount, int skippedLookupCount,
                               vector<MetricResult> ourResults, vector<double> groundTruthScores,
                               WeightProfile startProfile, WeightProfile discoveredProfile) {

    ostringstream out;

    double correlationBefore = evaluateProfileCorrelation(ourResults, groundTruthScores, startProfile);
    double correlationAfter = evaluateProfileCorrelation(ourResults, groundTruthScores, discoveredProfile);

    out << "=== Koeficientu kalibresanas atskaite (" << sourceLabel << ") ===" << endl;
    out << endl;
    out << "Ielādēto rindu skaits: " << loadedCount << endl;
    out << "Izlaisto rindu skaits (bojāts formāts): " << skippedRowsCount << endl;
    out << "Izlaisto rindu skaits (attēls nav atrasts Flickr8k datos): " << skippedLookupCount << endl;
    out << endl;

    out << "--- Korelacija PIRMS kalibresanas (sakotnejais 'informative' profils) ---" << endl;
    out << fixed << setprecision(3);
    out << "final_score vs cilveku vertejums: " << correlationBefore << endl;
    out << endl;

    out << "--- Korelacija PEC kalibresanas (atrastais profils) ---" << endl;
    out << "final_score vs cilveku vertejums: " << correlationAfter << endl;
    out << endl;

    // Piedāvātais svaru profils gatavs kopēšanai uz Coefficients.cpp.
    out << "--- Ieteiktais 'informative' profils (var kopēt uz Coefficients.cpp) ---" << endl;
    out << setprecision(4);
    out << "precision : " << discoveredProfile.wPrecision << endl;
    out << "recall    : " << discoveredProfile.wRecall << endl;
    out << "f1        : " << discoveredProfile.wF1 << endl;
    out << "bleu      : " << discoveredProfile.wBleu << endl;
    out << "meteor    : " << discoveredProfile.wMeteor << endl;
    out << "rouge_l   : " << discoveredProfile.wRougeL << endl;
    out << "cider     : " << discoveredProfile.wCider << endl;
    out << "semantic  : " << discoveredProfile.wSemantic << endl;
    out << endl;

    out << "--- Piezime ---" << endl;
    out << "Sis process kalibre TIKAI 'informative' profilu, jo ne ExpertAnnotations.txt," << endl;
    out << "ne CrowdFlowerAnnotations.txt nesatur atzimes par attela funkciju (decorative/" << endl;
    out << "functional/complex). Pareja 3 profili paliek pamatoti, bet empiriski nepārbaudīti" << endl;
    out << "sakuma pienemumi." << endl;

    ofstream outFile(outputPath);
    outFile << out.str();
    outFile.close();
}

// Šī funkcija veic pilnu kalibrēšanas ciklu vienam avota failam
void Calibration::runCalibration(string annotationFile, vector<FlickrItem> flickrItems, CorpusStats corpusStats,
                                  string outputPath, int limit, bool useCrowdflower,
                                  Metrics metricsEngine, SemanticSimilarity semanticSimilarity) {

    map<string, FlickrItem> flickrIndex;  // Attēlu indekss ātrai piekļuvei pēc nosaukuma

    for (int i = 0; i < (int)flickrItems.size(); i++) {
        flickrIndex[flickrItems[i].imageName] = flickrItems[i];
    }

    // Ielādēju annotāciju rindas - vai nu no Expert, vai no CrowdFlower faila
    LoadedAnnotationRows loaded;
    if (useCrowdflower) {
        loaded = loadCrowdflowerAnnotations(annotationFile, limit);
    }
    else {
        loaded = loadExpertAnnotations(annotationFile, limit);
    }

    if (loaded.rows.size() == 0) {
        cout << "Kalibresanai nevareja ielādēt neviena derīgu rindu no: " << annotationFile << endl;
        return;
    }

    // Sagatavoju Coefficients objektu, lai iegūtu sākuma "informative" profilu
    Coefficients coefficients;
    WeightProfile startProfile = coefficients.getDefaultProfile();

    vector<MetricResult> ourResults;
    vector<double> groundTruthScores;
    int skippedLookupCount = 0;  // Rindas, kurām neizdevās atrast atbilstošos attēlus datos

    // Šis cikls iet cauri visām ielādētajām annotāciju rindām
    for (int i = 0; i < (int)loaded.rows.size(); i++) {
        ExpertRow row = loaded.rows[i];

        // Pārbaudu, vai vērtējamais un kandidāta attēls ir atrodami Flickr8k datos
        if (flickrIndex.find(row.judgedImageName) == flickrIndex.end()) {
            skippedLookupCount++;
            continue;
        }
        if (flickrIndex.find(row.candidateImageName) == flickrIndex.end()) {
            skippedLookupCount++;
            continue;
        }

        FlickrItem judgedItem = flickrIndex[row.judgedImageName];
        FlickrItem candidateItem = flickrIndex[row.candidateImageName];

        // Ja vērtējamajam attēlam nav neviena apraksta, to nevar izmantot par references tekstu
        if (judgedItem.captions.size() == 0) {
            skippedLookupCount++;
            continue;
        }

        // Pārbaudu, vai kandidāta indekss ir derīgā robežā
        if (row.candidateIndex < 0 || row.candidateIndex >= (int)candidateItem.captions.size()) {
            skippedLookupCount++;
            continue;
        }

        /* Par references tekstu izmantoju vērtējamā attēla pirmo cilvēka aprakstu - tas pats
        princips, ko jau lieto experiment režīms Flickr8k validācijai */
        string referenceText = judgedItem.captions[0];
        string candidateText = candidateItem.captions[row.candidateIndex];  // Apraksts, kuru eksperti vērtēja

        MetricResult result = metricsEngine.evaluate(referenceText, candidateText, corpusStats);
        double semanticScore = semanticSimilarity.computeSimilarity(referenceText, candidateText);

        // Semantiskā līdzība tiek pievienota ar sākuma profilu - to vēlāk pārrēķina hillClimbSearch
        result = metricsEngine.attachSemantic(result, semanticScore, startProfile);

        ourResults.push_back(result);
        groundTruthScores.push_back(row.groundTruthScore);
    }

    if (ourResults.size() == 0) {
        cout << "Kalibresanai nevareja atrast nevienu derigu pari Flickr8k datos." << endl;
        return;
    }

    WeightProfile discoveredProfile = hillClimbSearch(ourResults, groundTruthScores, startProfile);
    string sourceLabel = useCrowdflower ? "CrowdFlowerAnnotations.txt" : "ExpertAnnotations.txt";

    writeReport(outputPath, sourceLabel, (int)ourResults.size(), loaded.skippedCount, skippedLookupCount,
                ourResults, groundTruthScores, startProfile, discoveredProfile);

    cout << endl;
    cout << "Kalibresana pabeigta (" << sourceLabel << ")." << endl;
    cout << "Izmantoto paru skaits: " << ourResults.size() << endl;
    cout << "Atskaite saglabata: " << outputPath << endl;
}
