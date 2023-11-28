# OS

## Arch

### Programming

<em> Stay on VS Code</em>

The Aur version of VS-Code, can be cloned or 

```console
$ yay -S visual-studio-code-bin
```
#### Python

Installing extensions for python is enough without further hussle

#### C / C++

Check if gcc, g++, and gdb are installed.

```console
# check versions

$ gcc -v
$ g++ -v
$ gdb -v
```
gdb won't probably be installed, so install it with

```console
# dont know if gdb-common is needed. try without it next time
sudo pacman -S gdb gdb-common
```
and then in VS-Code install the extension for C/C++ an done!