#ifndef SEMANTICSIMILARITY_H
#define SEMANTICSIMILARITY_H

// Iekļauju teksta tipu.
#include <string>

using namespace std;

/* Šī klase izsauc Python skriptu ar sentence-transformers modeli, lai aprēķinātu semantisko līdzību starp diviem tekstiem.
Uzbūve ir tāda pati kā Florence klasei, jo abas klases dara vienu un to pašu lietu - palaiž Python skriptu un nolasa tā izvadi - tikai atšķiras modelis un ievaddati. */
class SemanticSimilarity {
private:
    string pythonExe;                   // Python izpildfaila ceļš
    string scriptPath;                  // semantic_similarity.py skripta ceļš
    string modelName;                   // sentence-transformers modeļa nosaukums
    string quote(string value);         // Ielieku tekstu pēdiņās komandrindai
    string runCommand(string command);  // Izpildu komandu un nolasu izvadi
    string findScript();                // Atrodu Python skriptu projekta mapē

public:
    SemanticSimilarity();               // Tas ir konstruktors ar noklusējuma vērtībām
    void setPythonExe(string path);     // Tas uzstāda Python ceļu
    void setModelName(string model);    // Tas uzstāda modeļa nosaukumu

    /* Te tiek aprēķināta semantisko līdzība starp diviem tekstiem, vērtība no 0 līdz 1.
    Atgriež -1.0, ja radās kļūda (piemēram, Python skripts nav atrasts). */
    double computeSimilarity(string text1, string text2);
};

#endif
