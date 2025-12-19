from mininet.topo import Topo

class LeafSpine(Topo):
    def build(self, radix=4):

        # Number of spine and leaf switches decided by radix
        num_spines = radix // 2
        num_leaves = radix // 2

        spines = []
        leaves = []

        # Create spine switches
        for i in range(1, num_spines + 1):
            spine = self.addSwitch(f"spine{i}")
            spines.append(spine)

        # Create leaf switches
        for i in range(1, num_leaves + 1):
            leaf = self.addSwitch(f"leaf{i}")
            leaves.append(leaf)

        # Create full mesh between leaf and spine
        for spine in spines:
            for leaf in leaves:
                self.addLink(spine, leaf)

        # Attach 1 host to each leaf
        for i, leaf in enumerate(leaves, start=1):
            host = self.addHost(f"h{i}")
            self.addLink(host, leaf)


# Make this topology visible to Mininet
topos = { 'leafspine': LeafSpine }

