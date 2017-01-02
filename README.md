# ThirdPartyInstallers

This is a collection of utilities to handle the installation of packages in non-native formats such as RPM and DEB.

Packages are unpacked under `/Programs`. The contents of `/opt`, when present, are flattened under the new program entry
and compatibility symlinks are created at /opt pointing back to the new entry at `/Programs`.

# Caveats

Note that running packages built for other distributions is a best-effort task. There are many features that won't
work out of the box and that are outside the scope of `ThirdPartyInstallers`. For instance, init scripts, tasks files,
and reliance on other distro-specific components may preclude the installed package from working as expected.

ThirdPartyInstallers should help you to install distro-agnostic packages such as LibreOffice, self-contained packages and packages that do have many fancy dependencies. It will **not** work with complex packages such as KDE.

If there is a GoboLinux recipe to build a program for which you have an RPM/DEB file, please use the Compile tool instead of the ThirdPartyInstallers utilities. We provide **zero** support for third-party packages.

# Usage

Installing a single RPM file:

`$ InstallPackage-RPM libfoo-2.1-3.x86_64.rpm`

Merging multiple RPMs under a single entry at `/Programs`:

`$ InstallPackage-RPM --app-name "LibreOffice" --version-number 5.2.3.3 *.rpm`
