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
```
xcode-select --install
xcode-select -p
```
```
/Applications/Xcode.app/Contents/Developer
```

## Mac Ports
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
cd synergy/

nano ext/toolchain/commands1.py
```
```
def configureCore(self, target="", extraArgs=""):
#       elif sys.platform == "darwin":
        if sys.platform == "darwin":
def getMacSdkDir(self):
#       return "/Developer/SDKs/" + sdkDirName + ".sdk"
        return "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk"
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
./hm.sh conf -g2
./hm.sh build
```
```
Build all projects

** BUILD SUCCEEDED **

sudo cp -v bin/Release/synergyc /opt/local/bin/
```

### Linux Server
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

### OSX Client
```
synergyc -f server-host-name
```
