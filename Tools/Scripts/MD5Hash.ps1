# IMPORTANT! Before asking stupid questions, read information below!
# Usage:
# Write your salt (auth.db.salt value from auth.opt) in $salt variable
# Save script
# Use script: Right mouse button click to script file -> Run via PowerShell
# If it throws an error like scripts execution is disabled, or instacloses, just google how to enable scripts
# OR just copy-paste it in powershell and press enter. That's it

$salt		= "kitekatsalt"
$someString = "admin"
$fullPass	= $salt + $someString
$md5		= new-object -TypeName System.Security.Cryptography.MD5CryptoServiceProvider
$utf8		= new-object -TypeName System.Text.UTF8Encoding
$hash		= ([System.BitConverter]::ToString($md5.ComputeHash($utf8.GetBytes($someString)))).ToLower() -replace '-', ''
echo $hash

Pause