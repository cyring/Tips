# System
## Terminal
```
nano /private/etc/nanorc
```
```
set nowrap
set const
set morespace
set nohelp
set nonewlines
```

```
nano .bash_profile
```
```
PATH=/opt/local/bin:/opt/local/libexec/qt5/bin:$PATH
PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033$
alias ll='ls -hlG'
alias la='ls -hAlG'
alias l='ls -CFG'
alias grep='grep --color=auto'
alias updatedb='sudo /usr/libexec/locate.updatedb'
```

```
chmod +x .bash_profile
```

## XQuartz
```
sudo nano /private/etc/sshd_config
X11Forwarding yes
```

## Xcode
* To build the Synergy GUI, download and install @ Apple Developers:  

1- Xcode 7.2  

* _for Yosemite_  

2- the Command Line Tools OS X 10.10 for Xcode 7.2  

* _for El Capitan_  

3- enter: ```xcode-select --install```  

* Check the path of the active developer directory  
```
xcode-select -p
```
```
/Applications/Xcode.app/Contents/Developer
```

## [Mac Ports](http://www.macports.org/install.php)
```
tar -zxvf MacPorts-2.3.4.tar.gz
cd MacPorts-2.3.4/
./configure
make
sudo make install
sudo port -v selfupdate
```

## Synergy
### Build
```
sudo port install cmake
sudo port install qt5

git clone https://github.com/synergy/synergy.git
```
* [Fix](https://github.com/synergy/synergy/pull/5140) GUI build
```
cd synergy/src/gui/src
nano CommandProcess.h
```
```
#include <QObject>
```
```
cd ..
qmake
cd ../..

# Fix the toolchain for OSX
nano ext/toolchain/commands1.py
```
```
def getMacSdkDir(self):
#       return "/Developer/SDKs/" + sdkDirName + ".sdk"
        return "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/" + sdkDirName + ".sdk"
```
```
# ./hm.sh setup
./hm.sh genlist
```
```
1: Unix Makefiles
2: Xcode
3: Eclipse CDT4 - Unix Makefiles
```
```
./hm.sh conf -g2 --mac-sdk 10.11
./hm.sh build
```
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
```
sudo cp -v bin/Release/synergyc /opt/local/bin/
```

### Server (Linux)
Follow these [instructions](https://wiki.archlinux.org/index.php/Synergy) to install the Synergy server on ArchLinux
```
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
```
synergys -f
```

### Client (OSX)
```
synergyc -f server-host-name
```

## Autostart
### Client
```
nano ~/Library/LaunchAgents/com.domain.synergy.plist
```
```
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
```
launchctl load ~/Library/LaunchAgents/com.domain.synergy.plist
```
### Server
## Solution #1 (SystemD)
```
systemctl enable synergys@<username>.socket     # replace with the unix user name
```
_Failed:_  
* The Synergy server terminates with a dumpcore because the <username> must be allowed to make connections to the X server.  

## Solution #2 (XDM)
```
sudo nano Xsetup
```
```
xhost +local:<username>
sudo -u <username> synergys --daemon

```
_Failed:_  
* Partly successful until the mouse leaves the master screen to enter the slave screen.  Might be due by the fact that XDM grabs the X keyboard.
