// Šeit tiek iekļauta galvenā Evaluation klase, ekrāna izvade un teksta tips
#include "Evaluation.h"
#include <iostream>
#include <string>

using namespace std;

//Šī funkcija izvada lietošanas palīdzību
void printHelp() {

    cout << "Ilustraciju aprakstu pieejamibas novertesanas programma" << endl;
    cout << endl;

    // Flickr8k režīma piemērs - noklusējumā ar pilnu datu kopu (limit 0 = visi attēli)
    cout << "1) Flickr8k validacija (ar kludu analizi):" << endl;
    cout << "   app.exe experiment data\\Flickr8k_text\\Flickr8k.token.txt 0" << endl;

    // Viena attēla režīma piemērs
    cout << "2) Viena attela parbaude:" << endl;
    cout << "   app.exe single sample_images\\test.jpg \"A dog is running outside\" --python C:\\path\\test_env\\Scripts\\python.exe" << endl;

    // Web režīma piemērs
    cout << "3) Tiemekla lapas parbaude:" << endl;
    cout << "   app.exe web https://example.com 5 --python C:\\path\\test_env\\Scripts\\python.exe" << endl;

    // Koeficientu kalibrēšanas režīma piemērs
    cout << "4) Koeficientu kalibresana pret ExpertAnnotations.txt (katra rinda ~5s, limit noklusejums 100):" << endl;
    cout << "   app.exe calibrate data\\Flickr8k_text\\ExpertAnnotations.txt data\\Flickr8k_text\\Flickr8k.token.txt --limit 30 --python C:\\path\\test_env\\Scripts\\python.exe" << endl;

    cout << endl;
    cout << "Papildu parametri:" << endl;
    cout << "   --model microsoft/Florence-2-base" << endl;
    cout << "   --semantic-model all-MiniLM-L6-v2" << endl;
    cout << "   --classify-complexity     (ieslēdz Florence regionu skaitisanu single/web rezimos)" << endl;
    cout << "   --limit N                 (calibrate rezima, cik rindas apstradat, 0 = visas)" << endl;
    cout << "   --crowdflower             (calibrate rezima, kalibre ari pret CrowdFlowerAnnotations.txt)" << endl;
    cout << "   --crowdflower-file <celjs>(calibrate rezima, cits CrowdFlower faila celjs)" << endl;
    cout << "   --top-n N                 (experiment rezima, cik sliktako/labako paru paradit, noklusejums 10)" << endl;
}

// Šī funkcija sameklē parametra vērtību argumentu sarakstā
string getArgValue(int argc, char* argv[], string name, string defaultValue) {

    for (int i = 1; i < argc - 1; i++) {
        if (string(argv[i]) == name) {
            return string(argv[i + 1]);
        }
    }

    return defaultValue;
}

// Šī funkcija pārbauda, vai norādītais karogs (bez vērtības) ir klāt argumentu sarakstā
bool hasFlag(int argc, char* argv[], string name) {

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == name) {
            return true;
        }
    }

    return false;
}

// Šī funkcija ir programmas sākuma funkcija
int main(int argc, char* argv[]) {

    // Ja nav pietiekami daudz argumentu, tad es izvadu palīdzību un beidzu
    if (argc < 2) {
        printHelp();
        return 0;
    }

    Evaluation evaluation;

    // Te nolasu Python ceļu, Florence un semantiskās līdzības modeļa nosaukumus, ja lietotājs tos norādīja
    string pythonPath = getArgValue(argc, argv, "--python", "python");
    string modelName = getArgValue(argc, argv, "--model", "microsoft/Florence-2-base");
    string semanticModelName = getArgValue(argc, argv, "--semantic-model", "all-MiniLM-L6-v2");

    evaluation.setPythonExe(pythonPath);
    evaluation.setModelName(modelName);
    evaluation.setSemanticModelName(semanticModelName);

    bool classifyComplexity = hasFlag(argc, argv, "--classify-complexity");
    string mode = argv[1];

    // Flickr8k validācijas režīms
    if (mode == "experiment") {
        if (argc < 4) {
            printHelp();
            return 0;
        }

        string tokenFile = argv[2];
        int limit = stoi(argv[3]);

        // Šis ir - cik sliktāko/labāko pāru rādīt kļūdu analīzē
        string topNText = getArgValue(argc, argv, "--top-n", "10");
        int topN = stoi(topNText);

        evaluation.runExperiment(tokenFile, limit, topN);
        return 0;
    }

    // Viena attēla režīms
    if (mode == "single") {
        if (argc < 4) {
            printHelp();
            return 0;
        }

        string imagePath = argv[2];
        string altText = argv[3];

        evaluation.runSingle(imagePath, altText, classifyComplexity);
        return 0;
    }

    // Tīmekļa lapas režīms
    if (mode == "web") {
        if (argc < 4) {
            printHelp();
            return 0;
        }

        string url = argv[2];
        int limit = stoi(argv[3]);

        evaluation.runWeb(url, limit, classifyComplexity);
        return 0;
    }

    // Koeficientu kalibrēšanas režīms
    if (mode == "calibrate") {
        if (argc < 4) {
            printHelp();
            return 0;
        }

        string expertFile = argv[2];
        string tokenFile = argv[3];

        /* Te ir limits, cik annotāciju rindas apstrādāt. Noklusējums NAV 0 (neierobežots) tāpat kā citiem režīmiem - pārbaudot izrādījās, ka katra rinda 
prasa apmēram 5 sekundes (jauns Python process semantiskajai līdzībai katrai rindai), tāpēc pilna 5822 rindu palaišana bez limita aizņemtu vairākas stundas. 
Lai nejauši nepalaistu tik garu procesu, noklusējums šeit ir 100 rindas (apmēram 8-9 minūtes) - pilnu datu kopu var palaist ar --limit 0, ja tiešām vajag 
un ir laika pagaidīt. */
        string limitText = getArgValue(argc, argv, "--limit", "100");
        int limit = stoi(limitText);

        string crowdflowerFile = getArgValue(argc, argv, "--crowdflower-file", "");

        // Šeit es kalibrēju arī pret CrowdFlower datiem, ja lietotājs to pieprasīja ar karogu vai norādīja failu
        bool useCrowdflower = hasFlag(argc, argv, "--crowdflower") || crowdflowerFile.size() > 0;

        evaluation.runCalibration(expertFile, tokenFile, limit, useCrowdflower, crowdflowerFile);
        return 0;
    }

    // Ja režīms nav atpazīts, es izvadu palīdzību
    printHelp();
    return 0;
}
