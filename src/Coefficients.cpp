// Šeit tiek iekļauta Coefficients klases deklarācija, matemātikas funkcija absolūtai vērtībai un ekrāna izvade brīdinājumam
#include "Coefficients.h"
#include <cmath>
#include <iostream>

using namespace std;

// Šī funkcija izveido vienu svaru profilu no 8 skaitļiem, lai konstruktorā nebūtu jāraksta katrs lauks atsevišķi katram profilam
WeightProfile Coefficients::makeProfile(string name, double wp, double wr, double wf1, double wb, double wm, double wrl, double wc, double ws) {

    WeightProfile profile;
    profile.name = name;

    profile.wPrecision = wp;
    profile.wRecall = wr;
    profile.wF1 = wf1;
    profile.wBleu = wb;
    profile.wMeteor = wm;
    profile.wRougeL = wrl;
    profile.wCider = wc;
    profile.wSemantic = ws;

    warnIfWeightsDontSumToOne(profile);

    return profile;
}

/* Šī funkcija pārbauda, vai profila svari summējas tuvu 1.0, un brīdina, ja nē. Tas nepārtrauc programmu, jo šī ir tikai palīdzība pašam sev, lai nepalaistu
garām drukas kļūdu profila skaitļos */
void Coefficients::warnIfWeightsDontSumToOne(WeightProfile profile) {

    double sum = profile.wPrecision + profile.wRecall + profile.wF1 + profile.wBleu +
                 profile.wMeteor + profile.wRougeL + profile.wCider + profile.wSemantic;

    double diff = fabs(sum - 1.0);

    // Ja atšķirība ir pārāk liela, izdrukāju brīdinājumu konsolē, bet neapturu programmu
    if (diff > 0.001) {
        cout << "Brīdinājums: profila '" << profile.name << "' svaru summa ir " << sum << ", nevis 1.0" << endl;
    }
}

/* Konstruktors sagatavo 4 sākotnējos svaru profilus. Tikai "informative" profils tiek pēc tam pielāgots ar calibrate režīmu - pārējie ir pamatoti, 
bet nepārbaudīti pieņēmumi, kas balstīti uz W3C WAI alt-teksta lēmumu koku. */
Coefficients::Coefficients() {

    // Informatīvam attēlam svarīga ir precizitāte un pilnība, tāpēc precision/recall/semantic ir augsti
    profiles.push_back(makeProfile("informative", 0.13, 0.13, 0.09, 0.13, 0.17, 0.13, 0.09, 0.13));

    /* Dekoratīvam attēlam parasti alt teksts vajadzētu būt tukšam, tāpēc semantic un precision svars ir augstāks,
    lai sodītu par liekiem, nevajadzīgiem aprakstiem */
    profiles.push_back(makeProfile("decorative", 0.18, 0.10, 0.10, 0.10, 0.12, 0.10, 0.10, 0.20));

    // Funkcionālam attēlam (poga, saite) svarīga ir darbība, ko attēls veic, tāpēc semantic svars ir vislielākais
    profiles.push_back(makeProfile("functional", 0.20, 0.08, 0.10, 0.10, 0.12, 0.10, 0.05, 0.25));

    // Sarežģītam attēlam (grafiks, diagramma) svarīgs ir recall - vai apraksts pārklāj visu saturu
    profiles.push_back(makeProfile("complex", 0.10, 0.20, 0.10, 0.10, 0.15, 0.10, 0.15, 0.10));
}

// Šī funkcija atrod profilu pēc nosaukuma
WeightProfile Coefficients::getProfile(string purposeName) {

    for (int i = 0; i < (int)profiles.size(); i++) {
        if (profiles[i].name == purposeName) {
            return profiles[i];
        }
    }

    // Ja profils nav atrasts (piemēram, purpose="n/a" vai drukas kļūda), atgriežu noklusējuma profilu
    return getDefaultProfile();
}

/* Funkcija atgriež noklusējuma profilu, ko izmanto experiment un calibrate režīmos, kur attēla funkcija netiek noteikta. 
Paņemu pirmo sarakstā, nevis meklēju pēc vārda vēlreiz, lai izvairītos no bezgalīgas rekursijas, ja kāds nejauši izdzēstu "informative" profilu. */
WeightProfile Coefficients::getDefaultProfile() {

    WeightProfile fallback = makeProfile("informative", 0.13, 0.13, 0.09, 0.13, 0.17, 0.13, 0.09, 0.13);

    if (profiles.size() == 0) {
        return fallback;
    }

    for (int i = 0; i < (int)profiles.size(); i++) {
        if (profiles[i].name == "informative") {
            return profiles[i];
        }
    }

    // Ja "informative" profila nav, atgriežu pirmo pieejamo profilu
    return profiles[0];
}
