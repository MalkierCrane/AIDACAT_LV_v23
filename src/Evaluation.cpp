// Šeit tiek iekļauta Evaluation klases deklarācija, ekrāna izvade, failu rakstīšana, failu sistēma mapju izveidei un formatēšanas funkcijas
#include "Evaluation.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

using namespace std;

namespace fs = filesystem;

// Funkcija izveido output mapi, ja tā vēl nav izveidota
void Evaluation::makeOutputDir() {

    if (!fs::exists("output")) {
        fs::create_directory("output");
    }

    if (!fs::exists("output/downloaded_images")) {
        fs::create_directories("output/downloaded_images");
    }
}

// Funkcija sagatavo tekstu CSV failam
string Evaluation::csvSafe(string value) {

    string result = "\"";

    // Šis cikls kopē katru simbolu; CSV vajag divkāršot pēdiņas iekšējā teksta pēdiņām
    for (int i = 0; i < (int)value.size(); i++) {
        if (value[i] == '"') {
            result += "\"\"";
        }
        else {
            result += value[i];
        }
    }

    result += "\"";

    return result;
}

// Šī funkcija saliek mapes un faila ceļu
string Evaluation::joinPath(string folder, string file) {

    fs::path path = fs::path(folder) / fs::path(file);

    return path.string();
}

// Šī funkcija izveido lokālu faila nosaukumu attēlam
string Evaluation::makeFileName(int index) {

    string name = "image_" + to_string(index) + ".jpg";

    return name;
}

// Šī funkcija izvada metrikas ekrānā
void Evaluation::printMetric(MetricResult result) {

    cout << "Precision : " << fixed << setprecision(3) << result.precision << endl;
    cout << "Recall    : " << fixed << setprecision(3) << result.recall << endl;
    cout << "F1        : " << fixed << setprecision(3) << result.f1 << endl;
    cout << "BLEU      : " << fixed << setprecision(3) << result.bleu << endl;
    cout << "METEOR    : " << fixed << setprecision(3) << result.meteor << endl;
    cout << "ROUGE-L   : " << fixed << setprecision(3) << result.rougeL << endl;
    cout << "CIDEr     : " << fixed << setprecision(3) << result.cider << endl;
    cout << "Semantic  : " << fixed << setprecision(3) << result.semantic << endl;
    cout << "Final     : " << fixed << setprecision(2) << result.finalScore << " / 100" << endl;
    cout << "Level     : " << result.level << endl;
    cout << "Purpose   : " << result.purpose << endl;
}

/* Šī funkcija ielādē korpusa statistiku no pilnā Flickr8k.token.txt faila, ja tas vēl nav izdarīts. To izmanto CIDER metrikas TF-IDF svaru aprēķinam 
visos režīmos, ne tikai experiment režīmā. Ja fails nav atrodams (piemēram, kāds palaiž programmu no citas mapes), es izvadu brīdinājumu
un ļauju programmai turpināt ar vienkāršoto, bez-korpusa CIDER versiju. */
void Evaluation::ensureCorpusStats() {

    // Ja jau esmu mēģinājis ielādēt korpusu šajā palaišanas reizē, otrreiz to nedaru
    if (corpusStatsLoaded) {
        return;
    }

    corpusStatsLoaded = true;  // Te atzīmēju, ka mēģinājums notika, neatkarīgi no rezultāta

    string corpusPath = "data/Flickr8k_text/Flickr8k.token.txt";
    vector<FlickrItem> corpusItems = flickrLoader.load(corpusPath, 0);

    // Ja korpuss neielādējās (fails nav atrasts vai tukšs), izvadu brīdinājumu un beidzu
    if (corpusItems.size() == 0) {
        cout << "Bridinajums: korpusa fails '" << corpusPath << "' netika atrasts vai ir tukss." << endl;
        cout << "CIDEr metrika izmantos vienkarsoto versiju bez korpusa TF-IDF svariem." << endl;

        corpusStatsAvailable = false;
        return;
    }

    // Šeit es savācu visus references aprakstus vienā sarakstā, lai varētu uzbūvēt korpusa statistiku
    vector<string> allCaptions;
    for (int i = 0; i < (int)corpusItems.size(); i++) {
        for (int j = 0; j < (int)corpusItems[i].captions.size(); j++) {
            allCaptions.push_back(corpusItems[i].captions[j]);
        }
    }

    corpusStats = metrics.buildCorpusStats(allCaptions);
    corpusStatsAvailable = true;
}

