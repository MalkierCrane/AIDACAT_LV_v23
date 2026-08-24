# Projekta struktura

Šis fails apraksta projekta mapes, failus un to nozīmi.

Projekta paredzamais ceļš:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23
```

---

## 1. Kopējā struktūra

```text
AIDACAT_LV_v23/
├── .vscode/
│   └── tasks.json
├── include/
│   ├── Types.h
│   ├── TextProc.h
│   ├── Metrics.h
│   ├── FlickrLoader.h
│   ├── Florence.h
│   ├── Downloader.h
│   ├── WebParser.h
│   ├── SemanticSimilarity.h
│   ├── Coefficients.h
│   ├── ImagePurpose.h
│   ├── ErrorAnalysis.h
│   ├── Calibration.h
│   ├── StatsUtil.h
│   └── Evaluation.h
├── src/
│   ├── main.cpp
│   ├── TextProc.cpp
│   ├── Metrics.cpp
│   ├── FlickrLoader.cpp
│   ├── Florence.cpp
│   ├── Downloader.cpp
│   ├── WebParser.cpp
│   ├── SemanticSimilarity.cpp
│   ├── Coefficients.cpp
│   ├── ImagePurpose.cpp
│   ├── ErrorAnalysis.cpp
│   ├── Calibration.cpp
│   ├── StatsUtil.cpp
│   └── Evaluation.cpp
├── python/
│   ├── florence_caption.py
│   └── semantic_similarity.py
├── data/
│   ├── Flickr8k_sample.token.txt
│   ├── Flickr8k_text/
│   │   ├── Flickr8k.token.txt
│   │   ├── Flickr8k.lemma.token.txt
│   │   ├── ExpertAnnotations.txt
│   │   ├── CrowdFlowerAnnotations.txt
│   │   ├── Flickr_8k.trainImages.txt
│   │   ├── Flickr_8k.devImages.txt
│   │   ├── Flickr_8k.testImages.txt
│   │   └── readme.txt
│   └── Flickr8k_images/
│       └── Flicker8k_Dataset/   (8091 attēli)
├── sample_images/
│   ├── 1002674143_1b742ab4b8.jpg
│   ├── 1003163366_44323f5815.jpg
│   └── 1015118661_980735411b.jpg
├── output/
│   └── downloaded_images/
├── build_windows.bat
├── requirements.txt
├── PARSKATS.md
├── LIETOTAJA_ROKASGRAMATA.md
├── PROJEKTA_STRUKTURA.md
└── VS_CODE_ROKASGRAMATA.md
```

Piezīme: šī projekta kopija NESATUR `test_env/` (Python vide) un `app.exe` - abus jāizveido pašam, sekojot `VS_CODE_ROKASGRAMATA.md` instrukcijām. Tas ir apzināti, jo Python vide nav droši pārvietojama uz citu mapi.

---

## 2. `.vscode` mape

### `.vscode/tasks.json`

Fails satur VS Code uzdevumus, lai nebūtu manuāli jāraksta garas komandas terminalī.

Taski:

| Task | Mērķis | Obligāts |
|---|---|---|
| `01 Create Python venv` | Izveido Python virtuālo vidi | Vajadzīgs Florence/semantiskajiem režīmiem |
| `02 Install Python requirements` | Instalē PyTorch, Transformers, sentence-transformers un citas bibliotēkas | Vajadzīgs Florence/semantiskajiem režīmiem |
| `03 Check Python path` | Pārbauda svarīgākos ceļus | Noderīgs diagnostikai |
| `04 Test Florence only` | Palaiž Florence bez C++ | Noderīgs pirms `single`, `web`, `calibrate` |
| `05 Build C++` | Kompilē C++ programmu | Vitāli svarīgs |
| `06 Run experiment mode (full dataset)` | Palaiž Flickr8k validāciju uz visas datu kopas | Vitāli svarīgs validācijai |
| `06b Run experiment mode (quick sample)` | Palaiž Flickr8k validāciju uz 5 attēlu parauga | Noderīgs ātrai pārbaudei |
| `07 Run single mode` | Palaiž viena attēla pārbaudi | Svarīgs praktiskai demonstrācijai |
| `08 Run web mode` | Palaiž tīmekļa lapas pārbaudi | Svarīgs praktiskai demonstrācijai |
| `09 Run calibrate mode` | Palaiž koeficientu kalibrēšanu | Svarīgs metožu pamatošanai |
| `10 Clean output` | Izdzēš iepriekšējos CSV/TXT rezultātus | Nav obligāts |

Ja projektu pārvieto uz citu mapi, šajā failā jāmaina ceļš:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23
```

