# Visual Studio Code lietotaja rokasgramata

Šis fails apraksta, ka uzstadīt, kompilēt un palaist projektu Visual Studio Code vide uz Windows datora.

Projekta paredzamais ceļš šajā rokasgramatā:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23
```

Ja projekts atrodas cita mape, tad komandas un `.vscode/tasks.json` failā jāmaina ceļš.

---

## 0. Kas jāuzstāda uz datora vispirms

Šī sadaļa ir domāta pilnīgi no jauna sākošam cilvēkam. Detaļas par instalēšanu meklē pašā programmā vai internetā - šeit ir tikai saraksts, KO tieši meklēt.

1. **Visual Studio Code** - lejupielādē un uzstādi no oficiālās VS Code mājaslapas.
2. **Divi VS Code paplašinājumi (extensions)** - VS Code atver `Extensions` (ikona kreisajā sānjoslā, izskatās pēc četriem kvadrātiem), meklē un uzstādi:

   | Paplašinājums | Izstrādātājs | Kāpēc vajadzīgs |
   |---|---|---|
   | `C/C++` | Microsoft | C++ koda izcelšana un IntelliSense (nav obligāts kompilācijai, bet ļoti noder) |
   | `Python` | Microsoft | Python koda izcelšana un palīdzība |

3. **C++ kompilators (g++)** - piemēram, MSYS2 (satur `g++` un citus rīkus) vai MinGW-w64. Pēc uzstādīšanas pārliecinies, ka `g++` ir pievienots Windows `PATH` mainīgajam (pārbaude aprakstīta 9.4. sadaļā).
4. **Python** (3.10 vai jaunāka versija) - jāuzstāda tā, lai komanda `python` darbotos terminālī.

Kad šie četri punkti ir gatavi, vari turpināt ar 1. sadaļu.

---

## 1. Projekta atveršana VS Code

1. Atver Visual Studio Code.
2. Izvēlies `File -> Open Folder`.
3. Atver šo mapi:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23
```

Pareizi atvērtā projekta saknē jābūt redzamiem šiem failiem un mapēm:

```text
include
src
python
data
sample_images
output
.vscode
build_windows.bat
requirements.txt
PARSKATS.md
```

Svarīgi: VS Code ir jāatver tieši projekta saknes mape, nevis `src`, `python` vai cita apakšmape.

---

## 2. Terminala atvēršana

VS Code izvēlies:

```text
Terminal -> New Terminal
```

Pārbaudi, kur atrodas terminalis:

```powershell
pwd
```

Pareizam rezultātam jābūt:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23
```

Ja terminalis ir citā mapē, pārej uz projekta mapi:

```powershell
cd "M:\VS_Code_Projects\AIDACAT_LV_v23"
```

---

## 3. Python vides izveide

Virtuālā vide vajadzīga Florence-2 un semantiskās līdzības Python moduļiem. Tā nav vajadzīga tikai `experiment` režīmam, bet ir vajadzīga `single`, `web` un `calibrate` režīmam.

Izveido vidi:

```powershell
python -m venv test_env
```

Pēc tam var aktivizēt vidi:

```powershell
.\test_env\Scripts\activate
```

Ja PowerShell rāda kļūdu par skriptu bloķēšanu, skaties sadaļu `Iespējamās problēmas`.

Svarīgi: programmu var palaist arī bez vides aktivizēšanas, ja komandās tiek norādīts pilns Python ceļš:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe
```

Tieši šo pieeju izmanto `tasks.json` fails.

---

## 4. Python bibliotēku instalēšana

Ja vide ir aktivizēta:

```powershell
python -m pip install --upgrade pip
pip install -r requirements.txt
```

