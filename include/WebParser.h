#ifndef WEBPARSER_H
#define WEBPARSER_H

// Šeit tiek iekļautas kopīgās struktūras, teksta tips un saraksta tips
#include "Types.h"
#include <string>
#include <vector>

using namespace std;

// Šis ir vienkāršs HTML parseris, kas meklē tikai <img> tagus
class WebParser {
private:
    string toLowerText(string text);                   // Šis pārvērš tekstu uz mazajiem burtiem
    string getAttribute(string tag, string name);      // Tad nolasa atribūtu no img taga
    string getFirstFromSrcset(string srcset);          // Tad paņem pirmo attēlu no srcset
    string getBaseUrl(string pageUrl);                 // Tad iegūst lapas bāzes URL
    string resolveUrl(string pageUrl, string src);     // Un pēc tam pārvērš relatīvu ceļu par pilnu URL

    /* Te tiek pārbaudīts, vai attēla tags atrodas iekš <a> saites - meklēju atpakaļ no attēla pozīcijas.
Šī ir vienkārša, ne pilnīgi droša heiristika (nav īsta HTML/DOM koka), jo parseris kopumā ir teksta skenēšana, nevis īsts pārlūks - 
 - tas pats ierobežojums jau ir arī pārējām WebParser funkcijām. */
    bool isImageWrappedInAnchor(string html, int imgTagPosition);

public:
    vector<ImageItem> parseImages(string html, string pageUrl, int limit); // Atrodu img tagus
};

#endif
