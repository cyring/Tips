
# Synergy
## Requirements
1. [Arch Linux](https://www.archlinux.org)
2. [Xcode](OSX.md#xcode)
3. [Mac Ports](OSX.md#mac-ports)
3. [Synergy](https://github.com/synergy/synergy)

## Build
* Install the required libraries.  
```sh
sudo port install cmake
sudo port install qt5
```
* Add the path of the libraries.  
```sh
nano .bash_profile
```
```
PATH=/opt/local/libexec/qt5/bin:$PATH
```
* Get the Synergy source code.  
```sh
git clone https://github.com/synergy/synergy.git
```
* [Fix](https://github.com/synergy/synergy/pull/5140) the GUI compilation.  
```sh
cd synergy/src/gui/src
nano CommandProcess.h
```
```C
#include <QObject>
```
```sh
cd ..
qmake
cd ../..
```
* Take note of the path of the active developer directory.  
```
xcode-select -p
```
```
/Applications/Xcode.app/Contents/Developer
```
* Fix the toolchain to handle the OS X version.  
```sh
nano ext/toolchain/commands1.py
```
```Python
def getMacSdkDir(self):
#       return "/Developer/SDKs/" + sdkDirName + ".sdk"
        return "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/" + sdkDirName + ".sdk"
```
```
# ./hm.sh setup
./hm.sh genlist
```
* Check for the supported compilers.  
```
1: Unix Makefiles
2: Xcode
3: Eclipse CDT4 - Unix Makefiles
```
*  Compile the sources, specifying the OS X version.  
```sh
./hm.sh conf -g2 --mac-sdk 10.11
./hm.sh build
```
* Check for a successful build.  
```
Entering dir: build
=== BUILD AGGREGATE TARGET ZERO_CHECK OF PROJECT synergy WITH CONFIGURATION Release ===

Check dependencies
...
Build all projects

** BUILD SUCCEEDED **
...
Make GUI command: make -w
Entering dir: src/gui
...
Copying plugins dirtree: bin/Release/plugins
Copying to: bin/Release/Synergy.app/Contents/MacOS/plugins
Entering dir: bin/Release
...
bin/Release/Synergy.app
```
* Copy the Client program into one of the binaries paths.  
```sh
sudo cp -v bin/Release/synergyc /opt/local/bin/
```
* Copy the Server program (if you want OS X to act as)
```sh
sudo cp -v bin/Release/synergys /opt/local/bin/
```

## Server (Linux)
* Follow these [instructions](https://wiki.archlinux.org/index.php/Synergy) to install the Synergy server from the Arch Linux Repository.  
```sh
yaourt -S synergy-git
```
* Configure the computers disposal.  
```sh
sudo nano /etc/synergy.conf
```
```
section: screens
        client-host-name:
        server-host-name:
end

section: links
        client-host-name:
                right = server-host-name

        server-host-name:
                left  = client-host-name
end

section: aliases
        client-host-name:
                client-host-name.some-domain
end
```
* Start the server.  
```sh
synergys -f
```

## Client (OSX)
* Start the client.
```sh
synergyc -f server-host-name
```

## Autostart
### Client
* Configure a plist file to auto start the Synergy client whenever the user starts a session.  
```sh
nano ~/Library/LaunchAgents/com.domain.synergy.plist
```
```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>Label</key>
		<string>com.domain.synergy</string>
	<key>ProgramArguments</key>
		<array>
			<string>/opt/local/bin/synergyc</string>
			<string>-f</string>
			<string>-d</string>
			<string>WARNING</string>
			<string>server-host-name</string>
		</array>
	<key>KeepAlive</key>
		<true/>
</dict>
</plist>
```
* Make the agent active.  
```sh
launchctl load ~/Library/LaunchAgents/com.domain.synergy.plist
```
### Server
#### Solution #1 (SystemD)
```sh
systemctl enable synergys@<username>.socket     # replace with the unix user name
```
* _Failed_ : The Synergy server is terminated with a core dump because the ```<username>``` must be allowed to make connections to the X server.  

#### Solution #2 (XDM)
```sh
sudo nano Xsetup
```
```
xhost +local:<username>
sudo -u <username> synergys --daemon

```
* _Partly successful_ : until the mouse leaves the master screen to enter the slave screen.  Might be due by the fact that XDM grabs the X keyboard.  

#### Solution #3 (X Session)
```sh
sudo nano Xstartup
```
```sh
#!/bin/sh
pidof synergys
declare -i STATE=$?
if (( $STATE == 1 )); then
    xhost +local:${USER} > /dev/null 2>&1
    declare -i RC=$?
        if (( $RC == 0 )); then
           sudo -u ${USER} synergys -d WARNING --daemon
        fi
fi
```
```sh
sudo nano Xreset
```
```sh
killall synergys > /dev/null 2>&1
```
* _Successful_ : this solution allows an early startup of the Synergy server, right before the user starts a X session, and die when the X session stops. However, it won't let the Synergy server run during the XDM chooser.  
* An Arch Linux package is available in the [AUR](https://aur.archlinux.org/packages/xdm-xfreq)  
