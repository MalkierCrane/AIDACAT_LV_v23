// Šeit tiek iekļauta Downloader klases deklarācija, buferis un C funkcijas komandas palaišanai un izvades nolasīšanai, un failu sistēmas pārbaude
#include "Downloader.h"
#include <array>
#include <cstdio>
#include <filesystem>

using namespace std;

namespace fs = filesystem;

// Windows vidē es izmantoju _popen nosaukumu
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

// Šī funkcija ieliek tekstu pēdiņās komandrindai
string Downloader::quote(string value) {

    string result = "\"";

    // Šis cikls kopē katru simbolu, izlaižot pēdiņas, lai tās nesagrieztu komandrindas argumentu
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
string Downloader::runCommand(string command) {

    array<char, 512> buffer;
    string result = "";

    FILE* pipe = popen(command.c_str(), "r");

    // Ja procesu nevar palaist, atgriežu tukšu tekstu
    if (pipe == nullptr) {
        return "";
    }

    // Šis cikls nolasa komandas izvadi rindu pa rindai
    while (fgets(buffer.data(), (int)buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    pclose(pipe);

    return result;
}

// Šī funkcija lejupielādē HTML tekstu ar curl
string Downloader::downloadText(string url) {

    string command = "curl -L -s ";
    command += quote(url);

    string html = runCommand(command);

    return html;
}

// Šī funkcija lejupielādē attēla failu ar curl
bool Downloader::downloadFile(string url, string path) {

    string command = "curl -L -s -o ";
    command += quote(path);
    command += " ";
    command += quote(url);

    runCommand(command);

    // Pārbaudu, vai fails patiešām parādījās pēc lejupielādes
    if (fs::exists(path)) {
        return true;
    }

    return false;
}
