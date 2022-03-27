# Лабораторная работа №2
## Operation Systems
Разработать комплекс программ на пользовательском уровне и уровне ярда, который собирает информацию на стороне ядра и передает информацию на уровень пользователя, и выводит ее в удобном для чтения человеком виде. Программа на уровне пользователя получает на вход аргумент(ы) командной строки (не адрес!), позволяющие идентифицировать из системных таблиц необходимый путь до целевой структуры, осуществляет передачу на уровень ядра, получает информацию из данной структуры и распечатывает структуру в стандартный вывод. Загружаемый модуль ядра принимает запрос через указанный в задании интерфейс, определяет путь до целевой структуры по переданному запросу и возвращает результат на уровень пользователя.
***
Интерфейс передачи между программой пользователя и ядром и целевая структура задается преподавателем. Интерфейс передачи может быть один из следующих:

`syscall` интерфейс системных вызовов.
ioctl - передача параметров через управляющий вызов к файлу/устройству.
procfs - файловая система /proc, передача параметров через запись в файл.
debugfs - отладочная файловая система /sys/kernel/debug, передача параметров через запись в файл.
Целевая структура может быть задана двумя способами:

Именем структуры в заголовочных файлах Linux
Файлом в каталоге /proc. В этом случае необходимо определить целевую структуру по пути файла в /proc и выводимым данным.
