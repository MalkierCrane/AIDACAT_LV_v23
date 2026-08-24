# Lietotaja rokasgramata

Šī rokasgramata apraksta, kā palaist programmu un kā izmantot tās režīmus. 
Programma ir paredzēta attēlu aprakstu precizitātes un pilnības analīzei pieejamības testēšanai.

Projekta paredzamais ceļš:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23
```

---

## 1. Programmas mērķis

Programmas galvenais uzdevums ir salīdzināt pārbaudāmo attēla alternatīvo tekstu ar references aprakstu un aprēķināt apraksta kvalitātes rādītājus.

Programma pati netrenē mākslīgā intelekta modeli. Florence-2 tiek izmantots tikai kā ārējs Python modulis references apraksta ģenerēšanai praktiskajos režīmos. Sentence-transformers tiek izmantots kā otrs ārējs Python modulis semantiskās līdzības aprēķinam.

---

## 2. Programmas režīmi

Programmā ir četri galvenie režīmi.

| Režīms | Komanda | Mērķis |
|---|---|---|
| Flickr8k validācija | `experiment` | Pārbauda metrikas uz cilvēku veidotiem Flickr8k aprakstiem, ar kļūdu analīzi |
| Viena attēla pārbaude | `single` | Pārbauda vienu attēlu un vienu lietotāja alt tekstu |
| Tīmekļa lapas pārbaude | `web` | Izvelk `<img>` un `alt` tekstus no tīmekļa lapas |
| Koeficientu kalibrēšana | `calibrate` | Salīdzina programmas score ar cilvēku vērtējumiem |

---

## 3. Nepieciešamās daļas

Lai palaistu tikai `experiment` režīmu, vajadzīgs:

```text
C++ kompilators
data/Flickr8k_text/Flickr8k.token.txt (vai mazais data/Flickr8k_sample.token.txt paraugam)
app.exe
```

Lai palaistu `single`, `web` un `calibrate` režīmu, papildus vajadzīgs:

```text
Python virtuālā vide
PyTorch
Transformers (Florence-2 references aprakstiem)
sentence-transformers (semantiskajai līdzībai)
```

---

## 4. Python vides sagatavošana

No projekta mapes:

```powershell
cd "M:\VS_Code_Projects\AIDACAT_LV_v23"
python -m venv test_env
```

Bibliotēku instalācija bez vides aktivizēšanas:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" -m pip install --upgrade pip
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" -m pip install -r requirements.txt
```

