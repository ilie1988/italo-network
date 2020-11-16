[CmdletBinding()]
param ()
$web = New-Object System.Net.WebClient
if( -not ( Test-Path $env:APPDATA\.italonet -PathType Container ) )
{
  italonet.exe -g
}


$web.DownloadFile("https://seed.italonet.org/italonet.signed", "$env:APPDATA\.italonet\bootstrap.signed")
