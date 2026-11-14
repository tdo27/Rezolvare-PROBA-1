# Progres

Bifează `[x]` și fă un commit după fiecare pas.

## 0. Setup
- [x] Template-ul SDL3 descărcat în `src/` (peisajul e desenat din cod, nu are assets)
- [ ] Proiectul compilează și pornește (fereastră 700×800 cu peisajul care derulează)

## 1. Elemente vizuale (20 p)
- [ ] Nava verde 50×50, poziționată corect
- [ ] Inamici portocalii 50×50
- [ ] Proiectile roșii / magenta, rază 10

## 2. Mișcare inerțială constrânsă (40 p)
- [ ] Cursor ascuns + mouse capturat
- [ ] Accelerare din `xrel`/`yrel` (400 px/s²), viteză max 500 px/s
- [ ] Decelerare 800 px/s² când mouse-ul stă
- [ ] Nava nu iese din fereastră

## 3. Tragerea jucătorului (20 p)
- [ ] Proiectil la 0.3 s cât timp e apăsat click stânga, 500 px/s în sus

## 4. Inamici + tragerea lor (30 p)
- [ ] Felii verticale, max un inamic pe felie, viteză aleatoare 50–150 px/s
- [ ] Respawn la 3 s pentru feliile libere
- [ ] Tragere după 1 s, o salvă la 3 s, spre poziția navei, 100 px/s
- [ ] Cele 2 proiectile extra la ±15°

## 5. Repulsorul (30 p)
- [ ] Click dreapta → −10 px/s pe direcția spre navă, o dată per apăsare

## 6. Coliziuni (20 p)
- [ ] Proiectil jucător ↔ inamic
- [ ] Navă ↔ inamic, navă ↔ proiectil inamic

## 7. Finalul jocului (10 p)
- [ ] Victorie (albastru) / înfrângere (roșu), totul se oprește
- [ ] Reset cu R doar după final

## 8. Calitate (10 p)
- [ ] Mișcare bazată pe delta time, fluidă, independentă de FPS
- [ ] Poziții relative la dimensiunea ferestrei
- [ ] Obiectele ieșite din ecran sunt șterse (jocul poate rula la nesfârșit)
