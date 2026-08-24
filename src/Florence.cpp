/* Šeit tiek iekļauta Florence klases deklarācija, buferis komandas rezultāta lasīšanai, C funkcijas
procesa palaišanai, simbolu funkcijas, failu sistēmas pārbaude, teksta plūsma kļūdu tekstu veidošanai un cstdlib atoi funkcijai. */
#include "Florence.h"
#include <array>
#include <cstdio>
#include <cctype>
#include <filesystem>
#include <sstream>
#include <cstdlib>

using namespace std;

namespace fs = filesystem;

// Windows vidē es izmantoju _popen nosaukumu
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

// Šis konstruktors sagatavo noklusējuma vērtības
Florence::Florence() {

    pythonExe = "python";
    scriptPath = "python/florence_caption.py";
    modelName = "microsoft/Florence-2-base";  // Šī darba prototipam lietoju vieglāko base modeli
}

// Šī funkcija uzstāda Python izpildfaila ceļu
void Florence::setPythonExe(string path) {
    if (path.size() > 0) {
        pythonExe = path;
    }
}

//Šī funkcija uzstāda Florence modeļa nosaukumu
void Florence::setModelName(string model) {
    if (model.size() > 0) {
        modelName = model;
    }
}

//Šī funkcija ieliek tekstu pēdiņās komandrindai
string Florence::quote(string value) {

    string result = "\"";

    for (int i = 0; i < (int)value.size(); i++) {
        if (value[i] == '"') {
            continue;
        }
        result += value[i];
    }

    result += "\"";
    return result;
}

