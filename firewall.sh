#!/bin/bash
cd
echo "Are you sure you want to change Firewall settings?"
read -p "Continue? (Y/N): " confirm && [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]] || exit 1

echo reseting iptables rules  :: :: :: :: !
if
# restore rules to default
echo reseting rules to default
iptables-restore < /etc/iptables/empty.rules
then

# try connection
echo trying network connection with default  :: :: :: :: !
ping -c 3 archlinux.org

#create neccesary chains
iptables -N TCP 
iptables -N UDP

# forwarding chain
# drop forwarding
echo droping forwarding chain  :: :: :: :: !
iptables -P FORWARD DROP

# outgoing chain
# accept outgoing
echo acceping output  :: :: :: :: !
iptables -P OUTPUT ACCEPT

# the input chain
# drop outgoing
echo droping input chain  :: :: :: :: !
iptables -P INPUT DROP

# try connection
echo trying network after droping input chain  :: :: :: :: !
ping -c 3 archlinux.org

# exeptions to input rule
echo adding exceptions to input chain rule  :: :: :: :: !
if
iptables -A INPUT -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT
then
echo related and established are accepted  :: :: :: :: !
fi
# accept traffic from loopback(necesary for printers)
if
iptables -A INPUT -i lo -j ACCEPT
then
echo loopback accepted  :: :: :: :: !
fi

# drop invalid connections
if
iptables -A INPUT -m conntrack --ctstate INVALID -j DROP
then
echo rules for invalid connections set  :: :: :: :: !
fi

# next rule is for accepting a ping (do I need it? for a personal PC)
# iptables -A INPUT -p icmp --icmp-type 8 -m conntrack --ctstate NEW -j ACCEPT

# attach TCP and UDP chains to INPUT chain to handle all incoming connections
# Once accepted by TCP or UDP, it is handled by RELATED/ESTABLISHED traffic rule
echo attaching udp and tcp chains to input  :: :: :: :: !
iptables -A INPUT -p udp -m conntrack --ctstate NEW -j UDP
iptables -A INPUT -p tcp --syn -m conntrack --ctstate NEW -j TCP

# reject TCP connections with TCP RESET packets & UDP streams
echo rejecting icmp inputs  :: :: :: :: !
iptables -A INPUT -p udp -j REJECT --reject-with icmp-port-unreachable
iptables -A INPUT -p tcp -j REJECT --reject-with tcp-reset
iptables -A INPUT -j REJECT --reject-with icmp-proto-unreachable

# save rules 
echo saving rules to /etc/iptables/iptables.rules :: :: :: :: !
iptables-save -f /etc/iptables/iptables.rules

# To accept incoming TCP connections on port 80 for a web server
# iptables -A TCP -p tcp --dport 80 -j ACCEPT

#To accept incoming TCP connections on port 443 for a web server (HTTPS)
# iptables -A TCP -p tcp --dport 443 -j ACCEPT

# ssh on port 22
# iptables -A TCP -p tcp --dport 22 -j ACCEPT

echo enabling and starting iptables :: :: :: :: !
systemctl enable iptables && systemctl start iptables
systemctl status iptables
echo DONE :: :: :: !!!!
else
echo iptables not found, please install it and run this script again.
fi
# now how about ipv6 heh? next time


