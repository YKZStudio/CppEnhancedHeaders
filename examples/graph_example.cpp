// graph 算法使用示例
#include <ceh/algorithms/graph.hpp>
#include <iostream>

int main() {
    using ceh::Graph;

    // 有向加权图
    Graph g(5, true);
    g.add_edge(0, 1, 2);
    g.add_edge(0, 2, 5);
    g.add_edge(1, 2, 1);
    g.add_edge(1, 3, 7);
    g.add_edge(2, 4, 3);
    g.add_edge(4, 3, 1);

    std::cout << "从 0 出发的最短距离: ";
    for (long long d : g.dijkstra(0))
        std::cout << (d == Graph::INF ? -1 : d) << ' ';
    std::cout << "\n";

    std::cout << "拓扑序: ";
    for (int v : g.topological_sort()) std::cout << v << ' ';
    std::cout << "\n";

    Graph u(6);
    u.add_edge(0, 1); u.add_edge(1, 2); u.add_edge(3, 4);
    std::cout << "无向图连通分量数: " << u.connected_components() << "\n";
    return 0;
}
