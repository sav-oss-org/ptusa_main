param(
    [switch]$Installer
)

$ErrorActionPreference = "Stop"
$ProjectDirectory = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $ProjectDirectory
try {
    uv sync --frozen --group dev
    uv run pyinstaller --noconfirm --clean PtusaLuaDebugger.spec

    if ($Installer) {
        $IsccCommand = Get-Command ISCC.exe -ErrorAction SilentlyContinue
        $IsccPath = if ($IsccCommand) { $IsccCommand.Source } else { $null }
        if (-not $IsccPath) {
            $DefaultIscc = "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
            if (Test-Path -LiteralPath $DefaultIscc) {
                $IsccPath = $DefaultIscc
            }
        }
        if (-not $IsccPath) {
            throw "Inno Setup 6 (ISCC.exe) не найден."
        }
        & $IsccPath installer.iss
    }
}
finally {
    Pop-Location
}
