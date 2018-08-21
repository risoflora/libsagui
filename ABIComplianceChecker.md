# Checking backward API/ABI compatibility of Sagui library versions

**Configure the build:**

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON -DSG_HTTPS_SUPPORT=ON -DSG_ABI_COMPLIANCE_CHECKER=ON -DSG_OLD_LIB_DIR=/home/USER/libsagui-1.0.0 -DSG_OLD_LIB_VERSION=1.0.0 ..
make abi_compliance_checker
```

**Check the generated HTML:**

```bash
xdg-open compat_reports/sagui/1.0.0_to_1.0.1/compat_report.html

# or just open compat_report.html on your browser
```

**NOTE:** the `Binary Compatibility` and `Source Compatibility` tabs should show `Compatibility: 100%` in their `Test Results` table.