/* Šī funkcija aprēķina metrikas vienam pārim, izmantojot korpusa CIDER (ja pieejams) un pieskaitot semantisko līdzību ar doto svaru profilu. 
To izmanto single un web režīmi, kur attēlu skaits ir mazs un papildu Python izsaukums semantiskajai līdzībai nav problēma.
Experiment režīms TO NEIZMANTO - skat. piezīmi runExperiment funkcijā. */
MetricResult Evaluation::scoreTexts(string referenceText, string candidateText, WeightProfile profile) {

    ensureCorpusStats();

    MetricResult result;

    // Ja korpuss ir pieejams, es izmantoju korpusa CIDER versiju, citādi vienkāršoto bez korpusa
    if (corpusStatsAvailable) {
        result = metrics.evaluate(referenceText, candidateText, corpusStats);
    }
    else {
        result = metrics.evaluate(referenceText, candidateText);
    }

    double semanticScore = semanticSimilarity.computeSimilarity(referenceText, candidateText);
    result = metrics.attachSemantic(result, semanticScore, profile);

    return result;
}

// Šī funkcija uzstāda Python ceļu gan Florence, gan SemanticSimilarity objektiem, jo abi izmanto to pašu Python virtuālo vidi
void Evaluation::setPythonExe(string path) {

    florence.setPythonExe(path);
    semanticSimilarity.setPythonExe(path);
}

// Šī funkcija uzstāda Florence modeļa nosaukumu
void Evaluation::setModelName(string model) {
    florence.setModelName(model);
}

// Šī funkcija uzstāda sentence-transformers modeļa nosaukumu
void Evaluation::setSemanticModelName(string model) {
    semanticSimilarity.setModelName(model);
}

// Šī funkcija palaiž Flickr8k validācijas režīmu ar kļūdu analīzi
void Evaluation::runExperiment(string tokenFile, int limit, int topN) {

    makeOutputDir();

    vector<FlickrItem> items = flickrLoader.load(tokenFile, limit);
    ensureCorpusStats();

    ofstream csv("output/flickr_validation_results.csv");
    csv << "image,reference,test_caption,precision,recall,f1,bleu,meteor,rouge_l,cider,semantic,final_score,level" << endl;

    double totalScore = 0;
    int count = 0;
    vector<ScoredPair> allPairs;

    // Šis cikls iet cauri katram attēlam un salīdzina tā pirmo aprakstu ar pārējiem
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].captions.size() < 2) {
            continue;
        }

        string reference = items[i].captions[0];

        for (int j = 1; j < (int)items[i].captions.size(); j++) {
            string testCaption = items[i].captions[j];

            /* SVARĪGA PIEZĪME: experiment režīmā apzināti NEaprēķinu semantic metriku. Pilnā Flickr8k datu kopā ir apmēram 32000 salīdzināmu 
            pāru - ja katram no tiem palaistu atsevišķu Python procesu semantiskās līdzības aprēķinam, viss process ilgtu neprātīgi ilgi. 
            Tas ir tas pats iemesls, kāpēc šis režīms jau kopš pirmās programmas versijas nekad neizsauc arī Florence - tas paliek tīri teksta metriku
            salīdzinājums, lai to varētu palaist uz visas datu kopas saprātīgā laikā. */
            MetricResult result;
            if (corpusStatsAvailable) {
                result = metrics.evaluate(reference, testCaption, corpusStats);
            }
            else {
                result = metrics.evaluate(reference, testCaption);
            }

            totalScore += result.finalScore;
            count++;

            csv << csvSafe(items[i].imageName) << ",";
            csv << csvSafe(reference) << ",";
            csv << csvSafe(testCaption) << ",";
            csv << result.precision << ",";
            csv << result.recall << ",";
            csv << result.f1 << ",";
            csv << result.bleu << ",";
            csv << result.meteor << ",";
            csv << result.rougeL << ",";
            csv << result.cider << ",";
            csv << result.semantic << ",";
            csv << result.finalScore << ",";
            csv << csvSafe(result.level) << endl;

            ScoredPair pair;
            pair.imageName = items[i].imageName;
            pair.referenceText = reference;
            pair.candidateText = testCaption;
            pair.metrics = result;

            allPairs.push_back(pair);
        }
    }

    csv.close();

    cout << "Flickr8k validacijas rezims pabeigts" << endl;
    cout << "Salidzinajumu skaits: " << count << endl;

    if (count > 0) {
        double average = totalScore / count;
        cout << "Videjais final score: " << fixed << setprecision(2) << average << " / 100" << endl;
    }

    cout << "Rezultati saglabati: output/flickr_validation_results.csv" << endl;

    errorAnalysis.runAnalysis(allPairs, "output/flickr_error_analysis.txt", topN);
}

