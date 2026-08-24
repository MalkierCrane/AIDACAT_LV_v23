// Šeit tiek iekļauta ImagePurpose klases deklarācija, teksta plūsma vārdu sadalīšanai un simbolu funkcijas pieturzīmju noņemšanai
#include "ImagePurpose.h"
#include <sstream>
#include <cctype>

using namespace std;

// Šis konstruktors sagatavo sarakstu ar vārdiem, kas liecina par funkcionālu attēlu (poga, saite, ikona ar darbību), nevis vienkārši informatīvu ilustrāciju
ImagePurpose::ImagePurpose() {

    // Šie ir biežākie "darbības" vārdi, ko mēdz likt pogu un saišu alt tekstos
    functionalKeywords.push_back("click");
    functionalKeywords.push_back("submit");
    functionalKeywords.push_back("search");
    functionalKeywords.push_back("download");
    functionalKeywords.push_back("play");
    functionalKeywords.push_back("menu");
    functionalKeywords.push_back("button");
    functionalKeywords.push_back("link");
    functionalKeywords.push_back("close");
    functionalKeywords.push_back("open");
    functionalKeywords.push_back("login");
    functionalKeywords.push_back("logout");
    functionalKeywords.push_back("sign in");
    functionalKeywords.push_back("sign up");
    functionalKeywords.push_back("next");
    functionalKeywords.push_back("previous");
    functionalKeywords.push_back("cart");
    functionalKeywords.push_back("checkout");
}

/* Šī funkcija pārbauda, vai alt tekstā (jau mazajos burtos) parādās kāds no funkcionālajiem atslēgvārdiem.
PIEZĪME PAR VESELU VĀRDU SALĪDZINĀŠANU: Iepriekšējā versijā šeit vienkārši meklēju atslēgvārdu kā apakšvirkni jebkur teksta iekšā (altTextLower.find(...)). 
Tas izrādījās kļūdains - piemēram, pilnīgi parasts apraksts "A child is playing outside" tika klasificēts kā "functional" tikai tāpēc, ka vārds "play" ir 
sastopams vārda "playing" IEKŠPUSĒ. Tas pats būtu noticis ar "display", "playground" un līdzīgiem vārdiem. Tāpēc tagad viena vārda atslēgvārdus es salīdzinu 
ar KATRU atsevišķu alt teksta vārdu tieši (ne kā apakšvirkni), bet frāzes (piemēram "sign in", kur ir atstarpe) joprojām meklēju kā apakšvirkni, jo 
salīdzināt veselu frāzi ar vienu vārdu nav jēgas. */
bool ImagePurpose::containsFunctionalKeyword(string altTextLower) {

    // Šis cikls iet cauri visiem atslēgvārdiem
    for (int i = 0; i < (int)functionalKeywords.size(); i++) {
        string keyword = functionalKeywords[i];

        // Ja atslēgvārds satur atstarpi, tā ir frāze - es meklēju to kā apakšvirkni
        if (keyword.find(' ') != string::npos) {
            if (altTextLower.find(keyword) != string::npos) {
                return true;
            }

            continue;
        }

        // Ja atslēgvārds ir viens vārds, tad sadalu alt tekstu atsevišķos vārdos un salīdzinu katru
        stringstream ss(altTextLower);
        string word;

        while (ss >> word) {

            // Te es noņemu pieturzīmes no vārda sākuma un beigām (piemēram, "play." vai "play,")
            while (word.size() > 0 && !isalnum((unsigned char)word[word.size() - 1])) {
                word = word.substr(0, word.size() - 1);
            }
            while (word.size() > 0 && !isalnum((unsigned char)word[0])) {
                word = word.substr(1);
            }

            if (word == keyword) {
                return true;
            }
        }
    }

    return false;
}

// Šī funkcija nosaka attēla funkciju bez Florence reģionu skaita. To izmanto single režīmā, kur nav zināms, vai attēls ir ietverts saitē.
string ImagePurpose::classifyFromAlt(string altText, bool isWrappedInLink) {

    /* Te izsaucu pilno versiju ar regionCount=0, kas nozīmē "informācija nav pieejama", nevis "attēlā nav neviena reģiona" - complex kategorija šeit 
tātad var iekrist tikai caur garā alt teksta noteikumu, nevis caur reģionu skaitu. */
    return classifyFromAlt(altText, isWrappedInLink, 0);
}

// Šī funkcija nosaka attēla funkciju, izmantojot alt tekstu, saites kontekstu un, ja pieejams, Florence-2 atrasto reģionu skaitu attēlā
string ImagePurpose::classifyFromAlt(string altText, bool isWrappedInLink, int regionCount) {

    string altLower = textProc.toLowerText(altText);  // Alt teksts mazajos burtos, lai meklēšana nebūtu atkarīga no reģistra

    // Šeit es noņemu liekas atstarpes no gala un sākuma, lai pārbaudītu, vai teksts patiešām ir tukšs
    string trimmed = altLower;
    while (trimmed.size() > 0 && trimmed[trimmed.size() - 1] == ' ') {
        trimmed = trimmed.substr(0, trimmed.size() - 1);
    }
    while (trimmed.size() > 0 && trimmed[0] == ' ') {
        trimmed = trimmed.substr(1);
    }

    // Ja alt teksts pēc atstarpju noņemšanas ir tukšs, attēls ir dekoratīvs - pieejamības vadlīnijās tukšs alt="" tieši nozīmē, ka attēlam nav satura nozīmes
    if (trimmed.size() == 0) {
        return "decorative";
    }

    // Ja attēls ir ietverts saitē, vai alt tekstā ir kāds darbības vārds, tas ir funkcionāls attēls
    if (isWrappedInLink || containsFunctionalKeyword(altLower)) {
        return "functional";
    }

    // Ja alt teksts ir ļoti garš (120 vai vairāk simboli), es pieņemu, ka attēls ir sarežģīts (piemēram, grafiks vai diagramma, kam vajadzīgs garš apraksts)
    if ((int)altText.size() >= 120) {
        return "complex";
    }

    // Ja Florence atrada 5 vai vairāk reģionus attēlā, tas arī liecina par sarežģītu attēlu
    if (regionCount >= 5) {
        return "complex";
    }

    // Ja neviens no iepriekšējiem noteikumiem neiestājas, es uzskatu attēlu par parastu informatīvu attēlu
    return "informative";
}
