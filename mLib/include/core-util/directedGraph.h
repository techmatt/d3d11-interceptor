
#ifndef CORE_UTIL_GRAPH_H_
#define CORE_UTIL_GRAPH_H_

namespace ml {

template<class NodeData, class EdgeData>
class DirectedGraph
{
public:
    struct Edge
    {
        size_t startIndex, endIndex;
        double weight;
        EdgeData data;
    };
    struct Node
    {
        size_t index;
        NodeData data;
        vector<Edge> edges;
    };

    const vector<Node>& nodes()
    {
        return _nodes;
    }

    void addNode(const NodeData &data)
    {
        Node newNode;
        newNode.index = _nodes.size();
        newNode.data = data;
        _nodes.push_back(newNode);
    }

    void addDirectedEdge(int startNode, int endNode, float weight = 1.0, const EdgeData &data = EdgeData())
    {
        Edge newEdge;
        newEdge.startIndex = startNode;
        newEdge.endIndex = endNode;
        newEdge.weight = weight;
        newEdge.data = data;
        _nodes[startNode].edges.push_back(newEdge);
    }

    void addUndirectedEdge(int nodeA, int nodeB, float weight = 1.0, const EdgeData &data = EdgeData())
    {
        Edge newEdge;
        newEdge.startIndex = nodeA;
        newEdge.endIndex = nodeB;
        newEdge.weight = weight;
        newEdge.data = data;
        _nodes[nodeA].edges.push_back(newEdge);

        newEdge.startIndex = nodeB;
        newEdge.endIndex = nodeA;
        _nodes[nodeB].edges.push_back(newEdge);
    }

    vector< vector<int> > computeWeaklyConnectedComponents() const
    {
        const int n = _nodes.size();
        set<int> remainingNodes;

        for (int i = 0; i < n; i++)
            remainingNodes.insert(i);

        auto visitNode = [](int nodeIndex, vector<int> &curComponent)
        {
            if (remainingNodes.count(nodeIndex) == 0) return;
            remainingNodes.insert(nodeIndex);
            curComponent.push_back(nodeIndex);
            result[nodeIndex] = componentIndex;
            for (const Edge &e : _nodes[nodeIndex].edges)
            {
                visitNode(e.endIndex, componentIndex);
            }
        };

        vector< vector<int> > result;

        int componentIndex = 0;
        while (remainingNodes.size() > 0)
        {
            result.push_back(vector<int>());
            int seedNodeIndex = *remainingNodes.begin();
            visitNode(seedNodeIndex, result.back());
        }

        return result;
    }

private:
    vector<Node> _nodes;
};

typedef DirectedGraph<int, int> PlainDirectedGraph;

}


#endif
