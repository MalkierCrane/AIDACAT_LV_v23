#ifndef COEFFICIENTS_H
#define COEFFICIENTS_H

// Te tiek iekļautas kopīgās struktūras, tai skaitā WeightProfile, teksta tips un saraksta tips
#include "Types.h"
#include <string>
#include <vector>

using namespace std;

/* Šī klase glabā nosaukto svaru profilu sarakstu. Katram attēla funkcijas veidam (decorative/informative/functional/complex) ir savs
svaru profils, jo, piemēram, dekoratīvam attēlam labs rezultāts nozīmē īsu vai tukšu aprakstu, bet informatīvam attēlam labs rezultāts nozīmē precīzu un pilnīgu aprakstu.
Tikai "informative" profils ir kalibrēts pret ExpertAnnotations.txt datiem (skat. Calibration klasi) - pārējie trīs profili ir pamatoti,
bet nav empīriski pārbaudīti, jo Flickr8k datos nav marķējuma pēc attēla funkcijas. */
class Coefficients {
private:
    WeightProfile makeProfile(string name, double wp, double wr, double wf1, double wb, double wm, double wrl, double wc, double ws);
    // Pārbaudu, vai profila 8 svari summējas tuvu 1.0, un brīdina konsolē, ja nē (bet neapstājas).
    void warnIfWeightsDontSumToOne(WeightProfile profile);

public:
    vector<WeightProfile> profiles; // Šeit ir visi zināmie svaru profili

    Coefficients();                               // Konstruktors sagatavo 4 sākotnējos profilus
    WeightProfile getProfile(string purposeName); // Atrod profilu pēc nosaukuma, noklusējums "informative"
    WeightProfile getDefaultProfile();            // Atgriež "informative" profilu
};

#endif
