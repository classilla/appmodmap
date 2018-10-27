# This script sets the Command ("Super") key to be equivalent to Control,
# and also sets the GNOME keybindings to that Super-Tab still switches.

#Uncomment these lines if you use xmodmap instead of setxkbmap.
#This is uncommon.
# xmodmap -e "remove mod4 = Super_L"
# xmodmap -e "add control = Super_L"

setxkbmap -option altwin:ctrl_win &

gsettings set org.gnome.desktop.wm.keybindings switch-applications "['<Ctrl>Tab', '<Alt>Tab']" &
gsettings set org.gnome.desktop.wm.keybindings switch-applications-backward "['<Shift><Ctrl>Tab', '<Shift><Alt>Tab']" &
gsettings set org.gnome.desktop.wm.keybindings switch-group-backward "['<Shift><Ctrl>Above_Tab', '<Shift><Alt>Above_Tab']" &
gsettings set org.gnome.desktop.wm.keybindings switch-group "['<Ctrl>Above_Tab', '<Alt>Above_Tab']" &

