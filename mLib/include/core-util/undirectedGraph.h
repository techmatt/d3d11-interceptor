
#ifndef CORE_UTIL_UNDIRECTEDGRAPH_H_
#define CORE_UTIL_UNDIRECTEDGRAPH_H_

namespace ml {

template<class NodeData, class EdgeData>
class UndirectedGraph
{
public:
    struct Edge
    {
        UINT getNeighbor(UINT nodeIndex) const
        {
            if (node0 == nodeIndex) return node1;
            return node0;
        }

        UINT node0, node1;
        EdgeData data;
    };

    struct Node
    {
        UINT index;
        NodeData data;
        vector<UINT> edges;
    };

    const vector<Node>& nodes() const
    {
        return _nodes;
    }

    vector<Node>& nodes()
    {
        return _nodes;
    }

    const vector<Edge>& edges() const
    {
        return _edges;
    }

    vector<Edge>& edges()
    {
        return _edges;
    }

    void addNode(const NodeData &data)
    {
        Node newNode;
        newNode.index = (UINT)_nodes.size();
        newNode.data = data;
        _nodes.push_back(newNode);
    }

    void addEdge(int node0, int node1, const EdgeData &data = EdgeData())
    {
        if (node0 > node1) std::swap(node0, node1);

        Edge newEdge;
        newEdge.node0 = node0;
        newEdge.node1 = node1;
        newEdge.data = data;

        _nodes[node0].edges.push_back((UINT)_edges.size());
        _nodes[node1].edges.push_back((UINT)_edges.size());

        _edges.push_back(newEdge);
    }

    vector< vector<const Node*> > computeConnectedComponents() const
    {
        return computeConnectedComponents([](const EdgeData &e) { return true; });
    }

    vector< vector<const Node*> > computeConnectedComponents(const std::function<bool(const EdgeData&)> &edgeTest) const
    {
        const UINT n = (UINT)_nodes.size();
        set<UINT> remainingNodes;

        for (UINT i = 0; i < n; i++)
            remainingNodes.insert(i);

        std::function<void(UINT, vector<const Node*>&)> visitNode = [&](UINT nodeIndex, vector<const Node*> &curComponent)
        {
            if (remainingNodes.count(nodeIndex) == 0) return;
            remainingNodes.erase(nodeIndex);
            curComponent.push_back(&_nodes[nodeIndex]);
            for (UINT edgeIndex : _nodes[nodeIndex].edges)
            {
                const Edge &e = _edges[edgeIndex];
                if (edgeTest(e.data))
                    visitNode(e.getNeighbor(nodeIndex), curComponent);
            }
        };

        vector< vector<const Node*> > result;

        UINT componentIndex = 0;
        while (remainingNodes.size() > 0)
        {
            result.push_back(vector<const Node*>());
            int seedNodeIndex = *remainingNodes.begin();
            visitNode(seedNodeIndex, result.back());
        }

        return result;
    }

    bool hasEdge(UINT n0, UINT n1) const
    {
        if (n0 > n1) std::swap(n0, n1);
        const Node &node0 = _nodes[n0];
        const UINT edgeCount = (UINT)node0.edges.size();
        for (UINT edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++)
        {
            const Edge &edge = _edges[node0.edges[edgeIndex]];
            if (edge.node0 == n0 && edge.node1 == n1)
                return true;
        }
        return false;
    }

    Edge& getEdge(UINT n0, UINT n1)
    {
        if (n0 > n1) std::swap(n0, n1);
        const Node &node0 = _nodes[n0];
        const UINT edgeCount = (UINT)node0.edges.size();
        for (UINT edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++)
        {
            Edge &edge = _edges[node0.edges[edgeIndex]];
            if (edge.node0 == n0 && edge.node1 == n1)
                return edge;
        }
        return *((Edge*)nullptr);
    }

    void writeToCSV(const std::string &filename, const std::function<std::string(const NodeData&)> &nodeWriter) const
    {
        std::ofstream file(filename);
        const UINT nodeCount = (UINT)_nodes.size();
        file << "Nodes,data,edges";
        for (UINT nodeIndex = 0; nodeIndex < _nodes.size(); nodeIndex++)
            file << "," << nodeIndex;
        file << endl;

        for (UINT nodeIndexA = 0; nodeIndexA < nodeCount; nodeIndexA++)
        {
            file << nodeIndexA << ",";
            file << nodeWriter(_nodes[nodeIndexA].data) << ",";
            file << _nodes[nodeIndexA].edges.size();
            for (UINT nodeIndexB = 0; nodeIndexB < nodeCount; nodeIndexB++)
            {
                if (hasEdge(nodeIndexA, nodeIndexB))
                    file << ",1";
                else
                    file << ",0";
            }
            file << endl;
        }
    }

private:
    vector<Node> _nodes;
    vector<Edge> _edges;
};

typedef UndirectedGraph<int, int> PlainUndirectedGraph;

}


#endif
