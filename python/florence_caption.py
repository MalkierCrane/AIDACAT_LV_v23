# Šeit tiek iekļauts argumentu parseris, kļūdu izvade, attēlu atvēršana, PyTorch un Hugging Face Florence-2 ielādei
import argparse
import sys
from PIL import Image
import torch
from transformers import AutoProcessor, AutoModelForCausalLM

# Šī funkcija automātiski izvēlas ierīci
def choose_device():

    if torch.cuda.is_available():   # Ja PyTorch vidē ir pieejama CUDA, lietoju GPU
        return "cuda"   # Atgriežu CUDA ierīci
    return "cpu"    # Ja CUDA nav, lietoju CPU.

# Šī funkcija ielādē Florence-2 modeli
def load_model(model_name):

    device = choose_device()    # Izvēlos ierīci

    # Ja ir CUDA, lietoju float16, lai samazinātu GPU atmiņu
    if device == "cuda":    
        dtype = torch.float16       # Sagatavoju float16 tipu

    # Ja ir CPU, lietoju float32
    else:
        dtype = torch.float32   # Sagatavojam float32 tipu

    # Diagnostiku es rakstu stderr, lai stdout paliktu tikai caption vai reģionu skaita tekstam
    # C++ puse tagad NEmerģē stderr atpakaļ uz stdout (skat. Florence.cpp), tāpēc šai rindai
    # vairs nevajadzētu nokļūt izmērītajā aprakstā. 
    print("Florence device: " + device, file=sys.stderr)

    # Ielādēju Florence processor
    processor = AutoProcessor.from_pretrained(model_name, trust_remote_code=True)

    # Ielādēju Florence modeli
    model = AutoModelForCausalLM.from_pretrained(
        model_name,
        trust_remote_code=True,
        torch_dtype=dtype
    )

    model = model.to(device)     # Te pārvietoju modeli uz izvēlēto ierīci

    model.eval()    # Te ieslēdzu inference režīmu

    return processor, model, device, dtype      # Te atgriežu visu, kas vajadzīgs ģenerēšanai

# Šī funkcija palaiž Florence-2 modeli ar norādīto uzdevuma prompt un atgriež neapstrādāto rezultātu
# To izmanto gan parastā apraksta ģenerēšana (<CAPTION>), gan reģionu noteikšana (<DENSE_REGION_CAPTION>)
def run_florence_task(processor, model, device, dtype, image_path, task):

    image = Image.open(image_path).convert("RGB")   # Atveru attēlu RGB režīmā
    inputs = processor(text=task, images=image, return_tensors="pt")    # Sagatavoju ievadi modelim ar doto uzdevuma prompt
    input_ids = inputs["input_ids"].to(device)  # Pārvietoju input_ids uz ierīci

    # Ja ir CUDA, pixel_values pārvēršu float16
    if device == "cuda":
        pixel_values = inputs["pixel_values"].to(device, dtype=dtype)   # Šeit es pārvietoju attēla tenzoru uz CUDA

    # Ja ir CPU, atstāju parasto tipu
    else:
        pixel_values = inputs["pixel_values"].to(device)    # Šeit es pārvietoju attēla tenzoru uz CPU

    # Te tiek izslēgti gradienti, jo modeli netrenēju
    with torch.no_grad():
        # Šeit es ģenerēju teksta tokenus. max_new_tokens ir paaugstināts no 64 uz 1024, salīdzinot ar
        # iepriekšējo versiju, jo reģionu noteikšanas uzdevumam (<DENSE_REGION_CAPTION>) vajag
        # daudz vairāk tokenu nekā parastam īsam aprakstam - katrs atrastais apgabals aizņem
        # vairākus tokenus. Parastajam <CAPTION> uzdevumam tas neko nemaina, jo modelis pats apstājas ar EOS tokenu, kad apraksts ir gatavs.
        generated_ids = model.generate(
            input_ids=input_ids,
            pixel_values=pixel_values,
            max_new_tokens=1024,
            num_beams=3
        )

    # Šeit es pārvēršu tokenus tekstā
    generated_text = processor.batch_decode(
        generated_ids,
        skip_special_tokens=False
    )[0]

    # Te Florence pēcapstrāde atgriež strukturētu rezultātu atkarībā no uzdevuma
    result = processor.post_process_generation(
        generated_text,
        task=task,
        image_size=(image.width, image.height)
    )

    return result   # Un tad atgriežu visu rezultāta vārdnīcu, izsaucējs paņems vajadzīgo daļu