**Svarīgi par versijām:** `requirements.txt` pin-o gan `transformers==4.49.0`, gan `sentence-transformers==4.1.0` konkrētās versijās apzināti. Ja instalē `sentence-transformers` atsevišķi (bez `requirements.txt`) vai vēlāk to atjaunini, `pip` var pats uzstādīt jaunāku `transformers` versiju (5.x), kas **salauž Florence-2** (Florence-2 izmanto `trust_remote_code=True` pielāgotu kodu, kas nav savietojams ar `transformers` 5.x). Ja tas notiek, atkārtoti instalē tieši pinned versijas:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" -m pip install "transformers==4.49.0" "sentence-transformers==4.1.0"
```

Florence pārbaude:

```powershell
& "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" "M:\VS_Code_Projects\AIDACAT_LV_v23\python\florence_caption.py" "M:\VS_Code_Projects\AIDACAT_LV_v23\sample_images\1002674143_1b742ab4b8.jpg"
```

---

## 5. C++ programmas kompilācija

No projekta mapes:

```powershell
.\build_windows.bat
```

Pēc veiksmīgas kompilācijas izveidojas:

```text
app.exe
```

---

## 6. Flickr8k validācijas režīms

Komanda (pilna datu kopa, 8091 attēli):

```powershell
.\app.exe experiment data\Flickr8k_text\Flickr8k.token.txt 0
```

Ātrai pārbaudei ar mazu paraugu (5 attēli):

```powershell
.\app.exe experiment data\Flickr8k_sample.token.txt 5
```

Trešais parametrs ir limits - cik attēlus ielādēt (`0` = visi). Papildu karogs `--top-n N` (noklusējums 10) nosaka, cik sliktākos/labākos pārus rādīt kļūdu analīzē.

Ko režīms dara:

```text
ielādē Flickr8k aprakstus
sagrupē tos pēc attēla nosaukuma
vienu aprakstu izmanto kā reference
citus aprakstus izmanto kā testējamos tekstus
aprēķina metrikas (BEZ semantiskās līdzības - skat. piezīmi zemāk)
saglabā CSV rezultātus
veic detalizētu kļūdu analīzi: statistiku, level sadalījumu, korelāciju, sliktākos/labākos pārus
```

**Kāpēc bez semantiskās līdzības:** pilnā datu kopā ir apmēram 32000 salīdzināmu pāru. Semantiskās līdzības aprēķins palaiž atsevišķu Python procesu katram pārim - ja to darītu 32000 reizes, viss process ilgtu vairākas stundas. Tāpēc `experiment` režīms (tāpat kā jau iepriekšējā versijā) nekad neizsauc Python - tas paliek tīri C++ teksta metriku salīdzinājums, kas ļauj to palaist uz visas datu kopas dažu sekunžu laikā (apmēram 15-20 sekundes uz vidēja datora).

Rezultāti:

```text
output\flickr_validation_results.csv    - katra pāra metrikas
output\flickr_error_analysis.txt        - kļūdu analīzes atskaite
```

---

## 7. Viena attēla režīms

Komanda:

```powershell
.\app.exe single "sample_images\1002674143_1b742ab4b8.jpg" "A child is playing outside" --python "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe"
```

Papildu karogs `--classify-complexity` ieslēdz Florence reģionu skaitīšanu (sarežģītības noteikšanai) - tas prasa vēl vienu Florence izsaukumu, tāpēc noklusējumā ir izslēgts.

Ko režīms dara:

```text
paņem lokālu attēlu
palaiž Florence-2 Python skriptu, iegūst reference aprakstu
palaiž sentence-transformers Python skriptu, aprēķina semantisko līdzību
nosaka attēla funkciju (decorative/informative/functional/complex) pēc alt teksta
izvēlas šai funkcijai atbilstošu svaru profilu
aprēķina pieejamības rādītājus
saglabā CSV rezultātu
```

Rezultāts:

```text
output\single_result.csv
```

---

## 8. Web režīms

Komanda:

```powershell
.\app.exe web "https://www.w3.org/WAI/tutorials/images/informative/" 5 --python "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" --classify-complexity
```

Ko režīms dara:

```text
lejupielādē HTML
atrod <img> tagus, tai skaitā pārbauda, vai tie ir ietverti <a> saitē
izvelk src un alt atribūtus
lejupielādē attēlus
Florence-2 ģenerē references aprakstus
sentence-transformers aprēķina semantisko līdzību
nosaka katra attēla funkciju un izvēlas atbilstošu svaru profilu
C++ aprēķina metrikas
saglabā lapas pieejamības rezultātus
```

Rezultāts:

```text
output\web_accessibility_results.csv
```

Lejupielādētie attēli:

```text
output\downloaded_images
```

---

## 9. Koeficientu kalibrēšanas režīms

Šis režīms salīdzina programmas aprēķināto score ar reāliem cilvēku vērtējumiem no Flickr8k datu kopas oriģinālā pētījuma (Hodosh, Young un Hockenmaier, 2013).
Pētījums atrodams šeit: https://www.jair.org/index.php/jair/article/view/10833

Komanda:

```powershell
.\app.exe calibrate data\Flickr8k_text\ExpertAnnotations.txt data\Flickr8k_text\Flickr8k.token.txt --limit 30 --python "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe"
```

Papildu karogi:

```text
--limit N              cik annotāciju rindas apstrādāt (noklusējums 100, jo bez limita
                        pilna datu kopa (5822 rindas) aizņemtu vairākas stundas - skat. zemāk)