// Šī funkcija pārbauda vienu lokālu attēlu un lietotāja aprakstu
void Evaluation::runSingle(string imagePath, string altText, bool classifyComplexity) {

    makeOutputDir();

    cout << "Attels: " << imagePath << endl;
    cout << "Parbaudamais alt teksts: " << altText << endl;

    string reference = florence.generateCaption(imagePath);
    cout << "Florence reference: " << reference << endl;

    /* Ja Florence atgrieza kļūdu, es nevaru jēgpilni turpināt vērtēšanu - ierakstu nulles rindu,
    tāpat kā to jau dara web režīms, kad attēlu neizdodas lejupielādēt. */
    if (reference.size() >= 6 && reference.substr(0, 6) == "ERROR:") {
        cout << "Kluda: Florence negeneraja aprakstu, pilna vertesana netiek veikta." << endl;

        ofstream csv("output/single_result.csv");
        csv << "image,reference,alt_text,precision,recall,f1,bleu,meteor,rouge_l,cider,semantic,final_score,level,purpose" << endl;
        csv << csvSafe(imagePath) << ",";
        csv << csvSafe(reference) << ",";
        csv << csvSafe(altText) << ",";
        csv << "0,0,0,0,0,0,0,0,0,";
        csv << csvSafe("insufficient") << ",";
        csv << csvSafe("n/a") << endl;
        csv.close();

        cout << "Rezultati saglabati: output/single_result.csv" << endl;
        return;
    }

    // Ja sarežģītības noteikšana ir ieslēgta, es izsaucu Florence reģionu noteikšanu
    int regionCount = 0;
    if (classifyComplexity) {
        regionCount = florence.countRegions(imagePath);
        if (regionCount < 0) {
            regionCount = 0;
        }
    }

    // Te es nosaku attēla funkciju. Single režīmā nav <a> konteksta, tāpēc isWrappedInLink vienmēr ir false.
    string purpose;
    if (classifyComplexity) {
        purpose = imagePurpose.classifyFromAlt(altText, false, regionCount);
    }
    else {
        purpose = imagePurpose.classifyFromAlt(altText, false);
    }

    WeightProfile profile = coefficients.getProfile(purpose);
    MetricResult result = scoreTexts(reference, altText, profile);
    result.purpose = purpose;

    printMetric(result);

    ofstream csv("output/single_result.csv");
    csv << "image,reference,alt_text,precision,recall,f1,bleu,meteor,rouge_l,cider,semantic,final_score,level,purpose" << endl;
    csv << csvSafe(imagePath) << ",";
    csv << csvSafe(reference) << ",";
    csv << csvSafe(altText) << ",";
    csv << result.precision << ",";
    csv << result.recall << ",";
    csv << result.f1 << ",";
    csv << result.bleu << ",";
    csv << result.meteor << ",";
    csv << result.rougeL << ",";
    csv << result.cider << ",";
    csv << result.semantic << ",";
    csv << result.finalScore << ",";
    csv << csvSafe(result.level) << ",";
    csv << csvSafe(result.purpose) << endl;
    csv.close();

    cout << "Rezultati saglabati: output/single_result.csv" << endl;
}

