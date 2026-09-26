# 1) Install MagickIMage https://imagemagick.org/script/download.php
# 2) Change $directory variable to directoy where your images located at
# 3) Save script
# 4) Right mouse button to script file -> Run with PowerShell
# 5) Wait until script would finish

$directory = "D:\Grimoire_v4122\Output\jpg"
$outputFile = "02_item.spr"


New-Item -Path $outputFile -ItemType File -Force

function IsImage($file) {
    $validExtensions = @(".png", ".jpg", ".jpeg", ".tga")
    $ext = [System.IO.Path]::GetExtension($file).ToLower()
    return $validExtensions -contains $ext
}

Get-ChildItem -Path $directory | ForEach-Object {
    $filePath = $_.FullName

    if ($_.PSIsContainer -or $_.Length -eq 0) {
        return
    }

    if (IsImage $filePath) {
        try {
            $identifyOutput = & magick identify -format "%wx%h" $filePath

            if ($identifyOutput -match "(\d+)x(\d+)") {
                $width = $matches[1]
                $height = $matches[2]
                
				if (!($width -eq 20 -and $height -eq 20) -and !($width -eq 34 -and $height -eq 34)) {
					return
				}

                $spriteName = [System.IO.Path]::GetFileNameWithoutExtension($_.Name)

                $row = "$spriteName`n1`n$($_.Name)`n$width,$height`n0"

                Add-Content -Path $outputFile -Value $row
            }
            else {
                Write-Warning "Failed to get xy for: $($_.Name)"
            }
        }
        catch {
            Write-Warning "Failed to process image: $($_.Name) - $_"
        }
    }
    else {
        Write-Warning "Skipping non-image file: $($_.Name)"
    }
}
