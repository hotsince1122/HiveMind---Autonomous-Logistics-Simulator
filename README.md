Proiect HiveMind – Enache Luca Ștefan 322AC

Descrierea algoritmului HiveMind (V1 – V2): 
HiveMind este un algoritm greedy ce asociază fiecărui pachet câte o rută și un agent.

HiveMind V1:
Acesta iterează prin toate pachetele din hub (cu statusul WAITING), la fiecare iterație trecând prin toți agenții. 
Pentru fiecare agent, stabilim mai întâi dacă îl putem lua în considerare: verificăm dacă are baterie suficientă 
pentru a ajunge la destinație și înapoi (evităm pe cât posibil moartea agenților, deoarece moartea unui agent este 
penalizată cu mult mai mult decât nelivrarea unui pachet). Dacă se încadrează în acest criteriu, calculăm cât de 
profitabil este să îl trimitem pe fiecare (package reward – agent cost), și îl alegem pe cel mai ieftin. De asemenea, 
la calculele costurilor (baterie / bani) luăm o marjă de eroare de 5%.

HiveMind V2:
Upgrade-ul față de V1 este faptul că acum algoritmul este mult mai agresiv, folosindu-se de stații. Acum, pe lângă 
faptul că livrăm toate pachetele care încă au rămas în bază chiar dacă au fost marcate cu EXPIRED (deoarece livrarea 
unui pachet întârziat este -50 față de nelivrare, care este -200), luăm în considerare și capacitatea agenților. 
Calculul de considerare a agentului este puțin diferit, acum condiția este ca agentul să aibă suficientă baterie 
să ajungă la destinație + cea mai apropiată stație de încărcare, care se află pe drum spre înapoi (dacă există, dacă 
nu calculăm pentru cea mai apropiată stație de destinație). După trimiterea agentului, HiveMind V2 se ocupă și de 
întoarcerea agenților înapoi la hub. După ce lasă pachetele, dacă agentul are suficientă baterie să ajungă la hub, 
va fi trimis direct acolo, dacă nu, îl trimitem spre cea mai apropiată stație, care se află pe drumul spre hub, și 
dacă nu există pe drum, îl trimitem la cea mai apropiată stație de unde se află. Astfel, maximizăm profitul, deoarece 
folosim capacitatea maximă a agenților și stațiile de pe hartă.

Descrierea algoritmului de generare a hărții (POI = point of interest)
Primul pas este citirea configurărilor din fișierul simulation_setup.txt. Creăm o hartă goală de dimensiunile 
specificate (fiecare celulă este de tipul „ROAD"). Vom avea un vector de indecși „occupied", ce contorizează unde 
punem POI-uri. Generăm o pereche de indecși random, la care vom pune hub-ul. Pentru clienți și stații, folosim la fel 
indecși randomși, însă îi validăm și punem clienții / stațiile, doar dacă respectă o anumită distanță de restul de 
POI-uri, pe care o calculăm cu ajutorul vectorului „occupied" (plecăm de la distanța minimă de 4 între POI-uri, și 
scădem distanța de fiecare dată când nu se mai găsesc locuri valide). Ideea sistemului acesta este să nu avem POI-uri 
unul lângă altul, chiar dacă și acest lucru poate fi complet random, astfel împrăștiind pe toată harta obiectivele. 
Pereții îi punem la sfârșit, generăm random indecșii pentru partea de stânga sus a peretelui, și decidem în funcție 
de niște parametri cât de mare să fie peretele (2 x 4, 4 x 3 etc.), punându-l doar dacă nu este peste vreun POI. Ne 
oprim din generat pereți doar atunci când atingem „wallDensity" minim, adică cât la sută din hartă este perete. La 
sfârșit, verificăm dacă harta este validă, folosind bfs și flood fill. Dacă aceasta nu este validă, repetăm procesul 
până avem o hartă validă, sau se atinge numărul de încercări de generare.
