# === Plugin-Grundkonfiguration ===
$pluginFolderName = "."                 # Plugin liegt im aktuellen Ordner
$zipFolderName = "PahoMQTT"            # Ordnername *im* ZIP

# === Version automatisch aus .uplugin lesen ===
$upluginFileName = "PahoMQTT.uplugin"
$upluginPath = Join-Path $pluginFolderName $upluginFileName
if (!(Test-Path $upluginPath)) {
    Write-Error ".uplugin file not found at $upluginPath"
    exit 1
}

$upluginContent = Get-Content $upluginPath -Raw | ConvertFrom-Json
$version = $upluginContent.VersionName
$zipName = "PahoMQTT-v$version.zip"
$targetZip = ".\$zipName"

# === Temporärer Arbeitsordner für ZIP-Erstellung ===
$tempZipDir = ".\temp_zip_build"

# === Vorherige Versionen bereinigen ===
if (Test-Path $targetZip) {
    Remove-Item $targetZip
}
if (Test-Path $tempZipDir) {
    Remove-Item $tempZipDir -Recurse -Force
}

# === Temporärstruktur anlegen ===
New-Item -ItemType Directory -Path "$tempZipDir\$zipFolderName" | Out-Null

# === Elemente definieren, die ins ZIP sollen ===
$itemsToInclude = @(
    "Binaries",
    "Content",
    "Resources",
    "ThirdPartyLicenses",
    $upluginFileName,
    "README.md",
    "LICENSE"
)

# === Elemente kopieren ===
foreach ($item in $itemsToInclude) {
    $sourcePath = Join-Path $pluginFolderName $item
    $destinationPath = Join-Path "$tempZipDir\$zipFolderName" $item
    if (Test-Path $sourcePath) {
        Copy-Item -Path $sourcePath -Destination $destinationPath -Recurse
    } else {
        Write-Warning "Skipping missing item: $sourcePath"
    }
}

# === ZIP erstellen ===
Compress-Archive -Path "$tempZipDir\*" -DestinationPath $targetZip

# === Aufräumen ===
Remove-Item $tempZipDir -Recurse -Force

Write-Host ""
Write-Host "Release archive created: $zipName"
