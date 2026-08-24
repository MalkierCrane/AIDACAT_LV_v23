#ifndef FLORENCE_H
#define FLORENCE_H

// Iekļauju teksta tipu
#include <string>

using namespace std;

// Šī klase izsauc Python skriptu ar Florence-2 modeli
class Florence {
private:
    string pythonExe;                  // Python izpildfaila ceļš
    string scriptPath;                 // Florence Python skripta ceļš
    string modelName;                  // Florence modeļa nosaukums
    string quote(string value);        // Teik ielikts teksts pēdiņās komandrindai
    string runCommand(string command); // Tiek izpildīta komanda un nolasīta izvade
    string findScript();               // Tiek atrasts Python skripts projekta mapē

    /* Šeit tiek noņemta diagnostikas rinda (piemēram "Florence device: cpu"), ja tā kaut kā tomēr nokļūtu izvadē kā iepriekš.
    Šis ir papildu drošības solis - galvenais labojums ir tas, ka komandai vairs nepievienoju 2>&1, tāpēc stderr diagnostika normālā gadījumā vispār netiek nolasīta. */
    string stripDiagnosticLine(string text);

public:
    Florence();                               // Konstruktors ar noklusējuma vērtībām
    void setPythonExe(string path);           // Šis uzstāda Python ceļu
    void setModelName(string model);          // Šis uzstāda modeļa nosaukumu
    string generateCaption(string imagePath); // Un šis ģenerē aprakstu vienam attēlam

    /* Te tiek izsaukts Florence-2 objektu noteikšanas uzdevums un atgriezts atrasto reģionu skaits
    To izmanto ImagePurpose klase, lai novērtētu attēla sarežģītību (complex kategorija). Atgriež -1, ja radās kļūda. */
    int countRegions(string imagePath);
};

#endif
