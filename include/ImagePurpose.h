#ifndef IMAGEPURPOSE_H
#define IMAGEPURPOSE_H

// Šeit tiek iekļauta teksta apstrādes klase mazo burtu pārvēršanai, teksta tipu un saraksta tipu
#include "TextProc.h"
#include <string>
#include <vector>

using namespace std;

/* Šī klase nosaka attēla funkciju - decorative / informative / functional / complex.
Šis ir vienkāršots tuvinājums W3C WAI "alt lēmumu koka" idejai (https://www.w3.org/WAI/tutorials/images/decision-tree/), nevis pilna tā realizācija
Reālas pieejamības ekspertīzes vietā es izmantoju dažus vienkāršus, viegli izskaidrojamus noteikumus, kurus var salīdzināt ar W3C WAI attēlu pamācības lapām, 
kas sadalītas tieši pēc šīm pašām kategorijām. */
class ImagePurpose {
private:
    bool containsFunctionalKeyword(string altTextLower); // Pārbaudu, vai alt tekstā ir kāds "darbības" vārds

public:
    TextProc textProc;                    // Teksta apstrādes objekts mazo burtu pārvēršanai.
    vector<string> functionalKeywords;    // Šie ir vārdi, kas liecina, ka attēls ir poga vai saite, nevis vienkārši ilustrācija

    ImagePurpose(); // Konstruktors sagatavo atslēgvārdu sarakstu

    // Nosaku funkciju bez Florence reģionu skaita - tiek pielietots single režīmā, kur nav <a> konteksta
    string classifyFromAlt(string altText, bool isWrappedInLink);

    // Nosaku funkciju ar Florence reģionu skaitu - tiek pielietots web režīmā, ja --classify-complexity ir ieslēgts
    string classifyFromAlt(string altText, bool isWrappedInLink, int regionCount);
};

#endif
