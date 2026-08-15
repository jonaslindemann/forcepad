<#
.SYNOPSIS
    Creates (once) a self-signed code-signing certificate for locally testing
    the ForcePAD MSIX package, and exports it as a .pfx for signtool.

.DESCRIPTION
    You do NOT need this - or any purchased certificate - to publish through
    the Microsoft Store: Store ingestion re-signs the uploaded .msix with
    Microsoft's own certificate. This script exists purely so the package can
    be installed and tested on this machine before submitting it, and for the
    `sideload` distribution channel if you ever want to hand the .msix to
    someone directly without going through the Store.

    The certificate subject must match the Publisher attribute in
    AppxManifest.xml character for character, or Windows refuses the install
    with 0x800B0100 / "publisher of the package does not match".

    Invoked automatically by the qtforcepad_msix target on the `dev` channel;
    can also be run by hand.

.EXAMPLE
    scripts\msix-devcert.ps1 -Subject "CN=ForcePAD Development" `
                             -PfxPath build-release\forcepad-dev.pfx
#>
[CmdletBinding()]
param(
    # Must match AppxManifest.xml's Publisher exactly.
    [Parameter(Mandatory = $true)][string]$Subject,
    [Parameter(Mandatory = $true)][string]$PfxPath,
    [string]$Password = 'forcepad',
    # Recreate even if $PfxPath already exists.
    [switch]$Force
)

$ErrorActionPreference = 'Stop'

# Idempotent by design: the CMake target runs this on every msix build, so a
# no-op fast path matters more than being clever.
#
# The fast path deliberately checks the *subject*, not just the file's
# existence. Changing FORCEPAD_MSIX_PUBLISHER (as happens the first time the
# real Partner Center identity is configured) otherwise leaves a stale .pfx in
# place, and the package then signs happily but refuses to install with
# 0x800B0100 - a failure that gives no hint about where it came from.
if ((Test-Path -LiteralPath $PfxPath) -and -not $Force) {
    try {
        $existing = [System.Security.Cryptography.X509Certificates.X509Certificate2]::new(
            (Resolve-Path -LiteralPath $PfxPath).Path, $Password)
        if ($existing.Subject -eq $Subject -and $existing.NotAfter -gt (Get-Date)) {
            Write-Host "Development certificate already present: $PfxPath"
            exit 0
        }
        Write-Host "Replacing $PfxPath - it is for '$($existing.Subject)', not '$Subject'."
    }
    catch {
        Write-Host "Replacing $PfxPath - it could not be read ($($_.Exception.Message))."
    }
}

$parent = Split-Path -Parent $PfxPath
if ($parent -and -not (Test-Path -LiteralPath $parent)) {
    New-Item -ItemType Directory -Path $parent | Out-Null
}

# Reuse an existing certificate with this subject if there is one, so that
# deleting the .pfx does not silently orphan a previously trusted cert.
$cert = Get-ChildItem -Path 'Cert:\CurrentUser\My' |
    Where-Object { $_.Subject -eq $Subject -and $_.NotAfter -gt (Get-Date) } |
    Sort-Object NotAfter -Descending |
    Select-Object -First 1

if ($null -eq $cert) {
    Write-Host "Creating self-signed code-signing certificate for $Subject"
    # -TextExtension: EKU 1.3.6.1.5.5.7.3.3 (code signing) plus an empty basic
    # constraints extension. This is the combination MSIX signing requires;
    # a plain -Type CodeSigningCert certificate is not accepted by signtool
    # for appx/msix packages.
    $cert = New-SelfSignedCertificate `
        -Type Custom `
        -Subject $Subject `
        -KeyUsage DigitalSignature `
        -FriendlyName 'ForcePAD MSIX development' `
        -CertStoreLocation 'Cert:\CurrentUser\My' `
        -NotAfter (Get-Date).AddYears(3) `
        -TextExtension @('2.5.29.37={text}1.3.6.1.5.5.7.3.3', '2.5.29.19={text}')
}
else {
    Write-Host "Reusing existing certificate $($cert.Thumbprint) for $Subject"
}

$securePassword = ConvertTo-SecureString -String $Password -Force -AsPlainText
Export-PfxCertificate -Cert $cert -FilePath $PfxPath -Password $securePassword | Out-Null
Write-Host "Exported $PfxPath (password: $Password)"

# Windows will not install a package signed by a certificate it does not
# trust. Importing into LocalMachine\TrustedPeople needs elevation, so treat
# failure as advisory rather than fatal - the .pfx is still usable for signing.
try {
    Import-PfxCertificate -FilePath $PfxPath `
        -CertStoreLocation 'Cert:\LocalMachine\TrustedPeople' `
        -Password $securePassword | Out-Null
    Write-Host 'Certificate trusted in LocalMachine\TrustedPeople.'
}
catch {
    Write-Warning @"
Could not import the certificate into LocalMachine\TrustedPeople (this needs
an elevated shell). The package will build and sign, but Add-AppxPackage will
reject it until you run, from an *Administrator* PowerShell:

    Import-PfxCertificate -FilePath '$((Resolve-Path $PfxPath).Path)' ``
        -CertStoreLocation Cert:\LocalMachine\TrustedPeople ``
        -Password (ConvertTo-SecureString -String '$Password' -Force -AsPlainText)
"@
}
