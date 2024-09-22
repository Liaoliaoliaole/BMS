$projectDir = ".\BMSServerApp"  
$buildDir = "$projectDir\build"

if (-Not (Test-Path $buildDir)) {
    mkdir $buildDir
}

cd $buildDir

$cmakeResult = cmake -DCMAKE_TOOLCHAIN_FILE="../toolchain.cmake" -G "Ninja" ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed with exit code $LASTEXITCODE."
    exit $LASTEXITCODE
}

$ninjaResult = ninja
if ($LASTEXITCODE -ne 0) {
    Write-Host "Ninja build failed with exit code $LASTEXITCODE."
    exit $LASTEXITCODE
}

st-flash write "${buildDir}\stm32l152_nucleo.bin" 0x08000000
Write-Host "Flashed firmware to STM32L152RE."

cd ..
