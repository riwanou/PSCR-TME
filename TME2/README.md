# TME2

- Riwan Coeffic-Qabali / 21309062
- Florent Bordas / 21308825

## Modification et Correction d'un parser

## TME3

### Question 9

Dans le cas ou on a une classe `Personne (nom, prenom, age, sexe...)` et un
`vector<Personne>`, pour construire efficacement les sous groupes des personnes
qui ont le meme age / prenom :

- Creation de `unordered_map<type_age, vector<Personne*>` /
  `unordered_map<type_prenom, vector<Personne*>>`.
- Recuperation des 2 vecteurs en utilisant `find(valeur_age)` /
  `find(valeur_prenom)`.
- Union des 2 vecteurs, on pourra utiliser un set pour eviter les doublons.
