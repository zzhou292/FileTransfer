@echo on
echo Process begins
call "%CONDA_INSTALL_LOCN%"\Scripts\activate.bat
call conda install --yes conda-build anaconda-client
call conda install --yes -c intel mkl-devel
call conda install -c conda-forge swig=4.0 --yes
call conda install -c dlr-sc opencascade --yes
call conda install mkl --yes
call conda install cmake --yes
call conda install jinja2 --yes
call conda install ninja --yes
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
Rem CMAKE config output is redirected to a file otherwise it gets truncated due to depth
call conda build purge-all timeout /t 240
call conda build .\contrib\packaging-python\conda --python=3.6 --no-remove-work-dir --dirty
call anaconda --token "%ANACONDA_TOKEN%" upload "%CONDA_INSTALL_LOCN%"\conda-bld\win-64\pychrono*.bz2 --force --label develop  >> "%LOG_DIR%"\condauploadlog.txt 2>&1
call conda build purge-all  timeout /t 240
call conda build .\contrib\packaging-python\conda --python=3.7 --no-remove-work-dir --dirty
call anaconda --token "%ANACONDA_TOKEN%" upload "%CONDA_INSTALL_LOCN%"\conda-bld\win-64\pychrono*.bz2 --force --label develop  >> "%LOG_DIR%"\condauploadlog.txt 2>&1
Rem call conda build purge  timeout /t 240
Rem call conda build .\contrib\packaging-python\conda --python=3.8 --no-remove-work-dir --dirty
Rem call anaconda --token "%ANACONDA_TOKEN%" upload "%CONDA_INSTALL_LOCN%"\conda-bld\win-64\pychrono*.bz2 --force --label develop  >> "%LOG_DIR%"\condauploadlog.txt 2>&1
echo End Reached