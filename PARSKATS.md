# Attēlu aprakstu precizitātes un pilnības analīze pieejamības testēšanai programma

Šis projekts ir prototips aAttēlu aprakstu precizitātes un pilnības analīze pieejamības testēšanai.

Projekta paredzamais ceļš šajā versijā:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23
```

---

## Ko programma dara

Programma salīdzina pārbaudāmo attēla alternatīvo tekstu ar references aprakstu un aprēķina kvalitātes rādītājus.

Galvenā ideja:

```text
reference apraksts
+
pārbaudāmais alt teksts
-> C++ metrikas (lexikas + semantiskā līdzība)
-> attēla funkcijas noteikšana (decorative / informative / functional / complex)
-> ar funkciju saskaņots svaru profils
-> pieejamības novērtējums
```

Florence-2 tiek izmantots tikai praktiskajos režīmos kā ārējs Python modulis references apraksta ģenerēšanai. Sentence-transformers tiek izmantots kā otrs ārējs Python modulis semantiskās līdzības aprēķinam.

---

## Kas ir jauns šajā versijā

Salīdzinot ar iepriekšējām versijām:

```text
experiment režīms tagad noklusējumā izmanto VISU Flickr8k datu kopu (8091 attēli), nevis
    5 attēlu paraugu, un pēc katras palaišanas veic detalizētu kļūdu analīzi
izlabota kļūda, kurā Florence-2 diagnostikas rinda ("Florence device: cpu") nokļuva
    izmērītajā aprakstā un piesārņoja metrikas
pievienota semantiskā līdzības metrika (sentence-transformers) papildus esošajām
    lexikajām metrikām
METEOR tagad izmanto reālu vārdu saskaņošanu un fragmentācijas sodu, ne tikai formulu
CIDER tagad izmanto korpusa TF-IDF svarus (kad pieejams pilns Flickr8k korpuss)
pievienota attēla funkcijas noteikšana (decorative / informative / functional / complex)
    pēc W3C WAI alt-teksta lēmumu koka principa
katrai funkcijai ir savs svaru profils, un "informative" profils ir kalibrēts pret
    ExpertAnnotations.txt cilvēku vērtējumiem (jauns "calibrate" režīms)
score kategorijas tagad ir angliski (insufficient/weak/average/good/excellent), nevis latviski
```

---

## Galvenie režīmi

| Režīms | Komanda | Nozīme |
|---|---|---|
| `experiment` | Flickr8k validācija | Pārbauda metrikas uz cilvēku veidotiem aprakstiem, visai datu kopai, ar kļūdu analīzi |
| `single` | Viena attēla pārbaude | Florence-2 ģenerē reference aprakstu vienam attēlam |
| `web` | Tīmekļa lapas pārbaude | Programma analizē `<img>` un `alt` tekstus no URL |
| `calibrate` | Koeficientu kalibrēšana | Salīdzina mūsu score ar ExpertAnnotations.txt / CrowdFlowerAnnotations.txt cilvēku vērtējumiem |

---

## Ātrā palaišana VS Code vidē

1. Atver projektu VS Code:

```text
M:\VS_Code_Projects\AIDACAT_LV_v23
```

2. Izmanto VS Code izvēlni:

```text
Terminal -> Run Task
```

3. Pirmajā reizē palaid (secībā):

```text
01 Create Python venv
02 Install Python requirements
03 Check Python path
04 Test Florence only
05 Build C++
```

4. Testē režīmus:

```text
06 Run experiment mode (full dataset)
07 Run single mode
08 Run web mode
09 Run calibrate mode (quick, ~3 min)
```

---

## Manuāla kompilācija

```powershell
.\build_windows.bat
```

---

## Manuāla režīmu palaišana

Flickr8k validācija (pilna datu kopa, ar kļūdu analīzi):

```powershell
.\app.exe experiment data\Flickr8k_text\Flickr8k.token.txt 0
```

Viena attēla pārbaude:

```powershell
.\app.exe single "sample_images\1002674143_1b742ab4b8.jpg" "A child is playing outside" --python "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe"
```

Web pārbaude:

```powershell
.\app.exe web "https://www.w3.org/WAI/tutorials/images/informative/" 5 --python "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe" --classify-complexity
```

Koeficientu kalibrēšana:

```powershell
.\app.exe calibrate data\Flickr8k_text\ExpertAnnotations.txt data\Flickr8k_text\Flickr8k.token.txt --limit 30 --python "M:\VS_Code_Projects\AIDACAT_LV_v23\test_env\Scripts\python.exe"
```

---

## Dokumentācija

Papildu informācija:

```text
PROJEKTA_STRUKTURA.md
LIETOTAJA_ROKASGRAMATA.md
VS_CODE_ROKASGRAMATA.md
```

---

## Ierobežojumi

Programma ir prototips.

Tā nav pilns rūpniecisks pieejamības audita rīks.

Galvenie ierobežojumi:

```text
HTML parseris ir vienkāršots, nav īsta DOM koka
JavaScript netiek izpildīts
CSS background-image netiek analizēts
Florence-2 reference nav cilvēka apstiprināta absolūta patiesība
attēla funkcijas noteikšana ir vienkāršota heiristika, nevis pilna WAI lēmumu koka realizācija
tikai "informative" svaru profils ir empīriski kalibrēts - pārējie 3 ir pamatoti pieņēmumi
calibrate režīms ir lēns pie liela --limit, jo katrai rindai jāpalaiž atsevišķs Python process
    semantiskās līdzības aprēķinam (skat. LIETOTAJA_ROKASGRAMATA.md)
```
