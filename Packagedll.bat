"C:\Program Files\MRE SDK V3.0.00\tools\DllPackage.exe" "D:\MyGitHub\Sttxt_v2\Sttxt_v2.vcproj"
if %errorlevel% == 0 (
 echo postbuild OK.
  copy Sttxt_v2.vpp ..\..\..\MoDIS_VC9\WIN32FS\DRIVE_E\Sttxt_v2.vpp /y
exit 0
)else (
echo postbuild error
  echo error code: %errorlevel%
  exit 1
)