// Šī funkcija izpilda komandu un nolasa tās izvadi
string Florence::runCommand(string command) {

    array<char, 512> buffer;
    string result = "";

    FILE* pipe = popen(command.c_str(), "r");
    if (pipe == nullptr) {
        return "ERROR: could not run Python command";
    }

    while (fgets(buffer.data(), (int)buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    pclose(pipe);

    // Te es noņemu rindas beigu simbolus un liekas atstarpes no gala
    while (result.size() > 0 && isspace((unsigned char)result.back())) {
        result.pop_back();
    }

    return result;
}

/*Šī funkcija noņem diagnostikas rindu no Python izvades, ja tāda tur kaut kā ir nokļuvusi. Galvenais labojums pret šo problēmu ir tas, ka generateCaption un 
countRegions komandām vairs nepievienoju "2>&1" - tāpēc Python puses stderr izvade (piemēram "Florence device: cpu") vispār netiek lasīta caur popen. 
Šī funkcija ir tikai papildu drošība gadījumam, ja kādreiz pievienoju jaunu diagnostikas print() bez pārdomāšanas, kur tas nonāk. */
string Florence::stripDiagnosticLine(string text) {

    size_t newlinePos = text.find('\n');
    if (newlinePos == string::npos) {
        return text;
    }

    string firstLine = text.substr(0, newlinePos);

    // Ja pirmā rinda sākas ar zināmo diagnostikas frāzi, tad atgriežu tekstu bez tās
    if (firstLine.find("Florence device") == 0) {
        return text.substr(newlinePos + 1);
    }

    return text;
}

//Šī funkcija atrod Python skriptu projekta mapē
string Florence::findScript() {

    fs::path path = fs::path(scriptPath);
    if (fs::exists(path)) {
        return fs::absolute(path).string();
    }

    // Te es mēģinu meklēt skriptu no pašreizējās mapes
    fs::path secondPath = fs::current_path() / path;
    if (fs::exists(secondPath)) {
        return fs::absolute(secondPath).string();
    }

    return scriptPath;
}

//Šī funkcija pārbauda, vai fails eksistē
static bool fileExists(string path) {
    if (path.size() == 0) {
        return false;
    }

    return fs::exists(fs::path(path));
}

//Šī funkcija ģenerē aprakstu vienam attēlam
string Florence::generateCaption(string imagePath) {

    string script = findScript();

    // Ja Python ceļš nav vienkārši "python", pārbaudu, vai fails eksistē
    if (pythonExe != "python" && pythonExe != "python.exe") {
        if (!fileExists(pythonExe)) {
            return "ERROR: Python exe not found: " + pythonExe;
        }
    }

    if (!fileExists(script)) {
        return "ERROR: Florence script not found: " + script;
    }

    if (!fileExists(imagePath)) {
        return "ERROR: image file not found: " + imagePath;
    }

    // Te es sāku komandu ar cmd /C, lai Windows korekti apstrādā ceļus pēdiņās
    string command = "cmd /C \"";
    command += quote(pythonExe);
    command += " ";
    command += quote(script);
    command += " ";
    command += quote(imagePath);
    command += " --model ";
    command += quote(modelName);

    /* SVARĪGI: šeit vairs NEpievienoju "2>&1". Iepriekšējā versijā šī rindiņa pāradresēja Python skripta stderr izvadi (piemēram "Florence device: cpu") 
atpakaļ uz stdout, kuru popen nolasa - tas nozīmēja, ka diagnostikas rinda vienmēr piesārņoja izmērīto aprakstu. Python skripts jau pats raksta diagnostiku 
uz stderr tieši tāpēc, lai stdout paliktu tīrs - man vienkārši nevajadzēja to atkal apvienot atpakaļ. */
    command += "\"";

    string caption = runCommand(command);
    caption = stripDiagnosticLine(caption);  // Papildu drošība, ja diagnostikas rinda kaut kā tomēr ir klāt >:(

    if (caption.size() == 0) {
        return "ERROR: Florence did not return caption";
    }

    return caption;
}

/*Šī funkcija izsauc Florence-2 objektu noteikšanas uzdevumu un atgriež atrasto reģionu skaitu. Izmanto <DENSE_REGION_CAPTION> uzdevumu, kas Florence-2 
modelim liek atrast attēla apgabalus. Reģionu skaits ir kā vienkāršs sarežģītības rādītājs ImagePurpose klasei. */
int Florence::countRegions(string imagePath) {

    string script = findScript();

    // Ja Python ceļš nav vienkārši "python", pārbaudu, vai fails eksistē
    if (pythonExe != "python" && pythonExe != "python.exe") {
        if (!fileExists(pythonExe)) {
            return -1;
        }
    }

    if (!fileExists(script)) {
        return -1;
    }

    if (!fileExists(imagePath)) {
        return -1;
    }

    string command = "cmd /C \"";
    command += quote(pythonExe);
    command += " ";
    command += quote(script);
    command += " ";
    command += quote(imagePath);
    command += " --model ";
    command += quote(modelName);

    /* SVARĪGI: šeit es lietoju vienkāršu vārdu "regions", NEVIS īsto Florence-2 marķieri "<DENSE_REGION_CAPTION>" - Iepriekšējā versijā šeit tieši liku 
"<DENSE_REGION_CAPTION>", un Windows cmd.exe simbolus "<" un ">" saprata kā failu novirzīšanas zīmes pat pēdiņu iekšpusē, tāpēc komanda vienmēr izgāzās 
ar "The system cannot find the file specified". Python skripts (florence_caption.py) tagad pats pārvērš "regions" par īsto Florence-2 marķieri. */
    command += " --task ";
    command += quote("regions");

    // Arī šeit nepievienoju 2>&1, lai stderr diagnostika neiejauktos rezultātā
    command += "\"";

    string output = runCommand(command);
    output = stripDiagnosticLine(output);

    if (output.size() == 0 || output.substr(0, 6) == "ERROR:") {
        return -1;
    }

    // Šeit atoi atgriež 0, ja teksts nav derīgs skaitlis, kas šajā gadījumā ir pieņemams noklusējums (nulle reģionu)
    int regionCount = atoi(output.c_str());

    return regionCount;
}