// Šī funkcija palaiž reālas tīmekļa lapas režīmu
void Evaluation::runWeb(string url, int limit, bool classifyComplexity) {

    makeOutputDir();

    cout << "Lejupielade HTML no: " << url << endl;
    string html = downloader.downloadText(url);

    if (html.size() == 0) {
        cout << "Kluda: HTML netika lejupieladets" << endl;
        return;
    }

    vector<ImageItem> images = webParser.parseImages(html, url, limit);
    cout << "Atrasti atteli: " << images.size() << endl;

    ofstream csv("output/web_accessibility_results.csv");
    csv << "index,image_url,local_path,alt_text,reference,precision,recall,f1,bleu,meteor,rouge_l,cider,semantic,final_score,level,note,purpose,wrapped_in_anchor" << endl;

    double totalScore = 0;
    int scoredCount = 0;

    // Šis cikls apstrādā katru atrasto attēlu - lejupielādē, ģenerē Florence references aprakstu un salīdzina ar alt tekstu
    for (int i = 0; i < (int)images.size(); i++) {
        cout << endl << "Apstrada attelu " << (i + 1) << " no " << images.size() << endl;

        string localPath = joinPath("output/downloaded_images", makeFileName(i + 1));
        images[i].localPath = localPath;

        bool downloaded = downloader.downloadFile(images[i].imageUrl, localPath);

        // Ja attēlu nevar lejupielādēt, es ierakstu kļūdas rindu un turpinu ar nākamo
        if (!downloaded) {
            csv << (i + 1) << ",";
            csv << csvSafe(images[i].imageUrl) << ",";
            csv << csvSafe(localPath) << ",";
            csv << csvSafe(images[i].altText) << ",";
            csv << csvSafe("") << ",";
            csv << "0,0,0,0,0,0,0,0,0,";
            csv << csvSafe("insufficient") << ",";
            csv << csvSafe("attelu neizdevas lejupieladet") << ",";
            csv << csvSafe("n/a") << ",";
            csv << (images[i].wrappedInAnchor ? "true" : "false") << endl;
            continue;
        }

        // Ja alt teksts ir tukšs, tas pēc definīcijas nozīmē dekoratīvu attēlu - tad es ierakstu rindu bez Florence salīdzināšanas
        if (images[i].altText.size() == 0) {
            csv << (i + 1) << ",";
            csv << csvSafe(images[i].imageUrl) << ",";
            csv << csvSafe(localPath) << ",";
            csv << csvSafe(images[i].altText) << ",";
            csv << csvSafe("") << ",";
            csv << "0,0,0,0,0,0,0,0,0,";
            csv << csvSafe("insufficient") << ",";
            csv << csvSafe("alt teksts nav atrasts vai ir tukss") << ",";
            csv << csvSafe("decorative") << ",";
            csv << (images[i].wrappedInAnchor ? "true" : "false") << endl;
            continue;
        }

        string reference = florence.generateCaption(localPath);

        // Ja Florence atgrieza kļūdu, es ierakstu kļūdas rindu un turpinu ar nākamo attēlu
        if (reference.size() >= 6 && reference.substr(0, 6) == "ERROR:") {
            csv << (i + 1) << ",";
            csv << csvSafe(images[i].imageUrl) << ",";
            csv << csvSafe(localPath) << ",";
            csv << csvSafe(images[i].altText) << ",";
            csv << csvSafe(reference) << ",";
            csv << "0,0,0,0,0,0,0,0,0,";
            csv << csvSafe("insufficient") << ",";
            csv << csvSafe("Florence generesana neizdevas") << ",";
            csv << csvSafe("n/a") << ",";
            csv << (images[i].wrappedInAnchor ? "true" : "false") << endl;
            continue;
        }

        // Ja sarežģītības noteikšana ir ieslēgta, tad esizsaucu Florence reģionu noteikšanu šim lokālajam attēlam
        int regionCount = 0;
        if (classifyComplexity) {
            regionCount = florence.countRegions(localPath);
            if (regionCount < 0) {
                regionCount = 0;
            }
        }

        // Šeit es nosaku attēla funkciju, izmantojot alt tekstu, saites kontekstu un, ja pieejams, reģionu skaitu
        string purpose;
        if (classifyComplexity) {
            purpose = imagePurpose.classifyFromAlt(images[i].altText, images[i].wrappedInAnchor, regionCount);
        }
        else {
            purpose = imagePurpose.classifyFromAlt(images[i].altText, images[i].wrappedInAnchor);
        }

        WeightProfile profile = coefficients.getProfile(purpose);
        MetricResult result = scoreTexts(reference, images[i].altText, profile);
        result.purpose = purpose;

        totalScore += result.finalScore;
        scoredCount++;

        csv << (i + 1) << ",";
        csv << csvSafe(images[i].imageUrl) << ",";
        csv << csvSafe(localPath) << ",";
        csv << csvSafe(images[i].altText) << ",";
        csv << csvSafe(reference) << ",";
        csv << result.precision << ",";
        csv << result.recall << ",";
        csv << result.f1 << ",";
        csv << result.bleu << ",";
        csv << result.meteor << ",";
        csv << result.rougeL << ",";
        csv << result.cider << ",";
        csv << result.semantic << ",";
        csv << result.finalScore << ",";
        csv << csvSafe(result.level) << ",";
        csv << csvSafe(images[i].note) << ",";
        csv << csvSafe(result.purpose) << ",";
        csv << (images[i].wrappedInAnchor ? "true" : "false") << endl;

        cout << "Alt: " << images[i].altText << endl;
        cout << "Reference: " << reference << endl;
        cout << "Purpose: " << purpose << endl;
        cout << "Score: " << fixed << setprecision(2) << result.finalScore << " / 100" << endl;
    }

    csv.close();

    cout << endl << "Web rezims pabeigts" << endl;
    cout << "Vertetie atteli ar alt tekstu: " << scoredCount << endl;

    if (scoredCount > 0) {
        double average = totalScore / scoredCount;
        cout << "Videjais pieejamibas score: " << fixed << setprecision(2) << average << " / 100" << endl;
    }

    cout << "Rezultati saglabati: output/web_accessibility_results.csv" << endl;
}

