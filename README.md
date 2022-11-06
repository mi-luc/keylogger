----------------------------
##CERINTE KEYLOGGER IN MODUL KERNEL:


1. Scriere **keylogger** in modul kernel.
2. Toate tasele apasate de la tastatura se vor salva intr-un fisier **"log.txt"**.
3. Daca fisierul nu exista acesta se va crea.
4. Se introduce modulul in modul kernel cu comanda insmod.
5. Se sterge modulul din modul kernel cu comanda rmmod.
6. La extragerea din modul kernel se salveaza continutul fisierului
7. Citirea de la tastatura se va face intr-un buffer cu lungime variabila, cand se atinge
o anumita dimensiune **NR_MAX** se salveaza bufferul din fisier.
8. Se vor intercepta intreruperile efectuate de tastatura IRQ.
9. Codurile **"scancodes"** de la tastatura vor fi traduse in ASCII si salvate codificat ASCII in fisier.
10. Pentru eficienta setul de taste citite va fi transmis cand se va atinge **NR_MAX** printr-un tasklet.
ca sa nu facem inca o intrerupere pentru fiecare tasta citita.
11. Fisierul de log va fi ascuns.
12. Pentru a putea ajunge sa luam tastele, trebuie sa gasim prima data portul tastaturii disponibil in fisierul /proc/ioports
13. Vom dezvolta un utilitar in care se va incarca un dictionar intr-un arbore de tip p si se vor incerca toate cuvintele
si in cazul gasirii unor **match-uri** salvam intr-un fisier "match.txt".