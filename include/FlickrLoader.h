#ifndef FLICKRLOADER_H
#define FLICKRLOADER_H

// Šeit tiek iekļautas kopīgās struktūras, teksta tips un saraksta tips
#include "Types.h"
#include <string>
#include <vector>

using namespace std;

// Šī klase nolasa Flickr8k.token.txt failu
class FlickrLoader {
public:
    vector<FlickrItem> load(string filePath, int limit); // Ielādēju aprakstus pa attēliem
};

#endif