// Šī funkcija palaiž koeficientu kalibrēšanas režīmu - salīdzina manu score ar cilvēku vērtējumu
void Evaluation::runCalibration(string expertFile, string tokenFile, int limit, bool useCrowdflower, string crowdflowerFile) {

    makeOutputDir();

    // Te ielādēju visus Flickr8k attēlus bez limita - annotāciju fails var norādīt uz jebkuru no tiem
    vector<FlickrItem> flickrItems = flickrLoader.load(tokenFile, 0);

    if (flickrItems.size() == 0) {
        cout << "Kluda: neizdevas ielādēt Flickr8k datus no: " << tokenFile << endl;
        return;
    }

    ensureCorpusStats();

    CorpusStats statsToUse;
    if (corpusStatsAvailable) {
        statsToUse = corpusStats;
    }

    calibration.runCalibration(expertFile, flickrItems, statsToUse, "output/calibration_report.txt", limit, false, metrics, semanticSimilarity);

    // Ja lietotājs pieprasīja arī CrowdFlower kalibrēšanu, veicu to arī
    if (useCrowdflower) {
        string cfFile = crowdflowerFile;

        // Ja lietotājs nenorādīja konkrētu ceļu, tad izmantoju Flickr8k datu kopas noklusējuma ceļu
        if (cfFile.size() == 0) {
            cfFile = "data/Flickr8k_text/CrowdFlowerAnnotations.txt";
        }

        calibration.runCalibration(cfFile, flickrItems, statsToUse, "output/calibration_report_crowdflower.txt", limit, true, metrics, semanticSimilarity);
    }
}
