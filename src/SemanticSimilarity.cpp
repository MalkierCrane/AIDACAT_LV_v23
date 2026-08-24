/* Šeit tiek iekļauta SemanticSimilarity klases deklarācija, buferis komandas rezultāta lasīšanai, C funkcijas
procesa palaišanai, simbolu funkcijas, failu sistēmas pārbaude un cstdlib atof funkcijai. */
#include "SemanticSimilarity.h"
#include <array>
#include <cstdio>
#include <cctype>
#include <filesystem>
#include <cstdlib>

using namespace std;

namespace fs = filesystem;

// Windows vidē es izmantoju _popen nosaukumu
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

// Šis konstruktors sagatavo noklusējuma vērtības
SemanticSimilarity::SemanticSimilarity() {

    pythonExe = "python";
    scriptPath = "python/semantic_similarity.py";
    modelName = "all-MiniLM-L6-v2";  // Neliels, ātrs sentence-transformers modelis, kas der šim prototipam
}

// Šī funkcija uzstāda Python izpildfaila ceļu
void SemanticSimilarity::setPythonExe(string path) {
    if (path.size() > 0) {
        pythonExe = path;
    }
}

// Šī funkcija uzstāda modeļa nosaukumu
void SemanticSimilarity::setModelName(string model) {
    if (model.size() > 0) {
        modelName = model;
    }
}

// Šī funkcija ieliek tekstu pēdiņās komandrindai
string SemanticSimilarity::quote(string value) {

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
string SemanticSimilarity::runCommand(string command) {

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

// Šī funkcija atrod Python skriptu projekta mapē
string SemanticSimilarity::findScript() {

    fs::path path = fs::path(scriptPath);
    if (fs::exists(path)) {
        return fs::absolute(path).string();
    }

    // Šeit es mēģinu meklēt skriptu no pašreizējās mapes
    fs::path secondPath = fs::current_path() / path;
    if (fs::exists(secondPath)) {
        return fs::absolute(secondPath).string();
    }

    return scriptPath;
}

// Šī funkcija pārbauda, vai fails eksistē
static bool fileExists(string path) {
    if (path.size() == 0) {
        return false;
    }

    return fs::exists(fs::path(path));
}

// Šī funkcija aprēķina semantisko līdzību starp diviem tekstiem
double SemanticSimilarity::computeSimilarity(string text1, string text2) {

    string script = findScript();

    // Ja Python ceļš nav vienkārši "python", pārbaudu, vai fails eksistē
    if (pythonExe != "python" && pythonExe != "python.exe") {
        if (!fileExists(pythonExe)) {
            return -1.0;
        }
    }

    if (!fileExists(script)) {
        return -1.0;
    }

    // Ja kāds no tekstiem ir tukšs, semantiskā līdzība nav jēgpilna
    if (text1.size() == 0 || text2.size() == 0) {
        return 0.0;
    }

    // Te es sāku komandu ar cmd /C, lai Windows korekti apstrādā ceļus un pēdiņas
    string command = "cmd /C \"";
    command += quote(pythonExe);
    command += " ";
    command += quote(script);
    command += " ";
    command += quote(text1);
    command += " ";
    command += quote(text2);
    command += " --model ";
    command += quote(modelName);

    // Šeit apzināti NEpievienoju "2>&1" - Python skripts diagnostiku raksta uz stderr, un tā man stdout pusē nav vajadzīga, tāpat kā Florence klasē
    command += "\"";

    string output = runCommand(command);

    if (output.size() == 0 || output.substr(0, 6) == "ERROR:") {
        return -1.0;
    }

    // Šeit es pārvēršu tekstu par skaitli ar peldošo komatu
    double similarity = atof(output.c_str());

    return similarity;
}