# Šī funkcija ģenerē attēla aprakstu, izmantojot <CAPTION> uzdevumu
def generate_caption(processor, model, device, dtype, image_path):
    prompt = "<CAPTION>"    # Šis ir Florence caption uzdevuma prompts

    result = run_florence_task(processor, model, device, dtype, image_path, prompt) # Te tiek palaists Florence ar caption uzdevumu

    return result[prompt]   # Atgriežu caption tekstu

# Šī funkcija saskaita, cik reģionus (apgabalus) Florence atrada attēlā, izmantojot reģionu noteikšanas uzdevumu.
# To izmanto ImagePurpose klase C++ pusē kā vienkāršu attēla sarežģītības rādītāju.
def count_regions(processor, model, device, dtype, image_path, task):
    result = run_florence_task(processor, model, device, dtype, image_path, task)   # Te tiel palaists Florence ar doto reģionu noteikšanas uzdevumu

    # Ja rezultātā nav gaidītā uzdevuma atslēgas, uzskatu, ka reģionu nav
    if task not in result:
        return 0    # Atgriežu 0.

    # Ja rezultātā nav "bboxes" atslēgas, uzskatu, ka reģionu nav
    if "bboxes" not in result[task]:
        return 0    # Atgriežu 0.

    return len(result[task]["bboxes"])  # Atgriežu atrasto reģionu skaitu

# Šī ir galvenā funkcija
def main():
    parser = argparse.ArgumentParser()  # Te es izveidoju argumentu parseri
    parser.add_argument("image_path")   # Te es pievienoju attēla ceļu
    parser.add_argument("--model", default="microsoft/Florence-2-base") # Un šeit es pievienoju modeļa nosaukumu

    # Te es pievienoju uzdevuma karogu - noklusējumā parasts apraksts, bet var lūgt arī reģionu noteikšanu.
    # SVARĪGA PIEZĪME: šeit es lietoju vienkāršus vārdus ("caption" / "regions"), NEVIS tieši Florence-2 iekšējos uzdevuma marķierus ("<CAPTION>" / "<DENSE_REGION_CAPTION>"), 
    # kaut gan tie ir īstie marķieri, ko modelim vajag. Pirmajā versijā šeit tieši lietoju "<DENSE_REGION_CAPTION>" kā komandrindas parametru, 
    # un C++ puse to nodeva tālāk cmd.exe komandai - izrādījās, ka Windows cmd.exe simbolus "<" un ">" saprot kā failu novirzīšanas (redirect) zīmes pat pēdiņu iekšpusē,
    # un komanda vienmēr izgāzās ar kļūdu "The system cannot find the file specified" (cmd.exe mēģināja atvērt failu ar nosaukumu "DENSE_REGION_CAPTION" ievades novirzīšanai). 
    # Risinājums bija: komandrindā lietot vienkāršus vārdus bez īpašiem simboliem, un šeit, Python pusē, tos pārvērst par īstajiem Florence-2 marķieriem.
    parser.add_argument("--task", default="caption")

    args = parser.parse_args()  # Nolasu argumentus
    processor, model, device, dtype = load_model(args.model)     # Ielādēju modeli

    # Ja uzdevums ir parasts apraksts, es izvadu tikai caption tekstu
    if args.task == "caption":
        caption = generate_caption(processor, model, device, dtype, args.image_path)    # Ģenerēju caption
        print(caption)  # Izvadu tikai caption tekstu C++ programmai
        return

    # Ja uzdevums ir reģionu noteikšana, es pārvēršu vienkāršo vārdu par īsto Florence-2 marķieri
    if args.task == "regions":
        regionCount = count_regions(processor, model, device, dtype, args.image_path, "<DENSE_REGION_CAPTION>") # Šeit es izsaucu reģionu skaitīšanu ar īsto marķieri
        print(regionCount)  # Te es izvadu tikai reģionu skaitu C++ programmai
        return

    # Ja uzdevums nav atpazīts, tiek izvadīta kļūda uz stderr un beidzu ar neveiksmes kodu
    print("ERROR: unknown --task value: " + args.task, file=sys.stderr)
    sys.exit(1)

# Programmas palaišana
if __name__ == "__main__":
    main()  # Galvenās programmas izsaukšana
