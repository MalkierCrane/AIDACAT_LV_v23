#ifndef DOWNLOADER_H
#define DOWNLOADER_H

// Iekļauju teksta tipu
#include <string>

using namespace std;

// Šī klase izmanto curl, lai lejupielādētu HTML un attēlus
class Downloader {
private:
    string quote(string value);        // Teksts tiek ielikts pēdiņās
    string runCommand(string command); // Šis izpilda komandu un nolasa izvadi

public:
    string downloadText(string url);             // Tas lejupielādē tīmekļa lapas HTML
    bool downloadFile(string url, string path);  // Tas lejupielādē attēlu failā
};

#endif