---

## 3. `include` mape

Šajā mapē ir C++ klašu un struktūru deklarācijas.

### `Types.h`

Satur kopīgās datu struktūras.

```text
ImageItem      - viens tīmekļa lapā atrasts attēls
FlickrItem     - viens Flickr8k attēls ar aprakstiem
MetricResult   - metriku aprēķina rezultāts
WeightProfile  - svaru profils vienai attēla funkcijai
CorpusStats    - korpusa n-gramu statistika CIDEr TF-IDF vajadzībām
```

Vitāli svarīgs fails. To nevajag dzēst.

### `TextProc.h`

Deklarē teksta apstrādes klasi (mazie burti, pieturzīmju noņemšana, stop-vārdi, vienkārša normalizācija). Vitāli svarīgs fails, nemainīts no iepriekšējās versijas.

### `Metrics.h`

Deklarē metriku aprēķinu.

```text
Precision, Recall, F1
BLEU-like
METEOR (reāla saskaņošana + fragmentācijas sods)
ROUGE-L
CIDER (ar un bez korpusa TF-IDF)
attachSemantic - pievieno semantisko metriku un aprēķina gala score pēc svaru profila
```

Vitāli svarīgs fails, programmas pētnieciskais kodols.

### `FlickrLoader.h`

Deklarē Flickr8k teksta faila ielādi. Vajadzīgs `experiment` un `calibrate` režīmam.

### `Florence.h`

Deklarē C++ klasi, kas palaiž Python Florence skriptu (aprakstu ģenerēšanai UN reģionu skaitīšanai sarežģītības noteikšanai). Vajadzīgs `single` un `web` režīmiem.

### `Downloader.h`

Deklarē vienkāršu failu lejupielādes funkcionalitāti (curl). Vajadzīgs `web` režīmam.

### `WebParser.h`

Deklarē vienkāršu HTML parseri, kas meklē `<img src="..." alt="...">` un pārbauda, vai attēls ir ietverts `<a>` saitē. Vajadzīgs `web` režīmam.

### `SemanticSimilarity.h`

**Jauns fails.** Deklarē C++ klasi, kas palaiž Python sentence-transformers skriptu semantiskās līdzības aprēķinam. Uzbūvēta pēc tā paša parauga kā `Florence.h`. Vajadzīgs `single`, `web` un `calibrate` režīmiem.

### `Coefficients.h`

**Jauns fails.** Deklarē klasi, kas glabā 4 nosauktos svaru profilus (decorative/informative/functional/complex).

### `ImagePurpose.h`

**Jauns fails.** Deklarē klasi, kas nosaka attēla funkciju pēc alt teksta, saites konteksta un (pēc izvēles) Florence reģionu skaita.

### `ErrorAnalysis.h`

**Jauns fails.** Deklarē klasi, kas veic detalizētu kļūdu analīzi `experiment` režīma rezultātiem (statistika, level sadalījums, korelācija, sliktākie/labākie pāri).

### `StatsUtil.h`

**Jauns fails.** Deklarē vienkāršas statistikas palīgfunkcijas (vidējais, mediāna, standartnovirze, Pīrsona korelācija), ko kopā izmanto `ErrorAnalysis` un `Calibration`.

### `Calibration.h`

**Jauns fails.** Deklarē klasi, kas salīdzina programmas score ar ExpertAnnotations.txt/CrowdFlowerAnnotations.txt cilvēku vērtējumiem un meklē uzlabotu svaru profilu.

### `Evaluation.h`

Deklarē galveno programmas loģikas klasi. Tā apvieno visas pārējās klases un realizē visus 4 režīmus. Vitāli svarīgs fails.

---

## 4. `src` mape

Šajā mapē ir C++ implementācijas - katram `include/*.h` failam atbilst tāda paša nosaukuma `.cpp` fails ar tādu pašu lomu. Sīkāku aprakstu skat. 3. sadaļā.

Piezīmes par konkrētiem failiem:

```text
main.cpp        - programmas sākuma punkts, komandrindas argumentu apstrāde, 4 režīmi
TextProc.cpp    - nemainīts no iepriekšējās versijas
FlickrLoader.cpp - nemainīts no iepriekšējās versijas
Downloader.cpp  - nemainīts no iepriekšējās versijas
Florence.cpp    - izlabota kļūda ar diagnostikas rindas piesārņojumu (skat. PARSKATS.md)
Metrics.cpp     - visgarākais fails, satur visu metriku matemātiku
Evaluation.cpp  - otrs garākais fails, savieno visu kopā
```

---

## 5. `python` mape

### `python/florence_caption.py`

