                               
----------------------------
## Cerinte keylogger in modul kernel:


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


----------------------------

# Modul de utilizare al keylogger-ului:
  Matricea us_keymap este folosita pentru a stoca toate caracterele de pe tastatura in mod uppercase sau lowercase.
  Intrucat suntem in modul kernel nu putem folosi functiile POSIX pentru manipularea fisierelor asa ca folosim metodele din modul kernel pentru deschidere fisier, scriere sau inchiderea lui.
  In functia keycode_to_string translatam scan code -urile citite de la tastatura intr-un caracter propriu-zis ce urmeaza sa fie scris in buffer.
  Toate caracterele sunt scrise intr-un buffer si atunci cand bufferul este plin, continutul este scris in fisierul de output.
  Exista structura spy_blk ...
  Cu ajutorul arborelui trie verificam daca ceea ce este scris la tastatura reprezinta un cuvant real nu neaparat numai "gunoi" scris de la tastatura in mod random( se va avea grija sa nu reprezinte caractere din cadrul unei parole - ex: Google random passwords).
  In arbore exista un dictionar de cuvinte cu care va fi verificat ceea ce citeste keyloggerul.
  
## Comenzi compliare si inserare in kernel:
` make`
`insmod keylogger.ko`
  
 
