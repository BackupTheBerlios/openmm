# Simple UDP Multicast Server example
# Kyle Robertson
# A Few Screws Loose, LLC
# http://www.afslgames.com
# ra1n@gmx.net
# MulticastServer.py

from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor
from twisted.application.internet import MulticastServer

class MulticastServerUDP(DatagramProtocol):
    def startProtocol(self):
        print 'Started Listening'
        # Join a specific multicast group, which is the IP we will respond to
        self.transport.joinGroup('239.255.255.250')

    def datagramReceived(self, datagram, address):
        # The uniqueID check is to ensure we only service requests from ourselves
        print "Server Received:" + repr(datagram)

# Note that the join function is picky about having a unique object
# on which to call join.  To avoid using startProtocol, the following is
# sufficient:
#reactor.listenMulticast(8005, MulticastServerUDP()).join('224.0.0.1')

# Listen for multicast on 224.0.0.1:8005
reactor.listenMulticast(1900, MulticastServerUDP())
reactor.run()

