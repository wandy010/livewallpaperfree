# 📺 LiveWallpaperFree

**LiveWallpaperFree** es un clon ligero de Wallpaper Engine desarrollado para Windows. Permite establecer videos como fondo de pantalla dinámico, insertando una ventana de reproducción detrás de los iconos del escritorio.

Este proyecto fue desarrollado íntegramente con la asistencia de **opencode IA**, aplicando ingeniería inversa a la gestión de ventanas de Windows y migrando motores de video para optimizar el rendimiento.

---

## 🚀 Cómo funciona (Arquitectura Técnica)

El programa utiliza una combinación de técnicas avanzadas de la API de Windows y motores de video externos:

### 1. El "Truco" del Escritorio (WorkerW)
Para que el video quede **detrás de los iconos**, el programa no crea una ventana normal. Utiliza un mensaje especial (`0x052C`) enviado a `Progman`, lo que obliga a Windows a crear una ventana llamada `WorkerW`. El programa detecta esta ventana y "engancha" la ventana del video justo debajo de ella.

### 2. Motor de Video: libmpv (Dynamic Loading)
Para evitar dependencias pesadas y errores de compilación, el programa utiliza **libmpv** mediante **Carga Dinámica**:
- No requiere que la librería esté instalada en el sistema.
- El programa busca la DLL (`libmpv-2.dll`) en su propia carpeta al iniciar.
- Carga las funciones de reproducción en tiempo de ejecución, lo que lo hace extremadamente flexible y compatible con casi cualquier formato de video (`.mp4`, `.mkv`, `.webm`, etc.).

### 3. Gestión de Recursos
- **Aceleración por Hardware**: Configurado para usar la GPU mediante el renderizador `gpu` de mpv.
- **Detección de Pantalla**: Pausa automáticamente el video cuando el usuario entra en modo pantalla completa o cuando el escritorio está oculto, ahorrando CPU y RAM.

---

## 🛠️ Instalación y Compilación

### Requisitos
- **Visual Studio 2019 o 2022** (con la carga de trabajo "Desarrollo para el escritorio con C++").
- **CMake 3.15+**.
- **Visual C++ Redistributable** (instalado en el sistema).

### Pasos para Compilar
1. Clona el repositorio.
2. Ejecuta el script de construcción:
   - En CMD: `scripts\\build.bat`
   - En PowerShell: `.\\scripts\\build.ps1`
3. El ejecutable se generará en `build\\Release\\WallpaperEngineClone.exe`.

### ⚠️ Paso Final Obligatorio (La DLL)
Para que el video funcione, debes colocar la librería de mpv en la carpeta del ejecutable:
1. Descarga el paquete **`mpv-dev-x86_64`** desde los builds de shinchiro.
2. Copia el archivo **`libmpv-2.dll`** y pégalo en:
   `build\\Release\\libmpv-2.dll`

---

## 📖 Cómo usarlo

1. **Ejecuta** el programa desde `build\\Release\\WallpaperEngineClone.exe`.
2. **Configura el video**: 
   - Puedes editar el archivo `assets\\config\\settings.json` y cambiar la ruta en `"video_path"`.
   - O usa el icono de la bandeja del sistema (Tray Icon) $\rightarrow$ **Settings** para elegir el video.
3. **Control**: Haz clic derecho en el icono de la bandeja para pausar, silenciar o abrir los ajustes.

---

## 🎯 ¿Qué falta para que sea una "App Pro"?

Aunque el motor ya es estable, para convertirlo en una aplicación completa faltaría:

- [ ] **Interfaz Gráfica (GUI) completa**: Actualmente los ajustes son básicos o vía JSON. Un panel moderno en Qt o ImGui sería ideal.
- [ ] **Soporte de Playlists**: Poder poner una carpeta de videos que roten automáticamente.
- [ ] **Auto-inicio con Windows**: Implementar la entrada en el registro de Windows para que el fondo inicie con la PC.
- [ ] **Soporte Multi-monitor**: Permitir un video diferente para cada monitor.
- [ ] **Instalador Automático**: Un `.exe` de instalación que incluya la DLL y configure todo sin necesidad de compilar.
- [ ] **Soporte para Web/Shaders**: Implementar fondos basados en HTML/JS o Shaders (como hace el Wallpaper Engine original).

---
*Desarrollado con la ayuda de opencode IA xd*
