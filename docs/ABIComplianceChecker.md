# Checking backward API/ABI compatibility of Sagui library versions

**Configure the build:**

```bash
export SG_OLD_VER="3.0.0" # Change to old release
export SG_NEW_VER="3.1.0" # Change to new release

curl -SL https://github.com/risoflora/libsagui/archive/v${SG_OLD_VER}.tar.gz | \
  tar -zx
cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSG_HTTPS_SUPPORT=ON \
  -DSG_ABI_COMPLIANCE_CHECKER=ON \
  -DSG_OLD_LIB_DIR=$(pwd)/libsagui-${SG_OLD_VER} \
  -DSG_OLD_LIB_VERSION=${SG_OLD_VER} ..
make abi_compliance_checker
```

**Check the generated HTML:**

```bash
xdg-open compat_reports/sagui/${SG_OLD_VER}_to_${SG_NEW_VER}/compat_report.html

# or just open the "compat_report.html" on your browser
```

**NOTE:** the `Binary Compatibility` and `Source Compatibility` tabs should
show `Compatibility: 100%` in their `Test Results` table.