--crowdflower           papildus kalibrē arī pret CrowdFlowerAnnotations.txt
--crowdflower-file <celjs>  cits CrowdFlower faila ceļš
```

**Cik ilgi tas aizņem:** katra annotāciju rinda prasa vienu semantiskās līdzības Python izsaukumu (modeļa ielāde katru reizi no jauna aizņem apmēram 5 sekundes). Pārbaudē 100 rindas aizņēma apmēram 9 minūtes. Tāpēc:

```text
--limit 30    apmēram 2-3 minūtes  - ātrai pārbaudei
--limit 100   apmēram 8-9 minūtes  - noklusējums, saprātīgs kompromiss
--limit 500   apmēram 45 minūtes   - nopietnākai analīzei
--limit 0     (visas 5822 rindas)  - vairākas stundas, palaist tikai ar laiku pāri
```

Ko režīms dara:

```text
ielādē visus Flickr8k attēlus un aprakstus (bez limita)
ielādē ExpertAnnotations.txt (un, ja pieprasīts, CrowdFlowerAnnotations.txt)
katrai annotāciju rindai: atrod vērtējamā attēla references aprakstu un vērtēto
    kandidāta aprakstu, aprēķina mūsu metrikas, salīdzina ar cilvēka vērtējumu
aprēķina korelāciju starp mūsu final_score un cilvēka vērtējumu (pirms kalibrēšanas)
meklē uzlabotu svaru profilu ar "coordinate ascent" (soli pa solim katru svaru pielāgojot)
aprēķina korelāciju ar jauno profilu (pēc kalibrēšanas)
ieraksta abus rezultātus un ieteikto profilu atskaitē
```

**Svarīgi par ExpertAnnotations.txt struktūru:** katra rinda vērtē, cik labi VIENA attēla apraksts (kolonna 2) apraksta CITU attēlu (kolonna 1) - vairumā gadījumu (apmēram 97%) tie ir DAŽĀDI attēli (šī ir oriģinālā pētījuma "ranking task" struktūra - tā pārbauda, vai sistēma pareizi atšķir pareizu aprakstu no nepareiza). Programma par references tekstu izmanto vērtējamā attēla pirmo cilvēka aprakstu, un salīdzina to ar norādīto kandidāta aprakstu - tieši tāpat, kā to dara arī `experiment` režīms.

Rezultāts:

```text
output\calibration_report.txt                  - ExpertAnnotations.txt rezultāti
output\calibration_report_crowdflower.txt       - CrowdFlowerAnnotations.txt rezultāti (ja --crowdflower)
```

**Rezultātu izmantošana:** atskaite piedāvā jaunu svaru profilu kā gatavu skaitļu sarakstu. Programma šos skaitļus AUTOMĀTISKI nepiemēro - ja gribi tos izmantot, jāieliek tie manuāli `src/Coefficients.cpp` faila `informative` profilā un jāpārkompilē programma. Tas apzināti nav automātisks process, lai persona pati izlemtu, vai uzlabotā korelācija ir pietiekams pamatojums izmaiņai.

### Kur meklēt vairāk salīdzināšanas datu

1. **`ExpertAnnotations.txt` / `CrowdFlowerAnnotations.txt`** - jau ir šajā projektā (`data/Flickr8k_text/`), nekas nav jālejupielādē. Tie ir reāli cilvēku vērtējumi no Hodosh, Young un Hockenmaier (2013), *"Framing Image Description as a Ranking Task"*, JAIR, 47. sējums.
2. **W3C WAI attēlu pamācība** - `https://www.w3.org/WAI/tutorials/images/` - tā pati vietne, ko šī programma jau izmanto kā `web` režīma demonstrācijas URL. Tai ir atsevišķas apakšlapas tieši pēc decorative/informative/functional/complex (un vēl text/group) kategorijām - tas ir bezmaksas, standartu organizācijas veidots atsauces materiāls, pret kuru var salīdzināt `ImagePurpose` klasificētāja rezultātus.
3. **Papildu lasīšanai (nav integrēts kodā):** MS COCO Captions (lielāka, cilvēku vērtēta aprakstu datu kopa), Flickr30k (Flickr8k lielākais "brālis"), PASCAL-50S/ABSTRACT-50S (datu kopas, ko oriģinālā CIDEr raksta autori izmantoja tieši metrikas-pret-cilvēku korelācijas mērīšanai - tas pats, ko dara šī projekta `calibrate` režīms).

