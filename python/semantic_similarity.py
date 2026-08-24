# Šeit tiek iekļauts argumentu parseris, kļūdu izvade, sentence-transformers klases un palīgfunkcijas
import argparse
import sys
from sentence_transformers import SentenceTransformer, util

# Šī funkcija ielādē sentence-transformers modeli
def load_model(model_name):
    # Diagnostiku rakstu stderr, lai stdout paliktu tikai skaitļa tekstam - tas pats princips, ko jau lieto florence_caption.py, 
    # lai C++ puse varētu droši nolasīt tikai vienu rindu ar rezultātu.
    print("Semantic model: " + model_name, file=sys.stderr)

    model = SentenceTransformer(model_name)  # Modeļa ielāde

    return model    # Atgriežu ielādēto modeli

# Šī funkcija aprēķina semantisko līdzību starp diviem tekstiem
def compute_similarity(model, text1, text2):
    embeddings = model.encode([text1, text2])   # Šeit tiek pārvērsti abi teksti par vektoriem (embeddings)

    similarity = util.cos_sim(embeddings[0], embeddings[1])     # Tad aprēķināta kosinusa līdzību starp diviem vektoriem

    score = float(similarity[0][0])     # Tad es paņemu skaitlisko vērtību no rezultāta tenzora

    # Ja rezultāts kaut kā ir negatīvs (reti, bet teorētiski iespējams), aizstāju to ar 0, jo C++ pusē negatīva semantiskā līdzība nav jēgpilna
    if score < 0.0:
        score = 0.0     # Tad uzstādu uz 0.

    return score    # Un atgriežu līdzības vērtību

# Šī ir galvenā funkcija
def main():
    parser = argparse.ArgumentParser()      # Te es izveidoju argumentu parseri
    parser.add_argument("text1")        # Te es pievienoju pirmo tekstu
    parser.add_argument("text2")        # Te es pievienoju otro tekstu
    parser.add_argument("--model", default="all-MiniLM-L6-v2")  # Te es pievienojam modeļa nosaukumu
    args = parser.parse_args()      # Šeit nolasu argumentus
    model = load_model(args.model)      # Modeļa ielāde
    similarity = compute_similarity(model, args.text1, args.text2)  # Līdzības aprēķins

    print(similarity)   #  Te tiek izvests tikai skaitlis c++ programmai

# Programmas palaišana
if __name__ == "__main__":
    main()      # Galvenās funkcijas izsaukšana
