// Šeit tiek iekļauta FlickrLoader klases deklarācija, failu lasīšana un vārdnīca aprakstu grupēšanai pēc attēla
#include "FlickrLoader.h"
#include <fstream>
#include <map>

using namespace std;

// Šī funkcija ielādē Flickr8k.token.txt failu
vector<FlickrItem> FlickrLoader::load(string filePath, int limit) {

    vector<FlickrItem> result;
    ifstream file(filePath);

    // Ja failu nevar atvērt, tad atgriežu tukšu sarakstu
    if (!file.is_open()) {
        return result;
    }

    map<string, vector<string>> grouped;  // Šie ir apraksti, sagrupēti pēc attēla nosaukuma
    string line;

    // Šis cikls nolasa failu pa rindām
    while (getline(file, line)) {
        if (line.size() == 0) {
            continue;
        }

        // Te meklēju tabulatoru starp attēla nosaukumu un aprakstu, ja tā nav, mēģinu atstarpi
        size_t tabPos = line.find('\t');
        if (tabPos == string::npos) {
            tabPos = line.find(' ');
        }

        // Ja atdalītāja nav vispār, rinda nav derīga
        if (tabPos == string::npos) {
            continue;
        }

        string imagePart = line.substr(0, tabPos);
        string caption = line.substr(tabPos + 1);

        // Attēla daļa var izskatīties kā "image.jpg#0" - atstāju tikai faila nosaukumu
        size_t hashPos = imagePart.find('#');
        if (hashPos != string::npos) {
            imagePart = imagePart.substr(0, hashPos);
        }

        if (imagePart.size() == 0 || caption.size() == 0) {
            continue;
        }

        grouped[imagePart].push_back(caption);
    }

    file.close();

    // Šis cikls pārvērš sagrupēto karti par FlickrItem sarakstu, ievērojot limitu
    for (auto it = grouped.begin(); it != grouped.end(); it++) {
        if (limit > 0 && (int)result.size() >= limit) {
            break;
        }

        FlickrItem item;
        item.imageName = it->first;
        item.captions = it->second;

        result.push_back(item);
    }

    return result;
}
