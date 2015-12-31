Terminal
```
nano /private/etc/nanorc
set nowrap
set const
set morespace
set nohelp
set nonewlines


nano .bash_profile
PATH=/opt/local/bin:$PATH
PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033$
alias ll='ls -hlG'
alias la='ls -hAlG'
alias l='ls -CFG'
alias grep='grep --color=auto'
alias updatedb='sudo /usr/libexec/locate.updatedb'

```

X-Window
```
sudo nano /private/etc/sshd_config
X11Forwarding yes
```

Xcode
```
xcode-select --install
xcode-select -p
```

Mac Ports
```
tar -zxvf MacPorts-2.3.4.tar.gz
cd MacPorts-2.3.4/
./configure
make
sudo make install
sudo port -v selfupdate

```

Synergy
```
sudo port install cmake
sudo port install qt5

git clone https://github.com/synergy/synergy.git
cd synergy/

nano ext/toolchain/commands1.py
def configureCore(self, target="", extraArgs=""):
#       elif sys.platform == "darwin":
        if sys.platform == "darwin":
def getMacSdkDir(self):
#       return "/Developer/SDKs/" + sdkDirName + ".sdk"
        return "/Applications/Xcode.app/Contents/Developer/" + sdkDirName + ".sdk"
```
