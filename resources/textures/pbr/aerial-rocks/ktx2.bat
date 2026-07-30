@echo off
setlocal enabledelayedexpansion

REM ============================================
REM PBR Batch Converter for KTX2 (UASTC + ETC1S)
REM ============================================

set INPUT=./raw
set OUTPUT=./ktx2

if not exist %OUTPUT% mkdir %OUTPUT%

echo Converting PBR textures from %INPUT% to %OUTPUT%
echo.

for %%f in (%INPUT%\*) do (

    set FILE=%%~nf
    set EXT=%%~xf

    REM -----------------------------------------
    REM ALBEDO (color)
    REM -----------------------------------------
    echo !FILE! | findstr /i "_alb _albedo _diff _basecolor" >nul
    if !errorlevel! == 0 (
        echo [ALBEDO] %%f
        toktx --bcmp --assign_oetf srgb --zcmp 18 ^
            "%OUTPUT%\!FILE!.ktx2" "%%f"
        echo.
        REM continue to next file
        ) else (

    REM -----------------------------------------
    REM NORMAL
    REM -----------------------------------------
    echo !FILE! | findstr /i "_nor _normal" >nul
    if !errorlevel! == 0 (
        echo [NORMAL] %%f
        toktx --uastc --uastc_quality 2 --assign_oetf linear --zcmp 18 ^
            "%OUTPUT%\!FILE!.ktx2" "%%f"
        echo.
        ) else (

	REM -----------------------------------------
    REM AO
    REM -----------------------------------------
    echo !FILE! | findstr /i "_ao _ambient" >nul
    if !errorlevel! == 0 (
        echo [AO] %%f
        toktx --uastc --uastc_quality 2 --assign_oetf linear --zcmp 18 ^
            "%OUTPUT%\!FILE!.ktx2" "%%f"
        echo.
        ) else (
	
	REM -----------------------------------------
    REM roughness
    REM -----------------------------------------
    echo !FILE! | findstr /i "_rough" >nul
    if !errorlevel! == 0 (
        echo [ROUGHNESS] %%f
        toktx --uastc --uastc_quality 2 --assign_oetf linear --zcmp 18 ^
            "%OUTPUT%\!FILE!.ktx2" "%%f"
        echo.
        ) else (
		
	REM -----------------------------------------
    REM metal
    REM -----------------------------------------
    echo !FILE! | findstr /i "_metal" >nul
    if !errorlevel! == 0 (
        echo [METAL] %%f
        toktx --uastc --uastc_quality 2 --assign_oetf linear --zcmp 18 ^
            "%OUTPUT%\!FILE!.ktx2" "%%f"
        echo.
        ) else (
		
    REM -----------------------------------------
    REM ARM (occlusion/roughness/metallic)
    REM -----------------------------------------
    echo !FILE! | findstr /i "_orm _rma _arm" >nul
    if !errorlevel! == 0 (
        echo [ARM] %%f
        toktx --uastc --uastc_quality 2 --assign_oetf linear --zcmp 18 ^
            "%OUTPUT%\!FILE!.ktx2" "%%f"
        echo.
        ) else (

    REM -----------------------------------------
    REM HEIGHT / DISPLACEMENT
    REM -----------------------------------------
    echo !FILE! | findstr /i "_height _disp _displacement" >nul
    if !errorlevel! == 0 (
        echo [HEIGHT] %%f
        toktx --uastc --uastc_quality 2 --assign_oetf linear --zcmp 18 ^
            "%OUTPUT%\!FILE!.ktx2" "%%f"
        echo.
        ) else (

    REM -----------------------------------------
    REM UNKNOWN → skip
    REM -----------------------------------------
        echo [SKIP] %%f
        echo.
    )))))))
)

echo Done.
pause
