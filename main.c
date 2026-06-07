#include <psxapi.h>
#include <psxgpu.h>
#include <psxetc.h>

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// Массивы из 2 элементов для честной двойной буферизации
static DISPENV dispEnv[2];
static DRAWENV drawEnv[2];
static int db = 0;

void init_graphics(void) {
    // Настраиваем первый буфер (индекс 0) на координаты (0, 0)
    SetDefDispEnv(&dispEnv[0], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDrawEnv(&drawEnv[0], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Настраиваем второй буфер (индекс 1) ниже — на координаты (0, 256)
    SetDefDispEnv(&dispEnv[1], 0, 256, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDrawEnv(&drawEnv[1], 0, 256, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Включаем очистку заднего фона черным цветом (0,0,0) строго внутри DRAWENV
    drawEnv[0].isbg = 1;
    setRGB0(&drawEnv[0], 0, 0, 0);
    
    drawEnv[1].isbg = 1;
    setRGB0(&drawEnv[1], 0, 0, 0);
    
    // Применяем настройки начального кадра
    PutDispEnv(&dispEnv[0]);
    PutDrawEnv(&drawEnv[0]);
    
    // Включаем отображение дисплея на экран
    SetDispMask(1);
}

int main(void) {

    // 2. Сбрасываем видеочип и размечаем экранную область
    ResetGraph(0);
    init_graphics();
    
    // 3. Загружаем текстуру встроенного шрифта BIOS в VRAM (координаты 960, 0)
    FntLoad(960, 0);
    
    // 4. Открываем текстовый поток (X, Y, Ширина, Высота, Автоочистка фона, Макс. символов)
    int text_stream = FntOpen(16, 16, SCREEN_WIDTH - 32, SCREEN_HEIGHT - 32, 0, 512);

    // Главный игровой цикл
    while (1) {
        // Переключаем активный буфер кадра (0 или 1)
        db = !db;
        PutDispEnv(&dispEnv[db]);
        PutDrawEnv(&drawEnv[db]);

        // Записываем форматированный текст в буфер
        FntPrint(text_stream, "HELLO, PS1 WORLD!\n");
        FntPrint(text_stream, "PSN00BSDK WORKS BOTH IN EXE AND CUE!\n");

        // Отправляем накопленный текст на отрисовку в GPU
        FntFlush(text_stream);
        
        // Ожидаем вертикального прерывания луча (V-Blank) для фиксации кадров
        VSync(0);
    }

    return 0;
}