Ja vidi neaktivizē, lieto pilnu Python ceļu:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" -m pip install --upgrade pip
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" -m pip install -r requirements.txt
```

**Svarīgi:** vienmēr instalē no `requirements.txt`, nevis pa vienai bibliotēkai. Fails satur precīzi saskaņotas versijas (`transformers==4.49.0` un `sentence-transformers==4.1.0`) - ja instalē `sentence-transformers` atsevišķi, `pip` var uzstādīt jaunāku `transformers`, kas salauž Florence-2 (skat. `LIETOTAJA_ROKASGRAMATA.md` 4. sadaļu).

Pārbaude:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" -c "import torch; print(torch.__version__); print(torch.cuda.is_available())"
```

Ja `torch.cuda.is_available()` rāda `False`, tas nav defekts. Tas tikai nozīmē, ka Florence-2 strādās CPU režīmā (lēnāk, bet strādā).

---

## 5. Florence-2 un semantiskās līdzības pārbaude bez C++ programmas

Pirms lieto `single`, `web` vai `calibrate` režīmu, pārbaudi abus Python skriptus atsevišķi.

Florence-2:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" "M:\VS_Code_Projects\AIDACAT_LV_v23\python\florence_caption.py" "M:\VS_Code_Projects\AIDACAT_LV_v23\sample_images\1002674143_1b742ab4b8.jpg"
```

Pirmajā reizē modelis var lejupielādēties (vajadzīgs interneta pieslēgums). Tas ir normāli. Ja viss darbojas, terminālī parādīsies anglisks attēla apraksts.

Semantiskā līdzība:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" "M:\VS_Code_Projects\AIDACAT_LV_v23\python\semantic_similarity.py" "a dog running in a park" "a puppy playing outside"
```

Arī šeit pirmajā reizē modelis var lejupielādēties. Ja viss darbojas, terminālī parādīsies skaitlis no 0 līdz 1.

Ja kāds no šiem testiem nestrādā, tad `single`, `web` vai `calibrate` režīms arī nestrādās.

---

## 6. C++ programmas kompilēšana

Vienkāršākais variants:

```powershell
.\build_windows.bat
```

Ja kompilācija izdodas, projekta mapē parādās:

```text
app.exe
```

Manuālā komanda:

```powershell
g++ -std=c++17 src\main.cpp src\TextProc.cpp src\Metrics.cpp src\FlickrLoader.cpp src\Florence.cpp src\Downloader.cpp src\WebParser.cpp src\Evaluation.cpp src\SemanticSimilarity.cpp src\Coefficients.cpp src\ImagePurpose.cpp src\ErrorAnalysis.cpp src\Calibration.cpp src\StatsUtil.cpp -I include -o app.exe
```

---

## 7. Režīmu palaišana no terminala

### 7.1. Flickr8k validācijas režīms

Šis režīms pārbauda C++ metrikas bez Florence-2 un bez sentence-transformers - tāpēc tas ir ātrs (apmēram 15-20 sekundes pilnai 8091 attēlu datu kopai).

```powershell
.\app.exe experiment data\Flickr8k_text\Flickr8k.token.txt 0
```

Rezultāts:

```text
output\flickr_validation_results.csv
output\flickr_error_analysis.txt
```

Šis ir pirmais režīms, kuru vajag pārbaudīt. Ja tas strādā, C++ metrikas un Flickr ielāde darbojas.

### 7.2. Viena attēla režīms

Šis režīms izmanto Florence-2 references apraksta ģenerēšanai un sentence-transformers semantiskajai līdzībai.

```powershell
.\app.exe single "sample_images\1002674143_1b742ab4b8.jpg" "A child is playing outside" --python "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe"
```

Rezultāts:

```text
output\single_result.csv
```

Datu plūsma:

```text
attēls
-> Florence-2 reference apraksts
-> pārbaudāmais alt teksts
-> sentence-transformers semantiskā līdzība
-> attēla funkcijas noteikšana
-> C++ metrikas ar pareizo svaru profilu
-> pieejamības vērtējums
```

### 7.3. Web režīms

Šis režīms pārbauda vienkāršu tīmekļa lapu.

```powershell
.\app.exe web "https://www.w3.org/WAI/tutorials/images/informative/" 5 --python "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" --classify-complexity
```

