# Optional parameter to enable TEST mode
param (
    [switch]$TestBuild
)

# Define variables
$projectDir = ".\BMSServerApp"  
$buildDir = "$projectDir\build"

# Check if build directory exists, if not, create it
if (-Not (Test-Path $buildDir)) {
    mkdir $buildDir
}

# Change directory to the build directory
cd $buildDir

# Construct the CMake command
$cmakeCommand = "cmake -DCMAKE_TOOLCHAIN_FILE=""../toolchain.cmake"" -G ""Ninja"" .."

# Add TEST flag if the parameter is provided
if ($TestBuild) {
    $cmakeCommand += " -DTEST=ON"
}

# Run CMake configuration
$cmakeResult = Invoke-Expression $cmakeCommand
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed with exit code $LASTEXITCODE."
    exit $LASTEXITCODE
}

# Run the Ninja build
$ninjaResult = ninja
if ($LASTEXITCODE -ne 0) {
    Write-Host "Ninja build failed with exit code $LASTEXITCODE."
    exit $LASTEXITCODE
}

# Return to the project directory
cd ..