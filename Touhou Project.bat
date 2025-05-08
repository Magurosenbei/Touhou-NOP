del /q *.ncb
del /a:h /q *.suo
del /a:h /q *.user
del /s /q *.user
del /s /q *.pdb
del /s /q *.ilk
del /s /q *.exe
for /d %%G in ("*") do rmdir /s /q "%%G/Debug/"
for /d %%G in ("*") do rmdir /s /q "%%G/Release/"