Rezultāts:

```text
output\web_accessibility_results.csv
```

Lejupielādētie attēli:

```text
output\downloaded_images
```

Svarīgi: web parseris ir vienkāršs. Tas meklē parastus `<img>` elementus ar `src` un `alt`. Tas neizpilda JavaScript.

### 7.4. Koeficientu kalibrēšanas režīms

Šis režīms salīdzina programmas score ar cilvēku vērtējumiem. Detalizēts apraksts ir `LIETOTAJA_ROKASGRAMATA.md` 9. sadaļā - **svarīgi izlasīt par laika patēriņu pirms palaišanas**.

```powershell
.\app.exe calibrate data\Flickr8k_text\ExpertAnnotations.txt data\Flickr8k_text\Flickr8k.token.txt --limit 30 --python "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe"
```

Rezultāts:

```text
output\calibration_report.txt
```

---

## 8. Tasks izmantošana VS Code

Projektā ir pievienots fails:

```text
.vscode\tasks.json
```

Tas ļauj palaist biežākās komandas no VS Code izvēlnes.

Atver:

```text
Terminal -> Run Task
```

Pieejamie uzdevumi:

```text
01 Create Python venv
02 Install Python requirements
03 Check Python path
04 Test Florence only
05 Build C++
06 Run experiment mode (full dataset)
06b Run experiment mode (quick sample)
07 Run single mode
08 Run web mode
09 Run calibrate mode (quick, ~3 min)
10 Clean output
```

Ieteicamā secība pirmajai palaišanai:

```text
01 Create Python venv
02 Install Python requirements
03 Check Python path
04 Test Florence only
05 Build C++
06b Run experiment mode (quick sample)
07 Run single mode
08 Run web mode
09 Run calibrate mode (quick, ~3 min)
```

Pēc pirmās uzstādīšanas ikdienā parasti pietiek ar:

```text
05 Build C++
06 Run experiment mode (full dataset)
07 Run single mode
08 Run web mode
```

`tasks.json` izmanto konkrētu projekta ceļu:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23
```

Ja projekts tiek pārvietots, šis ceļš jāmaina `.vscode/tasks.json` failā (visur, kur tas parādās).

---

## 9. Iespējamās problēmas un risinājumi

### 9.1. PowerShell bloķē vides aktivizāciju

Kļūda:

```text
Activate.ps1 cannot be loaded because running scripts is disabled on this system
```

Iemesls: PowerShell drošības politika bloķē `.ps1` skriptus.

Risinājums tikai pašreizējam terminalim:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\test_env\Scripts\activate
```

Alternatīva bez aktivizēšanas: lieto tiešu Python ceļu, kā tas ir izdarīts `tasks.json` failā.

---

### 9.2. `The system cannot find the path specified`

Iemesls: kāds no ceļiem nav pareizs vai fails neeksistē.

Pārbaudi:

```powershell
Test-Path "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe"
Test-Path "M:\VS_Code_Projects\AIDACAT_LV_v23\python\florence_caption.py"
Test-Path "M:\VS_Code_Projects\AIDACAT_LV_v23\python\semantic_similarity.py"
Test-Path "M:\VS_Code_Projects\AIDACAT_LV_v23\sample_images\1002674143_1b742ab4b8.jpg"
```

Visiem rezultātiem jābūt:

```text
True
```

Ja kāds ir `False`, jālabo attiecīgais ceļš vai fails jāievieto pareizajā mapē.

---

### 9.3. `ERROR: Florence did not return caption`

Iemesls: Python skripts netika palaists vai nokrita ar kļūdu.

Pirmais tests:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" "M:\VS_Code_Projects\AIDACAT_LV_v23\python\florence_caption.py" "M:\VS_Code_Projects\AIDACAT_LV_v23\sample_images\1002674143_1b742ab4b8.jpg"
```

Ja šeit parādās `ModuleNotFoundError`, jāinstalē bibliotēkas:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" -m pip install -r requirements.txt
```

