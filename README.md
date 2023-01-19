                               
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
  
 
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

###UPDATE_Prezentare

## Cerinte keylogger in modul kernel:


1.  Scriere **keylogger** in modul kernel.
2.  Toate tasele apasate de la tastatura se vor salva intr-un fisier **"log.txt"**. De asemenea, tastele sunt salvate si in fisierul victimei (local) "Victim.txt"
  2a. Rolul acestui fisier este pentru a se stoca toate tastele pe care le apasa victima 
  2b. Tastele sunt trimite printr-un socket din user space in kernel space pentru a putea fi prelucrate de catre atacator cu ajutoru algoritmului dedicat.
3.  Daca fisierul nu exista acesta se va crea. (log.txt)
4.  Se introduce modulul in modul kernel cu comanda insmod.
5.  Se sterge modulul din modul kernel cu comanda rmmod.
6.  La extragerea din modul kernel se salveaza continutul fisierului.
  6a. Plus trimiterea in cazul in care victima apasa comanda de taste (CTRL+C_semnalul SIGINT), tot continutul din bufferul populat cu tastele apasate vor fi scrise          imediat in fisierul victimei pentru a fi trimise prin socket la un interval de timp (7 secunde).
  6b. Timp de 7 secunde victima tasteaza tastele, apoi la expirarea coantei de timp se va trimite tot pentru prelucrare.
7.  Citirea de la tastatura se va face intr-un buffer cu lungime variabila, cand se atinge
    o anumita dimensiune **NR_MAX** se salveaza bufferul din fisier.
8.  Codurile **"scancodes"** de la tastatura vor fi traduse in ASCII si salvate codificat ASCII in fisier.
  ca sa nu facem inca o intrerupere pentru fiecare tasta citita.
10. Fisierul de log va fi ascuns ( exista 2 fisiere: fisierul victimei, <nu se va cunoaste existenta acestui fisier> si fisierul atacatorului)
12. Pentru a putea ajunge sa luam tastele, trebuie sa gasim prima data portul tastaturii disponibil in fisierul /proc/ioports
13. Vom dezvolta un utilitar in care se va incarca un dictionar intr-un arbore de tip trie si se vor incerca toate cuvintele
    si in cazul gasirii unor **match-uri** ele vor fi afisate atacatorului.


----------------------------

## Modul de utilizare al keylogger-ului:
 # In cadrul victimei:
  Folosim structura nlmsghdr pentru transferul de date atacator-victima. Mesajele de trimis sunt copiate in campul "NLMSG_DATA" si sunt trimise cu ajutorul functiei "sendmsg" si primite cu "recvmsg".
  Pe parcursul rularii, in timpul alocat de 7 secunde utilizatorul va apasa tastele, ele se vor scrie intr-un buffer iar la expirarea timpului continutul din buffer va fi trimis catre keylogger. De asemenea, in cazul opririi victimei se va scrie tot continutul bufferului in fisierul local al victimei ( s-a tratat printr-un handle semnalul. ).
  Cu ajutorul arborelui trie verificam daca ceea ce este scris la tastatura reprezinta un cuvant real nu neaparat numai "gunoi" scris de la tastatura in mod random( se va avea grija sa nu reprezinte caractere din cadrul unei parole - ex: Google random passwords).
  In arbore exista un dictionar de cuvinte cu care va fi verificat ceea ce se primeste de la apasarile tastelor.
  Pentru conexiunea intre kernel mode si user mode am folosit "NETLINK".
  Netlink este o familie de socketi folosita pentru comunicatiile dintre procese din userspace-uri diferite (in cazul nostru kernel mode - user mode).
 # In cadrul atacatorului:
   Matricea "scancode_to_text" este folosita pentru a stoca toate caracterele de pe tastatura in mod uppercase sau lowercase.
   Intrucat suntem in modul kernel nu putem folosi functiile POSIX pentru manipularea fisierelor asa ca folosim metodele din modul kernel pentru deschidere fisier, scriere sau inchiderea lui. De asemenea am folosit functii din modul kernel pentru primirea calupurilor de date primite din user mod prin socketul dedicat.
   Operatiile cu fisierele se fac cu filp_open, filp_close, kernel_write, kernel_read.
   Utilizam fisierul pr_debug pentru a citi scan-code urile si le introducem in buffer.
   Functia "send_reply" este utilizata in comunicatia cu usermode-ul prin acelasi mod de lucru ca in cazul victimei.
   Translatam scancode-urile citite si existente in buffer cu ajutorul matricii ( in functia keycode_to_string).
   
  
## Comenzi compliare si inserare in kernel:
` make`
`insmod keylogger.ko`
