@echo off
REM Requires VCPKG_ROOT to point at your vcpkg checkout.
cmake -B build -S . -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake ^
  -DVCPKG_TARGET_TRIPLET=x64-windows-static ^
  -DCOPY_BUILD=OFF
cmake --build build --config Release --parallel
pause
