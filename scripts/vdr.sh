#!/bin/sh

#unset LANG
#vdr -L . -L /usr/lib/vdr/plugins -P "xineliboutput --local=none --remote=none" 
#vdr -L . -P upnp -L /usr/lib/vdr/plugins -P "xineliboutput --local=none --remote=none" 
#vdr -L . -P upnp -L /usr/lib/vdr/plugins -P "xineliboutput --local=none --remote=none" -P streamdev-client
#~/work/vdr/vdr-1.4.4/vdr -v /var/lib/video.00/ -c /etc/vdr/ -L . -P upnp -P "xineliboutput --local=none --remote=none"
vdr -v /var/lib/video.00/ -c /etc/vdr/ -P streamdev-server -P upnp
