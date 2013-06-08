var address = "chat.freenode.com"
var port = 8001
var proxy = ""
var username = ""
var nick = ""
var pass = ""


function ircConnecting() {
        print("connecting to irc");
}

function ircConnected() {
        print("connected");
        for(var i=0;i<irc.channels.length;i++)
        {
                irc.join(irc.channels[i]);
        }
}


irc.channels = ["#bluemonkey","#linuxice"]
irc.connected.connect(ircConnected);
irc.connecting.connect(ircConnecting);

irc.connect(address,port,proxy,username,nick,pass);
