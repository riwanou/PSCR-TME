# TME sujet semaine 4 : thread, atomic, mutex

[Programmation Système Répartie et Concurrente : Homepage](https://pages.lip6.fr/Yann.Thierry-Mieg/PR)

[Sujet du TME en PDF](https://pages.lip6.fr/Yann.Thierry-Mieg/PR/tdtme4.pdf)

(c) Sorbonne Université 2018-2019

## Question 2

On a des problemes de undefined behaviour avec datarace car la fonction
transfert de banque n'est pas atomique. Donc si il y a commutation, on peut
arriver a un cas ou un compte est debite plusieurs fois et le crediteur pas
credite directement.

## Question 5

Blocage car on lock 2 fois le meme mutex:

- Premier lock sur le compte debiteur
- Deuxieme lock dans la fonction debiter, sur le meme mutex

## Question 6
