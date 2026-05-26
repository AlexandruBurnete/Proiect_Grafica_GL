# Trotinetă 3D – OpenGL (Visual Studio 2022, x86)

Proiect C/C++ OpenGL care desenează o **trotinetă 3D** într-o scenă simplă (sol + grilă), cu:
- iluminare punctuală (GL_LIGHT0)
- obiect “bec” care marchează poziția luminii
- **umbră proiectată** pe plan (shadow matrix)
- animație roți (wheel spin)
- control din tastatură pentru mișcare stânga/dreapta și pentru poziția luminii

> Proiectul folosește biblioteci vechi de tip `glaux` / `glut` (stil laborator). Din acest motiv este recomandat să rulezi în **x86**.

---

## Demo / Controale
### Mișcare trotinetă
- `A` / `a` – stânga
- `D` / `d` – dreapta

### Mutare lumină (bec)
- `J` / `j` – stânga (X-)
- `L` / `l` – dreapta (X+)
- `U` / `u` – sus (Y+)
- `O` / `o` – jos (Y-) *(cu limită minimă)*
- `I` / `i` – în scenă (Z-)
- `K` / `k` – în afară (Z+)

---

## Ce conține codul (pe scurt)
- `DrawCylinder`, `DrawDisk`, `DrawBox` – primitive (cilindru, disc, cutie)
- `DrawWheel` – roată (cilindru + material)
- `DrawScooterModel` – model trotineta (deck + roți + ghidon)
- `ShadowMatrix` – matrice pentru umbra proiectată pe planul `y=0`
- `display()` – randare completă (sol + bec + trotinetă + umbră)
- `IdleFunction()` – animație (spin roți) + refresh ~60 FPS

---

## Cerințe
- Windows
- Visual Studio 2022
- Platformă: **x86** (important)
- Set de fișiere/librării OpenGL de laborator:
  - headere: `glos.h`, `glaux.h`, `gl.h`, `glu.h`, `glut.h` etc.
  - librării: `glaux.lib`, `glu32.lib`, `glut32.lib`, `glui32.lib`, `opengl32.lib`
  - dll-uri: `opengl32.dll`, `glu32.dll`, `glut32.dll` (în funcție de arhivă)

---

## Setup proiect în Visual Studio 2022 (pas cu pas)

### 1) Creare proiect
1. `File -> New -> Project`
2. Alege un proiect C/C++ (de ex. **Empty Project** sau Console/Windows Application)
3. Creează proiectul într-un folder dedicat (recomandat să ai toate proiectele de lab în același “base folder”).

### 2) Selectează platforma x86
În Visual Studio:
- sus, lângă **Debug**, selectează **x86** (nu x64)

> Dacă nu există x86: `Build -> Configuration Manager -> Active solution platform -> New... -> x86`

### 3) Copiază fișierele externe în folderul proiectului
- Dezarhivează arhiva cu `include + lib + dll` în **folderul proiectului** (nu în folderul soluției).
- Structura tipică poate fi:
```text
<proiect>/
  GL/                (folder cu .lib)
  dlls/              (folder cu .dll)
  (header files .h)
  (source files .cpp)
```

### 4) Adaugă DLL-urile în proiect (și copiere la build)
1. Click dreapta pe Solution -> **Add -> New Filter**
2. Creează un filter numit `dlls`
3. Click dreapta pe filter `dlls` -> **Add -> Existing Item...**
4. Selectează DLL-urile: `glu32.dll`, `glut32.dll`, `opengl32.dll` (în funcție de ce ai)
5. Selectează toate DLL-urile în tree -> **Properties**
   - `Content` -> **No**
   - `Item Type` -> **Copy File**
6. Apply/OK

### 5) Adaugă header-ele
1. Click dreapta `Header Files` -> **Add -> Existing Item...**
2. Selectează headerele (ex: `GL.H`, `GLAux.h`, `GLOS.H`, `GLU.H`, `glut.h`, `glui.h` etc.)

### 6) Adaugă fișierul sursă (codul)
1. Copiază fișierul `.cpp` în folderul proiectului
2. Click dreapta pe Solution -> **Add -> Existing Item...**
3. Selectează fișierul sursă și OK

---

## Configurare proiect (Properties)

Click dreapta pe proiect -> **Properties**:

### A) Character Set
- `Configuration Properties -> Advanced`
- `Character Set` = **Use Multi-Byte Character Set**

### B) Linker: Additional Library Directories
- `Linker -> General`
- `Additional Library Directories` = `GL`

> Dacă folderul cu librării are alt nume/cale, setează calea corectă, ex:
`$(ProjectDir)GL`

### C) Linker: Additional Dependencies
- `Linker -> Input`
- `Additional Dependencies`:
  - `glu32.lib`
  - `glut32.lib`
  - `glui32.lib`
  - `opengl32.lib`
  - `glaux.lib`
  - `legacy_stdio_definitions.lib`

### D) SAFESEH
- `Linker -> Advanced`
- `Image Has Safe Exception Handlers` = **No (/SAFESEH:NO)**

Apasă **OK** pentru salvare.

---

## Probleme frecvente
### 1) Proiectul nu rulează pe x64
Bibliotecile de laborator sunt adesea doar x86. Soluție: rulează proiectul în **x86**.

### 2) Lipsesc DLL-uri la rulare
Asigură-te că DLL-urile sunt copiate lângă executabil (sau sunt configurate ca „Copy File”).

### 3) Include/Lib paths greșite
Verifică:
- `Additional Library Directories`
- dacă folderul `GL` există și conține `.lib`

---

## Autor
Alexandru Burnete  
(Completează aici cu link-ul repo-ului / numele complet dacă vrei)
