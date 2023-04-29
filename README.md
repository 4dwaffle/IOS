# IOS – projekt 2 (synchronizace)

Zadání je inspirováno knihou Allen B. Downey: The Little Book of Semaphores (The barbershop
problem)

## Popis Úlohy (Pošta)

V systému máme 3 typy procesů: (0) hlavní proces, (1) poštovní úředník a (2) zákazník. Každý zákazník jde na poštu vyřídit jeden ze tří typů požadavků: listovní služby, balíky, peněžní služby. Každý požadavek je jednoznačně identifikován číslem (dopisy:1, balíky:2, peněžní služby:3). Po příchodu se zařadí do fronty dle činnosti, kterou jde vyřídit. Každý úředník obsluhuje všechny fronty (vybírá pokaždé náhodně jednu z front). Pokud aktuálně nečeká žádný zákazník, tak si úředník bere krátkou přestávku. Po uzavření pošty úředníci dokončí obsluhu všech zákazníků ve frontě a po vyprázdnění všech front odhází domů. Případní zákazníci, kteří přijdou po uzavření pošty, odcházejí domů (zítra je také den).

### Podrobná specifikace úlohy

#### Spuštění:

$ ./proj2 NZ NU TZ TU F

- NZ: počet zákazníků
- NU: počet úředníků
- TZ: Maximální čas v milisekundách, po který zákazník po svém vytvoření čeká, než vejde na
    poštu (eventuálně odchází s nepořízenou). 0<=TZ<=
- TU: Maximální délka přestávky úředníka v milisekundách. 0<=TU<=
- F: Maximální čas v milisekundách, po kterém je uzavřena pošta pro nově příchozí.
    0<=F<=

#### Hlavní proces

- Hlavní proces vytváří ihned po spuštění NZ procesů zákazníků a NU procesů úředníků.
- Čeká pomocí volání usleep náhodný čas v intervalu <F/2,F>
- Vypíše: _A: closing_
- Poté čeká na ukončení všech procesů, které aplikace vytváří. Jakmile jsou tyto procesy
    ukončeny, ukončí se i hlavní proces s kódem (exit code) 0.

#### Proces Zákazník

- Každý zákazník je jednoznačně identifikován číslem idZ, 0<idZ<=NZ
- Po spuštění vypíše: _A: Z idZ: started_
- Následně čeká pomocí volání usleep náhodný čas v intervalu <0,TZ>
- Pokud je pošta uzavřena
    ◦ Vypíše: A: _Z idZ: going home_
    ◦ Proces končí
- Pokud je pošta otevřená, náhodně vybere činnost X---číslo z intervalu <1,3>
    ◦ Vypíše: _A: Z idZ: entering office for a service X_
    ◦ Zařadí se do fronty X a čeká na zavolání úředníkem.
    ◦ Vypíše: _Z idZ: called by office worker_
    ◦ Následně čeká pomocí volání usleep náhodný čas v intervalu <0,10> (synchronizace s
       úředníkem na dokončení žádosti není vyžadována).
    ◦ Vypíše: _A: Z idZ: going home_
    ◦ Proces končí

#### Proces Úředník

- Každý úředník je jednoznačně identifikován číslem idU, 0<idU<=NU
- Po spuštění vypíše: _A: U idU: started_
    [začátek cyklu]
- Úředník jde obsloužit zákazníka z fronty X (vybere náhodně libovolnou neprázdnou).
    ◦ Vypíše _: A: U idU: serving a service of type X_
    ◦ Následně čeká pomocí volání usleep náhodný čas v intervalu <0,10>
    ◦ Vypíše _: A: U idU: service finished_
    ◦ Pokračuje na [začátek cyklu]
- Pokud v žádné frontě nečeká zákazník a pošta je otevřená vypíše
    ◦ Vypíše _: A: U idU: taking break_
    ◦ Následně čeká pomocí volání usleep náhodný čas v intervalu <0,TU>
    ◦ Vypíše _: A: U idU: break finished_
    ◦ Pokračuje na [začátek cyklu]
- Pokud v žádné frontě nečeká zákazník a pošta je zavřená
    ◦ Vypíše _: A: U idU: going home_
    ◦ Proces končí

## Příklad výstupu

Příklad výstupního souboru proj2.out pro následující příkaz:

$ ./proj2 3 2 100 100 100

1: U 1: started\
2: Z 3: started\
3: Z 1: started\
4: Z 1: entering office for a service 2\
5: U 2: started\
6: Z 2: started\
7: Z 3: entering office for a service 1\
8: Z 1: called by office worker\
9: U 1: serving a service of type 2\
10: U 1: service finished\
11: Z 1: going home\
12: Z 3: called by office worker\
13: U 2: serving a service of type 1\
14: U 1: taking break\
15: closing\
16: U 1: break finished\
17: U 1: going home\
18: Z 2: going home\
19: U 2: service finished\
20: U 2: going home\
21: Z 3: going home\


