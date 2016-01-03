# Mac OS X
## Terminal
### Nano
* Improve the editor area.  
```sh
sudo nano /private/etc/nanorc
```
```
set nowrap
set const
set morespace
set nohelp
set nonewlines
```
### Bourne-Again SHell
* Add the path of the [Mac Ports](OSX.md#mac-ports).  
```sh
nano .bash_profile
```
```
PATH=/opt/local/bin:$PATH

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
* Enable X11 forwarding.  
```sh
sudo nano /private/etc/sshd_config
```
```
X11Forwarding yes
```
* Enable the SSH daemon in ```System Preferences > Sharing > Remote Login```
* To log in to this computer remotely, type "ssh username@hostname".

### SSH
* From the remote system, start a ssh session with X-Window capabilities  
```sh
ssh -Y username@hostname
```
* Remark : _verify_ and accept the proposed key.  

## Xcode
1- Xcode 7.2  
* Search for Xcode : ```App store > Store > Search```  
* Or download and install @ Apple Developers.  

2- _Yosemite_  
* Get the Command Line Tools OS X 10.10 for Xcode 7.2  

3- _El Capitan_  
* Enter: ```xcode-select --install```  

## Mac Ports
* Build [Mac Ports](http://www.macports.org/install.php) from its source code.  
```sh
tar -zxvf MacPorts-2.3.4.tar.gz
cd MacPorts-2.3.4/
./configure
make
sudo make install
sudo port -v selfupdate
```
