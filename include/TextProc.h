#ifndef TEXTPROC_H
#define TEXTPROC_H

// Šeit tiek iekļauts teksta tips, saraksta tips, kopas tips stop-vārdiem un vārdnīcas tips normalizācijai
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

// Šī klase sagatavo tekstu metrikām
class TextProc {
private:
    set<string> stopWords;              // Stop-vārdi, kuri maz palīdz attēla satura salīdzināšanā
    map<string, string> normalWords;    // Vienkārša sinonīmu un formu vārdnīca
    bool isStopWord(string word);       // Pārbaudu, vai vārds ir stop-vārds

public:
    TextProc();                           // Konstruktors sagatavo vārdnīcas
    string toLowerText(string text);      // Tad pārvērš tekstu uz mazajiem burtiem
    string cleanText(string text);        // Tad noņem pieturzīmes un liekus simbolus
    string normalizeWord(string word);    // Tad normalizē vienu vārdu
    vector<string> tokenize(string text); // Un visbeidzot sadala tekstu vārdos
};

#endif
