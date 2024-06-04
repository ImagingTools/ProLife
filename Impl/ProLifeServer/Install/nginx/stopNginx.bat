@echo on
.\nginx.exe -s quit
taskkill /f /IM nginx.exe
