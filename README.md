# CodeRun7 – Hard Task: Vertical Scrolling Shooter (SDL3)

Repo de învățare pentru **Hard Task** din CodeRun7 (TUCN, noiembrie 2026):
un joc simplu de tip *vertical scrolling shooter* scris în **C/C++** cu **SDL3** (+ GLM).

## Structura

```
coderun-hard-task/
├── docs/
│   ├── CodeRun_training_and_tasks.pdf   # enunțul original
│   ├── HardTask_demo.mov                # demo-ul jocului
│   └── TASK.md                          # enunțul Hard Task, rescris pe puncte
├── src/                                 # codul jocului (pornind de la template-ul oficial)
├── assets/                              # imagini/texturi din template (peisajul care derulează)
├── CMakeLists.txt                       # build cu SDL3 + GLM descărcate automat
├── PROGRESS.md                          # checklist cu cerințele și punctajul
└── .gitignore
```

## Pornire

1. Descarcă **template-ul SDL3** din linkul din PDF (pagina 2, „SDL3 application template”)
   și pune fișierele `.cpp/.h` în `src/`, iar imaginile/texturile în `assets/`.
   Folosirea template-ului este **obligatorie** pentru punctaj.
2. Build (Linux / macOS / Windows cu CMake ≥ 3.24):

   ```bash
   cmake -S . -B build
   cmake --build build
   ./build/shooter          # pe Windows: build\Debug\shooter.exe
   ```

   La primul build CMake descarcă SDL3 și GLM (durează câteva minute).
   Dacă folosești proiectele preconfigurate (Visual Studio 2022 / CLion) din PDF,
   poți ignora `CMakeLists.txt` și lucra direct în ele.

## Cum lucrez

- Câte un commit pentru fiecare cerință bifată în [`PROGRESS.md`](PROGRESS.md).
- Enunțul pe scurt: [`docs/TASK.md`](docs/TASK.md).
