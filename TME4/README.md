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

Interblocage car on lock 2 fois le meme mutex:

- Premier lock sur le compte debiteur
- Deuxieme lock dans la fonction debiter, sur le meme mutex

## Question 6

Interblocage car on lock dans 2 ordres inverses:

- Premier couple 10, 15
- (thread 1) 10 est lock, commutation
- Deuxieme couple 15, 10
- (thread 2) 15 est lock
- (thread 1) essaye d'obtenir 15, lock
- (thread 2) essaye d'obtenir 10, lock

Pour remedier a ce probleme, utilisation de `scoped_lock` qui ordonne les
operations lock en fonction de l'addresse des mutex (donc on garde toujours le
meme orde, peut importe i/j) unlock a la fin du scope.

## Question 7

Lorsque le thread comptable est en train de tourner, il est possible qu'en plein
millieu de la boucle, un transfer ait lieu, ce qui va fausser le resultat final.
