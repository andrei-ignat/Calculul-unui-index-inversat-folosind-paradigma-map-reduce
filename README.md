# Calculul-unui-index-inversat-folosind-paradigma-map-reduce

Descriere Implementare

   Pentru mapper, am creeat o coada cu fisiere si dupa fiecare thread extragea o element din coada,
parcurgeam fisierul si dupa scriam intr-o structura map cuvantul si in ce fisier se afla. Totodata,
am folosit bariere pentru astepta sa finaleze mapurile executia si apoi sa continue reducerii.
   Dupa ce si-au terminat mapurile executia, era randul reducerilor. Pentru asta am folosit o coada cu literele
de la a la z si fiecare thread reducer extragea cate o litera din coada. Dupa cu ajutorul unei structuri de data
map am selectat toata cuvintele dintr-un map global care incep cu o litera respectiva. Dupa aceea, am facut sortare,
apoi scrie in fisierul cu litera semnificativa cuvintele in ordine. 