Ja parādās kļūda par `Florence2LanguageConfig` vai līdzīgu, visdrīzāk `transformers` versija ir nepareiza - skaties 9.5. sadaļu.

Ja notiek modeļa lejupielāde, jāpagaida līdz tā pabeidzas.

---

### 9.4. `g++ is not recognized`

Iemesls: nav uzstādīts C++ kompilators vai tas nav pievienots Windows `PATH`.

Pārbaude:

```powershell
g++ --version
```

Ja komanda netiek atpazīta, jāuzstāda MinGW/MSYS2 vai cits `g++` kompilators un jāpievieno tā `bin` mape sistēmas `PATH` mainīgajam.

---

### 9.5. `ModuleNotFoundError` vai `AttributeError: 'Florence2LanguageConfig' object has no attribute...`

Pirmais iemesls: Python bibliotēkas nav instalētas tajā vidē, kuru programma izmanto.

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" -m pip install -r requirements.txt
```

Otrais iemesls (biežāk sastopams): `transformers` versija ir nesaskanoša ar Florence-2. Tas var notikt, ja `sentence-transformers` tika instalēts atsevišķi un pievilka jaunāku `transformers` (5.x) - Florence-2 strādā tikai ar `transformers==4.49.0`. Risinājums - instalē abas bibliotēkas atpakaļ pareizajās versijās:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" -m pip install "transformers==4.49.0" "sentence-transformers==4.1.0"
```

---

### 9.6. Florence-2 vai sentence-transformers lejupielāde apstājas

Ja Hugging Face modeļa lejupielāde iestrēgst, var izdzēst nepabeigtos failus:

```powershell
Remove-Item "$env:USERPROFILE\.cache\huggingface\hub" -Recurse -Filter "*.incomplete"
```

Pēc tam palaid attiecīgo Python testu vēlreiz.

---

### 9.7. Web režīms neko neatrod

Iespējamie iemesli:

```text
lapā nav parastu <img> tagu
attēli tiek ielādēti ar JavaScript
serveris bloķē lejupielādi
attēlu src ir neparastā formātā
```

Šis ir pieņemams ierobežojums. Programmas mērķis nav pilns web crawler, bet skaidrojošo tekstu novērtēšanas prototips.

---

### 9.8. `calibrate` režīms strādā ļoti ilgi

Tas ir normāli, ne kļūda - katrai annotāciju rindai jāpalaiž atsevišķs Python process semantiskajai līdzībai. Lieto mazāku `--limit` vērtību (skat. `LIETOTAJA_ROKASGRAMATA.md` 9. sadaļu par aptuveno laiku).

---

## 10. Ieteicamā demonstrācijas secība

Demonstrācijai ieteicama šāda secība:

```text
1. Parādīt projekta struktūru.
2. Palaist 05 Build C++.
3. Palaist 06b Run experiment mode (quick sample) - ātrai pārbaudei.
4. Palaist 06 Run experiment mode (full dataset) - parādīt pilnu 8091 attēlu analīzi.
5. Atvērt output/flickr_error_analysis.txt - parādīt statistiku, korelāciju, sliktākos/labākos pārus.
6. Palaist 04 Test Florence only.
7. Palaist 07 Run single mode.
8. Atvērt output/single_result.csv.
9. Palaist 08 Run web mode.
10. Atvērt output/web_accessibility_results.csv - parādīt purpose un semantic kolonnas.
11. Palaist 09 Run calibrate mode (quick, ~3 min).
12. Atvērt output/calibration_report.txt - parādīt korelāciju pirms/pēc kalibrēšanas.
```

Tas skaidri parāda, ka programmai ir:

```text
validācijas režīms uz pilnas datu kopas ar kļūdu analīzi
praktiska viena attēla pārbaude ar semantisko līdzību un funkcijas noteikšanu
praktiska tīmekļa lapas pārbaude
koeficientu kalibrēšana pret reāliem cilvēku vērtējumiem
```
