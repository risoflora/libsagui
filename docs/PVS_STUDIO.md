# Running PVS Studio analysis

Download and install the [PVS-Studio Analyzer](https://www.viva64.com/en/pvs-studio/)
according its site instructions. After that, in the library root directory,
perform the following commands:

```bash
mkdir build && cd build/
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DSG_PVS_STUDIO=ON ..
make pvs_studio_analysis
```

The full PVS report will be generated in the `build/pvs_studio_fullhtml` directory.