---

## 10. Metriku nozīme

CSV failos un konsoles izvadē redzami šādi rādītāji:

| Rādītājs | Nozīme |
|---|---|
| Precision | Cik liela pārbaudāmā teksta daļa sakrīt ar reference tekstu |
| Recall | Cik liela reference teksta daļa ir iekļauta pārbaudāmajā tekstā |
| F1 | Precision un Recall līdzsvarots rādītājs |
| BLEU | N-gramu (1-4 vārdu virknes) līdzības rādītājs ar īsuma sodu |
| METEOR | Vārdu saskaņošanas rādītājs (precīza + sinonīmu saskaņošana) ar fragmentācijas sodu |
| ROUGE-L | Garākās kopīgās virknes līdzība |
| CIDEr | N-gramu satura līdzība ar korpusa TF-IDF svariem (retāki n-grami dabū lielāku svaru) |
| Semantic | Semantiskā līdzība starp abu tekstu nozīmi (sentence-transformers), 0 līdz 1 - **vienmēr 0 `experiment` režīmā** (skat. 6. sadaļu) |
| Final | Kopējais pieejamības novērtējums no 0 līdz 100, aprēķināts pēc attēla funkcijai atbilstoša svaru profila |
| Level | Tekstiska novērtējuma līmenis: `insufficient` / `weak` / `average` / `good` / `excellent` |
| Purpose | Attēla funkcija: `decorative` / `informative` / `functional` / `complex` / `n/a` (ja funkcija netiek noteikta, piemēram `experiment` režīmā) |

### Svaru profili

Katrai `purpose` kategorijai ir savs svaru profils (skat. `src/Coefficients.cpp`):

| Profils | Loģika |
|---|---|
| `informative` | Līdzsvarots profils, uzsver precizitāti un semantisko līdzību. **Vienīgais empīriski kalibrētais.** |
| `decorative` | Uzsver semantisko līdzību un precizitāti - dekoratīvam attēlam pareizi ir vai nu tukšs alt teksts, vai īss un precīzs, nevis garš apraksts. |
| `functional` | Vislielākais semantiskās līdzības svars - svarīgi, lai alt teksts pareizi apraksta DARBĪBU (pogas, saites mērķi), nevis vizuālo izskatu. |
| `complex` | Lielāks recall svars - svarīgi, lai apraksts pārklātu VISU saturu (piemēram, diagrammas datus), nevis tikai daļu. |

---

## 11. VS Code Tasks

Projektā ir fails:

```text
.vscode\tasks.json
```

Tas ļauj palaist komandas no VS Code:

```text
Terminal -> Run Task
```

Svarīgākie taski:

```text
05 Build C++
06 Run experiment mode (full dataset)
06b Run experiment mode (quick sample)
07 Run single mode
08 Run web mode
09 Run calibrate mode (quick, ~3 min)
```

Pirmreizējai uzstādīšanai:

```text
01 Create Python venv
02 Install Python requirements
03 Check Python path
04 Test Florence only
```

---

## 12. Ierobežojumi

Programma ir prototips, nevis pilns rūpniecisks pieejamības audita rīks.

Galvenie ierobežojumi:

```text
web parseris ir vienkāršs, nav īsta DOM koka
JavaScript netiek izpildīts
CSS background attēli netiek analizēti
Florence-2 reference nav cilvēka apstiprināta absolūta patiesība
attēla funkcijas noteikšana ir vienkāršota heiristika (garuma un atslēgvārdu pārbaude),
    nevis pilna W3C WAI lēmumu koka realizācija
tikai "informative" svaru profils ir empīriski kalibrēts
calibrate režīms ir lēns pie liela --limit (skat. 9. sadaļu)
```
