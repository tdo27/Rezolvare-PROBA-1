# Hard Task – enunț pe puncte

Sursa: `CodeRun_training_and_tasks.pdf`, secțiunea „Hard Task”. Demo: `HardTask_demo.mov`.

## Fereastra și scena
- Fereastră SDL3 de **700 × 800** px.
- Peisajul care derulează vertical este deja în template.
- **Toate pozițiile sunt relative la centru** și trebuie să rămână corecte dacă se schimbă dimensiunea ferestrei (fără valori hardcodate pe 700×800).
- Jocul trebuie să poată rula la nesfârșit.

## Nava jucătorului
- Dreptunghi **verde**, 50 × 50 px.
- Poziție inițială: pe linia centrală a ferestrei, la jumătate din înălțimea ei (25 px) de marginea de jos.
- Controlată cu mouse-ul, **fără să iasă din fereastră**; cursorul ascuns și mouse-ul capturat (`SDL_SetWindowRelativeMouseMode`).
- **Mișcare inerțială**:
  - accelerează în funcție de mișcarea relativă a mouse-ului (`event.motion.xrel`, `event.motion.yrel`), cu o tărie de **400 px/s²**;
  - viteză maximă **500 px/s**;
  - când mouse-ul se oprește, decelerează cu o tărie **de 2 ori** mai mare (800 px/s²).

## Inamici
- Dreptunghiuri **portocalii**, 50 × 50 px.
- Intră pe sus și coboară cu viteză constantă, aleasă aleator în **[50, 150] px/s**.
- Fereastra e împărțită în **felii verticale egale**; fiecare inamic e plasat aleator pe orizontală în felia lui.
- **Maxim un inamic activ pe felie**.
- La fiecare **3 secunde**, dacă o felie e liberă, se generează un inamic nou pentru ea.

## Proiectilele jucătorului
- Cercuri **roșii**, rază 10 px, pornesc din centrul navei, merg în sus cu **500 px/s**.
- Cât timp e apăsat **click stânga**: câte un proiectil la fiecare **0.3 s**; la eliberare se opresc.

## Proiectilele inamicilor
- Fiecare inamic începe să tragă la **1 s după ce intră în fereastră**.
- Cercuri **magenta**, rază 10 px, pornesc din centrul inamicului.
- Direcția: spre poziția navei **din momentul tragerii**; viteză **de 5 ori mai mică** decât a jucătorului (100 px/s).
- Pe lângă proiectilul principal, încă **2 proiectile** rotite la **+15° și −15°**.
- Rată de tragere **de 10 ori mai mică** decât a jucătorului (o salvă la 3 s).

## „Repulsor” (armă specială)
- La **click dreapta**, toate proiectilele inamice sunt încetinite cu **10 px/s**, împingându-le în direcția opusă navei.
- Efectul se aplică **o singură dată per apăsare**, dar apăsările repetate se cumulează, până la punctul în care proiectilele se îndepărtează de navă.

## Coliziuni și finalul jocului
- Proiectil jucător + inamic → ambele dispar.
- Nava + inamic sau nava + proiectil inamic → **înfrângere**.
- Toți inamicii distruși → **victorie**.
- La final: totul se oprește (inclusiv derularea și nava), inamicii și proiectilele dispar; nava devine **roșie** la înfrângere, **albastră** la victorie.
- Tasta **R** resetează jocul, dar **doar după ce s-a terminat**.

## Punctaj (total 220)
| Criteriu | Puncte |
|---|---|
| Prezență | 20 |
| Elemente vizuale corecte (navă, inamici, proiectile) | 20 |
| Mișcare inerțială constrânsă a navei | 40 |
| Proiectilele navei și tragerea | 20 |
| Proiectilele inamicilor și tragerea | 30 |
| Repulsorul | 30 |
| Detectarea și rezolvarea coliziunilor | 20 |
| Victorie, înfrângere, resetare | 10 |
| Mișcare constantă, fluidă, cu viteze corecte | 10 |
| Bonus de timp (1 p/min, max 20; doar dacă e rezolvat ≥ 50%) | 20 |
