// Šeit tiek iekļauta WebParser klases deklarācija un simbolu funkcijas
#include "WebParser.h"
#include <cctype>

using namespace std;

// Šī funkcija pārvērš tekstu uz mazajiem burtiem
string WebParser::toLowerText(string text) {

    string result = "";

    for (int i = 0; i < (int)text.size(); i++) {
        result += (char)tolower((unsigned char)text[i]);
    }

    return result;
}

// Šī funkcija nolasa atribūta vērtību no viena HTML taga
string WebParser::getAttribute(string tag, string name) {

    string lower = toLowerText(tag);
    string search = toLowerText(name) + "=";

    size_t pos = lower.find(search);
    if (pos == string::npos) {
        return "";
    }
    pos = pos + search.size();

    // Te es izlaižu atstarpes starp atribūta nosaukumu un vērtību
    while (pos < tag.size() && isspace((unsigned char)tag[pos])) {
        pos++;
    }

    if (pos >= tag.size()) {
        return "";
    }

    char quoteChar = tag[pos];

    // Ja vērtība ir pēdiņās, es to nolasu līdz nākamajai tāda paša veida pēdiņai
    if (quoteChar == '"' || quoteChar == '\'') {
        pos++;

        size_t end = tag.find(quoteChar, pos);
        if (end == string::npos) {
            return "";
        }

        return tag.substr(pos, end - pos);
    }

    // Ja nav pēdiņu, tad nolasu līdz atstarpei vai taga beigām
    size_t end = pos;
    while (end < tag.size() && !isspace((unsigned char)tag[end]) && tag[end] != '>') {
        end++;
    }

    return tag.substr(pos, end - pos);
}

// Šī funkcija no srcset paņem pirmo attēla adresi
string WebParser::getFirstFromSrcset(string srcset) {
    if (srcset.size() == 0) {
        return "";
    }

    // Ja ir vairāki attēli, atdalīti ar komatu, es paturu tikai pirmo
    size_t comma = srcset.find(',');
    string first = srcset;
    if (comma != string::npos) {
        first = srcset.substr(0, comma);
    }

    // Te nogriežu izmēra daļu aiz atstarpes, piemēram "300w"
    size_t space = first.find(' ');
    if (space != string::npos) {
        first = first.substr(0, space);
    }

    return first;
}

// Šī funkcija iegūst lapas bāzes URL
string WebParser::getBaseUrl(string pageUrl) {

    size_t protocol = pageUrl.find("://");
    if (protocol == string::npos) {
        return pageUrl;
    }

    size_t slash = pageUrl.find('/', protocol + 3);
    if (slash == string::npos) {
        return pageUrl;
    }

    return pageUrl.substr(0, slash);
}

// Šī funkcija pārvērš relatīvu attēla ceļu par pilnu URL
string WebParser::resolveUrl(string pageUrl, string src) {
    if (src.size() == 0) {
        return "";
    }

    if (src.find("http://") == 0 || src.find("https://") == 0) {
        return src;
    }

    // Ja URL sākas ar //, tas ir protokolam neitrāls ceļš - pievienoju https
    if (src.find("//") == 0) {
        return "https:" + src;
    }

    string base = getBaseUrl(pageUrl);

    if (src[0] == '/') {
        return base + src;
    }

    return base + "/" + src;
}

/* Šī funkcija pārbauda, vai attēla tags atrodas iekš <a> saites. Meklēju atpakaļ no attēla pozīcijas tuvāko derīgu "<a" tagu (ne "<article" vai "<aside"),
un tad pārbaudu, vai starp šo "<a" un attēlu ir arī "</a" beigu tags - ja nav, saite vēl nav aizvērta, tātad attēls ir tās iekšpusē. 
Šī nav īsta HTML koka pārbaude, tikai teksta meklēšana, tāpat kā visa pārējā WebParser klase. */
bool WebParser::isImageWrappedInAnchor(string html, int imgTagPosition) {

    string lower = toLowerText(html);
    size_t searchPos = (size_t)imgTagPosition;
    size_t openPos = string::npos;

    // Šis cikls meklē atpakaļ pēc derīga <a> taga, izlaižot "<article"/"<aside" gadījumus
    while (true) {
        size_t candidate = lower.rfind("<a", searchPos);
        if (candidate == string::npos) {
            break;
        }

        // Te es paņemu simbolu tūlīt aiz "<a", lai pārbaudītu, vai tas tiešām ir <a> tags
        char afterA = ' ';
        if (candidate + 2 < lower.size()) {
            afterA = lower[candidate + 2];
        }

        // Ja aiz "<a" ir atstarpe, ">" vai tabulācija, šis ir īsts saites tags, nevis "<article" vai "<aside"
        if (afterA == ' ' || afterA == '>' || afterA == '\t' || afterA == '\n') {
            openPos = candidate;
            break;
        }

        if (candidate == 0) {
            break;
        }

        searchPos = candidate - 1;
    }

    if (openPos == string::npos) {
        return false;
    }

    // Te meklēju tuvāko saites beigu tagu "</a" pirms attēla taga
    size_t closePos = lower.rfind("</a", (size_t)imgTagPosition);

    // Ja beigu taga nav, vai tas ir tālāk atpakaļ nekā sākuma tags, saite vēl nav aizvērta
    if (closePos == string::npos || closePos < openPos) {
        return true;
    }

    return false;
}

// Šī funkcija atrod img tagus HTML tekstā
vector<ImageItem> WebParser::parseImages(string html, string pageUrl, int limit) {

    vector<ImageItem> result;
    string lower = toLowerText(html);
    size_t pos = 0;

    // Šis cikls meklē visus img tagus, kamēr nav sasniegts limits
    while (true) {
        if (limit > 0 && (int)result.size() >= limit) {
            break;
        }

        size_t start = lower.find("<img", pos);
        if (start == string::npos) {
            break;
        }

        size_t end = lower.find('>', start);
        if (end == string::npos) {
            break;
        }

        string tag = html.substr(start, end - start + 1);
        string src = getAttribute(tag, "src");

        // Ja src nav tieši, mēģinu data-src, tad srcset
        if (src.size() == 0) {
            src = getAttribute(tag, "data-src");
        }
        if (src.size() == 0) {
            string srcset = getAttribute(tag, "srcset");
            src = getFirstFromSrcset(srcset);
        }

        ImageItem item;
        item.pageUrl = pageUrl;
        item.src = src;
        item.imageUrl = resolveUrl(pageUrl, src);
        item.altText = getAttribute(tag, "alt");

        if (item.altText.size() == 0) {
            item.note = "alt teksts nav atrasts vai ir tukss";
        }
        else {
            item.note = "ok";
        }

        // Šis pārbauda, vai attēls atrodas iekš <a> saites - tas vajadzīgs ImagePurpose klasei
        item.wrappedInAnchor = isImageWrappedInAnchor(html, (int)start);
        item.purpose = "";  // Attēla funkciju vēl nenosaku šajā solī, to izdara ImagePurpose vēlāk

        if (item.imageUrl.size() > 0) {
            result.push_back(item);
        }

        pos = end + 1;
    }

    return result;
}
