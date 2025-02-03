# Используем Windows Server Core
FROM mcr.microsoft.com/windows/servercore:ltsc2022

# Создаём рабочую папку
WORKDIR C:\app\

# Копируем все файлы за один раз
COPY D:\Temp\Qt6\ProLife\Tests\TestRunner.bat \
     D:\Temp\Qt6\Lisa\Bin\Release_Qt6_VC17_x64\ \
     D:\Temp\Qt6\ProLife\Bin\Release_Qt6_VC17_x64\ \
     D:\Temp\Qt6\Puma\Bin\Release_Qt6_VC17_x64\ \
     D:\Temp\Qt6\ProLife\Tests\postman_collection.json \
     D:\Temp\Qt6\ProLife\Tests\lisatest.backup \
     D:\Temp\Qt6\ProLife\Tests\pumatest.backup \
     D:\Temp\Qt6\ProLife\Tests\prolifetest.backup \
     C:\app\

# Запуск setup.bat
CMD ["cmd.exe", "/c", "C:\\app\\TestRunner.bat"]