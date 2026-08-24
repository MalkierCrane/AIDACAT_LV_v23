// Šeit tiek iekļauta TextProc klases deklarācija, simbolu funkcijas un teksta plūsma vārdu sadalīšanai
#include "TextProc.h"
#include <cctype>
#include <sstream>

using namespace std;

// Šis konstruktors sagatavo stop-vārdus un vienkāršu normalizācijas vārdnīcu
TextProc::TextProc() {

    // Šie ir biežākie angļu stop-vārdi, kas maz palīdz attēla satura salīdzināšanā
    stopWords.insert("a");
    stopWords.insert("an");
    stopWords.insert("the");
    stopWords.insert("is");
    stopWords.insert("are");
    stopWords.insert("was");
    stopWords.insert("were");
    stopWords.insert("in");
    stopWords.insert("on");
    stopWords.insert("at");
    stopWords.insert("of");
    stopWords.insert("to");
    stopWords.insert("with");
    stopWords.insert("and");
    stopWords.insert("or");
    stopWords.insert("by");
    stopWords.insert("for");
    stopWords.insert("into");
    stopWords.insert("from");
    stopWords.insert("near");
    stopWords.insert("next");
    stopWords.insert("front");
    stopWords.insert("while");

    // Vienkāršas daudzskaitļa un darbības formas, ko normalizēju uz vienskaitļa vai pamatformu
    normalWords["dogs"] = "dog";
    normalWords["puppy"] = "dog";
    normalWords["puppies"] = "dog";
    normalWords["men"] = "man";
    normalWords["women"] = "woman";
    normalWords["boys"] = "boy";
    normalWords["girls"] = "girl";
    normalWords["children"] = "child";
    normalWords["kids"] = "child";
    normalWords["kid"] = "child";
    normalWords["boy"] = "child";
    normalWords["girl"] = "child";
    normalWords["running"] = "run";
    normalWords["runs"] = "run";
    normalWords["ran"] = "run";
    normalWords["playing"] = "play";
    normalWords["plays"] = "play";
    normalWords["played"] = "play";
    normalWords["jumping"] = "jump";
    normalWords["jumps"] = "jump";
    normalWords["jumped"] = "jump";
    normalWords["leaping"] = "jump";
    normalWords["leaps"] = "jump";
    normalWords["climbing"] = "climb";
    normalWords["climbs"] = "climb";
    normalWords["walking"] = "walk";
    normalWords["walks"] = "walk";
    normalWords["standing"] = "stand";
    normalWords["stands"] = "stand";
    normalWords["sitting"] = "sit";
    normalWords["sits"] = "sit";
    normalWords["bicycle"] = "bike";
    normalWords["bicycles"] = "bike";
    normalWords["motorcycle"] = "bike";
    normalWords["football"] = "ball";
    normalWords["soccer"] = "ball";
}

// Šī funkcija pārbauda, vai vārds ir stop-vārds
bool TextProc::isStopWord(string word) {
    return stopWords.find(word) != stopWords.end();
}

// Šī funkcija pārvērš tekstu uz mazajiem burtiem
string TextProc::toLowerText(string text) {

    string result = "";

    // Šis cikls pārvērš katru simbolu uz mazo burtu
    for (int i = 0; i < (int)text.size(); i++) {
        result += (char)tolower((unsigned char)text[i]);
    }

    return result;
}

// Šī funkcija noņem pieturzīmes un liekus simbolus
string TextProc::cleanText(string text) {

    string result = "";
    text = toLowerText(text);

    // Šis cikls iet cauri visiem simboliem, atstājot burtus un ciparus, pārējo aizstājot ar atstarpi
    for (int i = 0; i < (int)text.size(); i++) {
        char c = text[i];

        if (isalnum((unsigned char)c)) {
            result += c;
        }
        else {
            result += ' ';
        }
    }

    return result;
}

// Šī funkcija normalizē vienu vārdu
string TextProc::normalizeWord(string word) {

    // Ja vārds ir vārdnīcā, tad es atgriežu tā aizvietojumu
    if (normalWords.find(word) != normalWords.end()) {
        return normalWords[word];
    }

    // Ja vārds beidzas ar s un ir garāks par 3 simboliem, noņemu beigu s
    if (word.size() > 3 && word[word.size() - 1] == 's') {
        return word.substr(0, word.size() - 1);
    }

    return word;
}

// Šī funkcija sadala tekstu vārdos
vector<string> TextProc::tokenize(string text) {

    vector<string> words;
    string clean = cleanText(text);
    stringstream ss(clean);
    string word;

    // Šis cikls nolasa katru vārdu, normalizē to un izlaiž pārāk īsus vārdus un stop-vārdus
    while (ss >> word) {
        word = normalizeWord(word);

        if (word.size() < 2) {
            continue;
        }

        if (isStopWord(word)) {
            continue;
        }

        words.push_back(word);
    }

    return words;
}
