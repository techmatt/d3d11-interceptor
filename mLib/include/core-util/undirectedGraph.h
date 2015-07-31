
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

    vector< vector<UINT> > computeConnectedComponents(const std::function<bool(const EdgeData&)> &edgeTest) const
    {
        const UINT n = (UINT)_nodes.size();
        set<UINT> remainingNodes;

        for (UINT i = 0; i < n; i++)
            remainingNodes.insert(i);

        std::function<void(UINT, vector<UINT>&)> visitNode = [&](UINT nodeIndex, vector<UINT> &curComponent)
        {
            if (remainingNodes.count(nodeIndex) == 0) return;
            remainingNodes.erase(nodeIndex);
            curComponent.push_back(nodeIndex);
            for (UINT edgeIndex : _nodes[nodeIndex].edges)
            {
                const Edge &e = _edges[edgeIndex];
                if (edgeTest(e.data))
                    visitNode(e.getNeighbor(nodeIndex), curComponent);
            }
        };

        vector< vector<UINT> > result;

        UINT componentIndex = 0;
        while (remainingNodes.size() > 0)
        {
            result.push_back(vector<UINT>());
            int seedNodeIndex = *remainingNodes.begin();
            visitNode(seedNodeIndex, result.back());
        }

        return result;
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

private:
    vector<Node> _nodes;
    vector<Edge> _edges;
};

typedef UndirectedGraph<int, int> PlainUndirectedGraph;

}


#endif
