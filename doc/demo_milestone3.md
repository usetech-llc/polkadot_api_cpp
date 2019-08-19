# Manual Demos

Milestone 3 deliverables are located in branch [release/milestone3](https://github.com/usetech-llc/polkadot_api_cpp/tree/release/milestone3)

The project can be built with provided instructions on bare installation of Ubuntu Linux 16.04.

For convenience the Dockerfile is provided that installs all dependencies as needed, checks out the code for the API and builds it. In order to execute single API tests manually, please have docker CE installed, then clone this repository and run following commands (first one takes ~20 minutes to run for the first time):
```
$ docker build -t cppapi .
$ docker run -it --rm cppapi /bin/bash
# cd polkadot_api_cpp
```

There are no deliverables to execute, this milestone only delivers documentation and some binaries

## Deliverable 6 - Create Distribution Package

### API and example full documentation

The API documentation is provided in the repository in doc/html folder. The easiest way to access API documentation is to open Github HTML preview: https://htmlpreview.github.io/?https://github.com/usetech-llc/polkadot_api_cpp/blob/release/milestone2/doc/html/classIApplication.html

### Cleanup project files, ensure all tests pass

The codebase has been kept clean, so there isn't a particular commit to show, but there are a few examples of small cleanup jobs done through development process:

278cae6bfa
7b9c10e756
47974d6cf9
630464530e
7f80225564
5e46ae0ad6

### Create repository and publish to enable package manager (`apt-get`) installation

Discussed with Edward that binary repository does not make much sense if source code and binaries are published on Github.

### Create downloadable distribution installable with package manager

See release files attached to this release on Github

Also, here are instructions how to create the deb package:
```
cmake .
make
sudo make install
sudo chmod 777 install_manifest.txt
make package
```

### Create manually installable distribution (archive)

See release files attached to this release on Github

## Deliverable 7 - Port to Other Platforms

Below you can find links to building guides on other platforms

[Windows 10](https://github.com/usetech-llc/polkadot_api_cpp/blob/release/milestone3/doc/ports/windows.md)
[Manjaro, Arch Linux, OpenSuse, Debian](https://github.com/usetech-llc/polkadot_api_cpp/blob/release/milestone3/doc/ports/linuses.md)
[Mac OS](https://github.com/usetech-llc/polkadot_api_cpp/blob/release/milestone3/doc/ports/macos.md)
[Bonus goal: Raspberry Pi](https://github.com/usetech-llc/polkadot_api_cpp/blob/release/milestone3/doc/ports/raspberrypi.md)
