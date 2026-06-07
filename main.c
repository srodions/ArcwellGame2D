#include <psxapi.h>
#include <psxgpu.h>
#include <psxetc.h>

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// Структуры для двойной буферизации (чтобы кадры не мерцали)
static DISPENV dispEnv[2];
static DRAWENV drawEnv[2];
static int db = 0;

void init_graphics(void) {
    // 1. Полный сброс графического процессора PS1
    ResetGraph(0);
    
    // 2. Настраиваем два буфера кадра (один на координатах 0,0, второй ниже — на 0,256)
    SetDefDispEnv(&dispEnv[0], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDrawEnv(&drawEnv[0], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SetDefDispEnv(&dispEnv[1], 0, 256, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDrawEnv(&drawEnv[1], 0, 256, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // 3. Включаем автоматическую очистку заднего фона черным цветом (0, 0, 0)
    drawEnv[0].isbg = 1;
    setRGB0(&drawEnv[0], 0, 0, 0);
    
    drawEnv[1].isbg = 1;
    setRGB0(&drawEnv[1], 0, 0, 0);
    
    // 4. Применяем начальные настройки для отрисовки первого кадра
    PutDispEnv(&dispEnv[0]);
    PutDrawEnv(&drawEnv[0]);
    
    // 5. Включаем вывод изображения на телевизор
    SetDispMask(1);
}

int main(void) {
    // Вызываем нашу инициализацию видеорежима
    init_graphics();
    
    // ТЕПЕРЬ МОЖНО: Загружаем текстуру встроенного шрифта BIOS в VRAM (координаты 960, 0)
    FntLoad(960, 0);
    
    // Открываем текстовый поток (X, Y, Ширина, Высота, Автоочистка фона, Макс. символов)
    int text_stream = FntOpen(16, 16, SCREEN_WIDTH - 32, SCREEN_HEIGHT - 32, 0, 512);

    // Главный игровой цикл
    while (1) {
        // Переключаем буферы (двойная буферизация)
        db = !db;
        PutDispEnv(&dispEnv[db]);
        PutDrawEnv(&drawEnv[db]);

        // Записываем строчки в буфер выбранного текстового потока
        FntPrint(text_stream, "Hello, PS1 World!\n");
        FntPrint(text_stream, "PSn00bSDK project works fine!\n");

        // Отправляем текст из буфера на экран (отрисовка поверх текущего кадра)
        FntFlush(text_stream);
        
        // Ждем вертикальной синхронизации луча (V-Blank), чтобы зафиксировать 60fps/50fps
        VSync(0);
    }

    return 0;
}