Python skripts, kas ielādē Florence-2 modeli un ģenerē attēla aprakstu VAI saskaita reģionus (atkarībā no `--task caption` / `--task regions`). C++ programma to palaiž caur `Florence.cpp`. Vajadzīgs `single`, `web` un `calibrate` režīmiem.

### `python/semantic_similarity.py`

**Jauns fails.** Python skripts, kas ielādē sentence-transformers modeli un aprēķina semantisko līdzību starp diviem tekstiem. C++ programma to palaiž caur `SemanticSimilarity.cpp`.

Ja izmanto tikai `experiment` režīmu, šos abus failus var neizmantot.

---

## 6. `data` mape

### `data/Flickr8k_sample.token.txt`

Mazs Flickr8k teksta faila paraugs (5 attēli). Vajadzīgs, lai ātri pārbaudītu `experiment` režīmu.

### `data/Flickr8k_text/Flickr8k.token.txt`

**Pilnais** Flickr8k teksta fails (8091 attēli, 5 apraksti katram). Šis ir jaunais noklusējuma `experiment` režīma avots.

### `data/Flickr8k_text/ExpertAnnotations.txt` un `CrowdFlowerAnnotations.txt`

Reāli cilvēku vērtējumi no oriģinālā Flickr8k pētījuma. Vajadzīgi `calibrate` režīmam (skat. `LIETOTAJA_ROKASGRAMATA.md` 9. sadaļu).

### `data/Flickr8k_images/Flicker8k_Dataset/`

8091 attēla faili. Vajadzīgi tikai `calibrate` režīmam (lai varētu parādīt attēlus, ja tos vēlas apskatīt manuāli) - `experiment` režīms izmanto tikai tekstu, nevis pašus attēlus.

---

## 7. `sample_images` mape

Satur testa attēlus `single` režīmam un Florence pārbaudei. Šie faili nav programmas kodols, bet tie ir noderīgi demonstrācijai.

---

## 8. `output` mape

Šeit programma saglabā rezultātus.

Iespējamie faili:

```text
flickr_validation_results.csv
flickr_error_analysis.txt
single_result.csv
web_accessibility_results.csv
calibration_report.txt
calibration_report_crowdflower.txt
```

`output/downloaded_images` satur web režīmā lejupielādētos attēlus.

---

## 9. Saknes faili

### `build_windows.bat`

Kompilē C++ programmu Windows vidē. Vitāli svarīgs praktiskai palaišanai. Piezīme: šī faila komentāri ir angliski, ne latviski, jo Latviešu diakritiskās zīmes `.bat` failos var izraisīt Windows `cmd.exe` konsoles koda lappuses kļūdas atkarībā no sistēmas iestatījumiem.

### `requirements.txt`

Satur Python bibliotēku sarakstu Florence-2 un sentence-transformers moduļiem, ar precīzi saskaņotām versijām. Vajadzīgs `single`, `web` un `calibrate` režīmiem.

### `PARSKATS.md`

Īss projekta pārskats.

### `LIETOTAJA_ROKASGRAMATA.md`

Detalizēta programmas palaišanas rokasgrāmata.

### `PROJEKTA_STRUKTURA.md`

Šis fails. Apraksta struktūru un failu nozīmi.

### `VS_CODE_ROKASGRAMATA.md`

Speciāla instrukcija programmas uzstādīšanai un testēšanai Visual Studio Code vidē, tai skaitā nepieciešamie VS Code paplašinājumi.

---

## 10. Ko var izņemt, ja nepieciešams

Var izņemt:

```text
sample_images papildu attēlus
output mapes saturu
10 Clean output tasku
VS_CODE_ROKASGRAMATA.md, ja dokumentācija nav vajadzīga projektā
data/Flickr8k_images/Flicker8k_Dataset/, ja neizmanto calibrate režīmu (bet tas
    joprojām ir vajadzīgs, ja grib paši apskatīt attēlus, kurus calibrate analizē)
```

Nevajadzētu izņemt:

```text
src
include
python/florence_caption.py
python/semantic_similarity.py, ja izmanto single/web/calibrate režīmus
requirements.txt
build_windows.bat
data/Flickr8k_text/Flickr8k.token.txt
data/Flickr8k_text/ExpertAnnotations.txt un CrowdFlowerAnnotations.txt, ja izmanto calibrate režīmu
.vscode/tasks.json, ja grib ērtu VS Code testēšanu
```

Ja izņem `python/florence_caption.py`, tad paliek tikai `experiment` režīms.

Ja izņem `FlickrLoader`, tad pazūd validācijas un kalibrēšanas režīmi.
