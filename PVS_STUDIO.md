# Running PVS Studio analysis

Download and install the PVS's command line tool [`how-to-use-pvs-studio-free`](https://github.com/viva64/how-to-use-pvs-studio-free) according its site instructions. After that, in the library root directory, perform the following commands:

```bash
how-to-use-pvs-studio-free -c 2 -m examples src test
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DSG_PVS_STUDIO=ON ..
make pvs_studio_analysis
```

The full PVS report will be generated in the `build/pvs_studio_fullhtml` directory.