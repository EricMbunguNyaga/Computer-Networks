from mininet.topo import Topo

class BinaryTreeTopo(Topo):
    def build(self):
        # Create switches
        s = {}
        for i in range(1, 8):
            s[i] = self.addSwitch(f's{i}')

        # Add binary links
        self.addLink(s[1], s[2])
        self.addLink(s[1], s[3])

        self.addLink(s[2], s[4])
        self.addLink(s[2], s[5])
        self.addLink(s[3], s[6])
        self.addLink(s[3], s[7])

        # Add 1 host per leaf switch
        for i in [4,5,6,7]:
            h = self.addHost(f'h{i}')
            self.addLink(h, s[i])

topos = { 'binarytree': (lambda: BinaryTreeTopo()) }

