$exe = "C:\projects\tool\x64\release\tool.exe"
$bcomp = "c:\Program Files\Beyond Compare 4\bcomp.com"
cd c:\tmp

function Test($src)
{
    &$exe -i $src -o 1.out -c
    &$exe -i $src -o 2.out -c -m 1
    &$bcomp .\1.out .\2.out -qc=binary
    $LastExitCode -eq 1
}

$same = Test .\ebat.csv
if (!$same)
{
    $left = 
}